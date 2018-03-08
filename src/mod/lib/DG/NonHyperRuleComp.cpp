#include "NonHyperRuleComp.h"

#include <mod/Config.h>
#include <mod/Function.h>
#include <mod/dg/GraphInterface.h>
#include <mod/Derivation.h>
#include <mod/lib/DG/Strategies/GraphState.h>
#include <mod/lib/DG/Strategies/Strategy.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/Graph/Properties/Term.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/Rules/Real.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

namespace mod {
namespace lib {
namespace DG {

struct NonHyperRuleComp::ExecutionEnv : public Strategies::ExecutionEnv {

	ExecutionEnv(NonHyperRuleComp &owner, LabelSettings labelSettings)
	: Strategies::ExecutionEnv(labelSettings), owner(owner) { }

	bool tryAddGraph(std::shared_ptr<graph::Graph> gCand) override {
		const auto ls = LabelSettings{owner.getLabelSettings().type, LabelRelation::Isomorphism, owner.getLabelSettings().withStereo, LabelRelation::Isomorphism};
		for(const auto &g : owner.getGraphDatabase()) {
			if(g == gCand) break;
			const bool equal = gCand->isomorphism(g, 1, ls);
			if(equal) {
				std::string msg = "Isomorphic graphs '" + g->getName() + "' and '" + gCand->getName() + "' in initial graph database and/or add strategies.";
				throw LogicError(std::move(msg));
			}
		}
		if(ls.type == LabelType::Term) {
			const auto &term = get_term(gCand->getGraph().getLabelledGraph());
			if(!isValid(term)) {
				std::string msg = "Parsing failed for graph '" + gCand->getName() + "' in dynamic add strategy. " + term.getParsingError();
				throw TermParsingError(std::move(msg));
			}
		}
		return addGraph(gCand);
	}

	bool addGraph(std::shared_ptr<graph::Graph> g) override {
		return owner.addGraph(g);
	}

	bool addGraphAsVertex(std::shared_ptr<graph::Graph> g) override {
		return owner.addGraphAsVertex(g);
	}

	bool doExit() const override {
		return owner.doExit;
	}

	bool checkLeftPredicate(const mod::Derivation &d) const override {

		BOOST_REVERSE_FOREACH(std::shared_ptr < mod::Function<bool(const mod::Derivation&)> > pred, owner.leftPredicates) {
			bool result = (*pred)(d);
			if(!result) return false;
		}
		return true;
	}

	bool checkRightPredicate(const mod::Derivation &d) const override {

		BOOST_REVERSE_FOREACH(std::shared_ptr < mod::Function<bool(const mod::Derivation&)> > pred, owner.rightPredicates) {
			bool result = (*pred)(d);
			if(!result) return false;
		}
		return true;
	}

	std::shared_ptr<graph::Graph> checkIfNew(std::unique_ptr<lib::Graph::Single> g) const {
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
			owner.doExit = true;
		}
		return isProduct;
	}

	bool isDerivation(const GraphMultiset &gmsSrc, const GraphMultiset &gmsTar, const lib::Rules::Real *r) const override {
		return owner.isDerivation(gmsSrc, gmsTar, r).second;
	}

	bool suggestDerivation(const GraphMultiset &gmsSrc, const GraphMultiset &gmsTar, const lib::Rules::Real *r) override {
		return owner.suggestDerivation(gmsSrc, gmsTar, r).second;
	}

	void pushLeftPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > pred) override {
		owner.leftPredicates.push_back(pred);
	}

	void pushRightPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > pred) override {
		owner.rightPredicates.push_back(pred);
	}

	void popLeftPredicate() override {
		owner.leftPredicates.pop_back();
	}

	void popRightPredicate() override {
		owner.rightPredicates.pop_back();
	}
public:

	void fillHyperEdges(std::vector<dg::DG::HyperEdge> &edges) const override {
		edges = owner.getAllHyperEdges();
	}
public:
	NonHyperRuleComp &owner;
};

NonHyperRuleComp::NonHyperRuleComp(const std::vector<std::shared_ptr<graph::Graph> > &graphDatabase,
		Strategies::Strategy *strategy, LabelSettings labelSettings, bool ignoreRuleLabelTypes)
: NonHyper(graphDatabase, labelSettings), strategy(strategy), input(new Strategies::GraphState()), doExit(false) {
	env.reset(new ExecutionEnv(*this, labelSettings));
	strategy->setExecutionEnv(*env);
	const auto ls = LabelSettings{getLabelSettings().type, LabelRelation::Isomorphism, getLabelSettings().withStereo, LabelRelation::Isomorphism};
	strategy->preAddGraphs([this, ls](std::shared_ptr<graph::Graph> gCand) {
		if(!getConfig().dg.skipInitialGraphIsomorphismCheck.get()) {
			for(const auto &g : getGraphDatabase()) {
				if(g == gCand) break;
				const bool equal = gCand->isomorphism(g, 1, ls);
				if(equal) {
					std::string msg = "Isomorphic graphs '" + g->getName() + "' and '" + gCand->getName() + "' in initial graph database and/or add strategies.";
					throw LogicError(std::move(msg));
				}
			}
		}
		if(ls.type == LabelType::Term) {
			const auto &term = get_term(gCand->getGraph().getLabelledGraph());
			if(!isValid(term)) {
				std::string msg = "Parsing failed for graph '" + gCand->getName() + "' in static add strategy. " + term.getParsingError();
				throw TermParsingError(std::move(msg));
			}
		}
		addGraph(gCand);
	});
	if(!ignoreRuleLabelTypes) {
		strategy->forEachRule([&](const lib::Rules::Real &r) {
			if(!r.getLabelType()) return;
			if(*r.getLabelType() != labelSettings.type) {
				std::string msg = "Rule '" + r.getName() + "' has intended label type " + boost::lexical_cast<std::string>(*r.getLabelType());
				msg += ", but the DG is using " + boost::lexical_cast<std::string>(labelSettings.type) + ".";
				msg += " Set the ignoreRuleLabelTypes argument to true to skip this check.";
				throw LogicError(std::move(msg));
			}
		});
	}
}

NonHyperRuleComp::~NonHyperRuleComp() { }

std::string NonHyperRuleComp::getType() const {
	return "DGRuleComp";
}

void NonHyperRuleComp::calculateImpl() {
	if(getHasCalculated()) return;
	strategy->execute(IO::log(), *input);
}

void NonHyperRuleComp::listImpl(std::ostream &s) const {
	printStrategyInfo(s);
}

void NonHyperRuleComp::printStrategyInfo(std::ostream& s) const {
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
