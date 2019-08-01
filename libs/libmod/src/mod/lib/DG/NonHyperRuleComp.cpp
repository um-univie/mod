#include "NonHyperRuleComp.hpp"

#include <mod/Config.hpp>
#include <mod/Function.hpp>
#include <mod/dg/GraphInterface.hpp>
#include <mod/Derivation.hpp>
#include <mod/lib/DG/Strategies/GraphState.hpp>
#include <mod/lib/DG/Strategies/Strategy.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/Graph/Properties/String.hpp>
#include <mod/lib/Graph/Properties/Term.hpp>
#include <mod/lib/IO/IO.hpp>
#include <mod/lib/Rules/Real.hpp>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

namespace mod {
namespace lib {
namespace DG {

struct NonHyperRuleComp::ExecutionEnv : public Strategies::ExecutionEnv {

	ExecutionEnv(NonHyperRuleComp &owner, LabelSettings labelSettings)
			: Strategies::ExecutionEnv(labelSettings), owner(owner) {}

	void tryAddGraph(std::shared_ptr<graph::Graph> gCand) override {
		owner.tryAddGraph(gCand);
	}

	bool trustAddGraph(std::shared_ptr<graph::Graph> g) override {
		return owner.trustAddGraph(g);
	}

	bool trustAddGraphAsVertex(std::shared_ptr<graph::Graph> g) override {
		return owner.trustAddGraphAsVertex(g);
	}

	bool doExit() const override {
		return doExit_;
	}

	bool checkLeftPredicate(const mod::Derivation &d) const override {

		BOOST_REVERSE_FOREACH(std::shared_ptr<mod::Function<bool(const mod::Derivation &)> > pred, leftPredicates) {
						bool result = (*pred)(d);
						if(!result) return false;
					}
		return true;
	}

	bool checkRightPredicate(const mod::Derivation &d) const override {

		BOOST_REVERSE_FOREACH(std::shared_ptr<mod::Function<bool(const mod::Derivation &)> > pred, rightPredicates) {
						bool result = (*pred)(d);
						if(!result) return false;
					}
		return true;
	}

	virtual std::shared_ptr<graph::Graph> checkIfNew(std::unique_ptr<lib::Graph::Single> g) const override {
		return owner.checkIfNew(std::move(g)).first;
	}

	void giveProductStatus(std::shared_ptr<graph::Graph> g) override {
		owner.giveProductStatus(g);
	}

	bool addProduct(std::shared_ptr<graph::Graph> g) override {
		bool isProduct = owner.addProduct(g);
		if(owner.getProducts().size() >= getConfig().dg.productLimit.get()) {
			IO::log() << "DG::RuleComp:\tproduct limit reached, aborting rest of rule application" << std::endl
						 << "\t(further rule application in the strategy is skipped)" << std::endl;
			doExit_ = true;
		}
		return isProduct;
	}

	bool isDerivation(const GraphMultiset &gmsSrc,
							const GraphMultiset &gmsTar,
							const lib::Rules::Real *r) const override {
		return owner.isDerivation(gmsSrc, gmsTar, r).second;
	}

	bool suggestDerivation(const GraphMultiset &gmsSrc,
								  const GraphMultiset &gmsTar,
								  const lib::Rules::Real *r) override {
		return owner.suggestDerivation(gmsSrc, gmsTar, r).second;
	}

	void pushLeftPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation &)> > pred) override {
		leftPredicates.push_back(pred);
	}

	void pushRightPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation &)> > pred) override {
		rightPredicates.push_back(pred);
	}

	void popLeftPredicate() override {
		leftPredicates.pop_back();
	}

	void popRightPredicate() override {
		rightPredicates.pop_back();
	}

public:
	NonHyperRuleComp &owner;
private: // state for computation
	std::vector<std::shared_ptr<mod::Function<bool(const mod::Derivation &)> > > leftPredicates;
	std::vector<std::shared_ptr<mod::Function<bool(const mod::Derivation &)> > > rightPredicates;
	bool doExit_ = false;
};

NonHyperRuleComp::NonHyperRuleComp(const std::vector<std::shared_ptr<graph::Graph> > &graphDatabase,
											  Strategies::Strategy *strategy,
											  LabelSettings labelSettings,
											  bool ignoreRuleLabelTypes)
		: NonHyper(graphDatabase, labelSettings), strategy(strategy), input(new Strategies::GraphState()) {
	env.reset(new ExecutionEnv(*this, labelSettings));
	strategy->setExecutionEnv(*env);
	const auto ls = LabelSettings{getLabelSettings().type, LabelRelation::Isomorphism, getLabelSettings().withStereo,
											LabelRelation::Isomorphism};
	strategy->preAddGraphs([this, ls](std::shared_ptr<graph::Graph> gCand) {
		if(ls.type == LabelType::Term) {
			const auto &term = get_term(gCand->getGraph().getLabelledGraph());
			if(!isValid(term)) {
				std::string msg = "Parsing failed for graph '" + gCand->getName() + "' in static add strategy. " +
										term.getParsingError();
				throw TermParsingError(std::move(msg));
			}
		}
		if(!getConfig().dg.skipInitialGraphIsomorphismCheck.get()) {
			const auto g = getGraphDatabase().findIsomorphic(gCand);
			if(g && g != gCand) {
				std::string msg = "Isomorphic graphs '" + g->getName() + "' and '" + gCand->getName() +
										"' in initial graph database and/or add strategies.";
				throw LogicError(std::move(msg));
			}
		}
		trustAddGraph(gCand);
	});
	if(!ignoreRuleLabelTypes) {
		strategy->forEachRule([&](const lib::Rules::Real &r) {
			if(!r.getLabelType()) return;
			if(*r.getLabelType() != labelSettings.type) {
				std::string msg = "Rule '" + r.getName() + "' has intended label type " +
										boost::lexical_cast<std::string>(*r.getLabelType());
				msg += ", but the DG is using " + boost::lexical_cast<std::string>(labelSettings.type) + ".";
				msg += " Set the ignoreRuleLabelTypes argument to true to skip this check.";
				throw LogicError(std::move(msg));
			}
		});
	}
}

NonHyperRuleComp::~NonHyperRuleComp() {}

std::string NonHyperRuleComp::getType() const {
	return "DGRuleComp";
}

void NonHyperRuleComp::calculateImpl(bool printInfo) {
	if(getHasCalculated()) return;
	strategy->execute(printInfo ? IO::log() : IO::nullStream(), *input);
}

void NonHyperRuleComp::listImpl(std::ostream &s) const {
	printStrategyInfo(s);
}

void NonHyperRuleComp::printStrategyInfo(std::ostream &s) const {
	if(!getHasCalculated()) {
		s << "No strategy information to print. Has not calculated yet." << std::endl;
		return;
	}
	strategy->printInfo(s);
}

const Strategies::GraphState &NonHyperRuleComp::getOutput() const {
	return strategy->getOutput();
}

} // namespace DG
} // namespace lib
} // namespace mod
