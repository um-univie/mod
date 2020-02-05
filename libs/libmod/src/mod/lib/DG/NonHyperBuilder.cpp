#include "NonHyperBuilder.hpp"

#include <mod/Error.hpp>
#include <mod/Function.hpp>
#include <mod/Misc.hpp>
#include <mod/rule/Rule.hpp>
#include <mod/lib/DG/RuleApplicationUtils.hpp>
#include <mod/lib/DG/Strategies/GraphState.hpp>
#include <mod/lib/DG/Strategies/Strategy.hpp>
#include <mod/lib/Graph/Properties/Term.hpp>
#include <mod/lib/IO/DG.hpp>
#include <mod/lib/IO/IO.hpp>
#include <mod/lib/RC/ComposeRuleReal.hpp>
#include <mod/lib/RC/MatchMaker/Super.hpp>

#include <boost/lexical_cast.hpp>

namespace mod {
namespace lib {
namespace DG {

ExecuteResult::ExecuteResult(NonHyperBuilder *owner, int execution)
		: owner(owner), execution(execution) {}

const Strategies::GraphState &ExecuteResult::getResult() const {
	return owner->executions[execution].strategy->getOutput();
}

void ExecuteResult::list(bool withUniverse) const {
	owner->executions[execution].strategy->printInfo(
			Strategies::PrintSettings(IO::log(), withUniverse));
}

// -----------------------------------------------------------------------------

Builder::Builder(NonHyperBuilder *dg) : dg(dg) {
	if(dg->getHasCalculated()) {
		this->dg = nullptr;
		throw LogicError(dg->getType() + ": has already been build.");
	}
	dg->calculatePrologue();
}

Builder::Builder(Builder &&other) : dg(other.dg) {
	other.dg = nullptr;
}

Builder &Builder::operator=(Builder &&other) {
	if(&other == this) return *this;
	dg = other.dg;
	other.dg = nullptr;
	return *this;
}

Builder::~Builder() {
	if(dg) dg->calculateEpilogue();
}

std::pair<NonHyper::Edge, bool> Builder::addDerivation(const Derivations &d, IsomorphismPolicy graphPolicy) {
	assert(!d.left.empty());
	assert(!d.right.empty());
	// add graphs
	switch(graphPolicy) {
	case IsomorphismPolicy::Check: {
		for(const auto &g : d.left)
			dg->tryAddGraph(g);
		for(const auto &g : d.right)
			dg->tryAddGraph(g);
		break;
	}
	case IsomorphismPolicy::TrustMe:
		for(const auto &g : d.left)
			dg->trustAddGraph(g);
		for(const auto &g : d.right)
			dg->trustAddGraph(g);
		break;
	}
	// add hyperedges
	const auto makeSide = [](const mod::Derivation::GraphList &graphs) -> GraphMultiset {
		std::vector<const lib::Graph::Single *> gPtrs;
		gPtrs.reserve(graphs.size());
		for(const auto &g : graphs) gPtrs.push_back(&g->getGraph());
		return GraphMultiset(std::move(gPtrs));
	};
	auto gmsLeft = makeSide(d.left);
	auto gmsRight = makeSide(d.right);
	dg->rules.insert(d.rules.begin(), d.rules.end());
	if(d.rules.size() <= 1) {
		const lib::Rules::Real *rule = nullptr;
		if(!d.rules.empty()) rule = &d.rules.front()->getRule();
		return dg->suggestDerivation(std::move(gmsLeft), std::move(gmsRight), rule);
	} else {
		auto res = dg->suggestDerivation(gmsLeft, gmsRight, &d.rules.front()->getRule());
		for(const auto &r : asRange(d.rules.begin() + 1, d.rules.end()))
			dg->suggestDerivation(gmsLeft, gmsRight, &r->getRule());
		return res;
	}
}

struct NonHyperBuilder::ExecutionEnv final : public Strategies::ExecutionEnv {
	ExecutionEnv(NonHyperBuilder &owner, LabelSettings labelSettings)
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
		for(const auto &pred : asRange(leftPredicates.rbegin(), leftPredicates.rend())) {
			bool result = (*pred)(d);
			if(!result) return false;
		}
		return true;
	}

	bool checkRightPredicate(const mod::Derivation &d) const override {
		for(const auto &pred : asRange(rightPredicates.rbegin(), rightPredicates.rend())) {
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
	NonHyperBuilder &owner;
private: // state for computation
	std::vector<std::shared_ptr<mod::Function<bool(const mod::Derivation &)> > > leftPredicates;
	std::vector<std::shared_ptr<mod::Function<bool(const mod::Derivation &)> > > rightPredicates;
	bool doExit_ = false;
};

ExecuteResult
Builder::execute(std::unique_ptr<Strategies::Strategy> strategy_, int verbosity, bool ignoreRuleLabelTypes) {
	NonHyperBuilder::StrategyExecution exec{
			std::make_unique<NonHyperBuilder::ExecutionEnv>(*dg, dg->getLabelSettings()),
			std::make_unique<Strategies::GraphState>(),
			std::move(strategy_)
	};
	exec.strategy->setExecutionEnv(*exec.env);

	exec.strategy->preAddGraphs(
			[this](std::shared_ptr<graph::Graph> gCand, IsomorphismPolicy graphPolicy) {
				if(dg->getLabelSettings().type == LabelType::Term) {
					const auto &term = get_term(gCand->getGraph().getLabelledGraph());
					if(!isValid(term)) {
						std::string msg = "Parsing failed for graph '" + gCand->getName() + "' in static add strategy. " +
												term.getParsingError();
						throw TermParsingError(std::move(msg));
					}
				}
				switch(graphPolicy) {
				case IsomorphismPolicy::Check: dg->tryAddGraph(gCand);
					break;
				case IsomorphismPolicy::TrustMe: dg->trustAddGraph(gCand);
					break;
				}
			}
	);
	if(!ignoreRuleLabelTypes) {
		exec.strategy->forEachRule([&](const lib::Rules::Real &r) {
			if(!r.getLabelType()) return;
			if(*r.getLabelType() != dg->getLabelSettings().type) {
				std::string msg = "Rule '" + r.getName() + "' has intended label type '" +
										boost::lexical_cast<std::string>(*r.getLabelType()) + "', but the DG is using '" +
										boost::lexical_cast<std::string>(dg->getLabelSettings().type) + "'.";
				throw LogicError(std::move(msg));
			}
		});
	}

	exec.strategy->execute(Strategies::PrintSettings(IO::log(), false, verbosity), *exec.input);
	dg->executions.push_back(std::move(exec));
	return ExecuteResult(dg, dg->executions.size() - 1);
}

void Builder::addAbstract(const std::string &description) {
	std::ostringstream err;
	auto res = lib::IO::DG::Read::abstract(description, err);
	if(!res) throw InputError("Could not parse description of abstract derivations.\n" + err.str());
	const auto &derivations = *res;
	std::unordered_map<std::string, std::shared_ptr<graph::Graph> > strToGraph;
	const auto handleSide = [this, &strToGraph](const lib::IO::DG::Read::AbstractDerivation::List &side) {
		for(const auto &e : side) {
			const auto iter = strToGraph.find(e.second);
			if(iter != end(strToGraph)) continue;
			auto gBoost = std::make_unique<lib::Graph::GraphType>();
			auto pString = std::make_unique<lib::Graph::PropString>(*gBoost);
			auto gLib = std::make_unique<lib::Graph::Single>(std::move(gBoost), std::move(pString), nullptr);
			auto g = graph::Graph::makeGraph(std::move(gLib));
			dg->addProduct(g); // this renames it
			g->setName(e.second);
			strToGraph[e.second] = g;
		}
	};
	for(const auto &der : derivations) {
		handleSide(der.left);
		handleSide(der.right);
	}

	using Side = std::unordered_map<std::shared_ptr<graph::Graph>, unsigned int>;
	const auto makeSide = [&strToGraph](const lib::IO::DG::Read::AbstractDerivation::List &side) {
		Side result;
		for(const auto &e : side) {
			const auto g = strToGraph[e.second];
			assert(g);
			auto iter = result.find(g);
			if(iter == end(result)) iter = result.insert(std::make_pair(g, 0)).first;
			iter->second += e.first;
		}
		return result;
	};
	for(const auto &der : derivations) {
		const Side left = makeSide(der.left);
		const Side right = makeSide(der.right);
		std::vector<const lib::Graph::Single *> leftGraphs, rightGraphs;
		for(const auto &e : left) {
			for(unsigned int i = 0; i < e.second; i++)
				leftGraphs.push_back(&e.first->getGraph());
		}
		for(const auto &e : right) {
			for(unsigned int i = 0; i < e.second; i++)
				rightGraphs.push_back(&e.first->getGraph());
		}
		lib::DG::GraphMultiset gmsLeft(std::move(leftGraphs)), gmsRight(std::move(rightGraphs));
		dg->suggestDerivation(gmsLeft, gmsRight, nullptr);
		if(der.reversible)
			dg->suggestDerivation(gmsRight, gmsLeft, nullptr);
	}
}

// -----------------------------------------------------------------------------

NonHyperBuilder::NonHyperBuilder(LabelSettings labelSettings,
											const std::vector<std::shared_ptr<graph::Graph> > &graphDatabase,
											IsomorphismPolicy graphPolicy)
		: NonHyper(labelSettings, graphDatabase, graphPolicy) {}

NonHyperBuilder::~NonHyperBuilder() = default;

std::string NonHyperBuilder::getType() const {
	return "DG";
}

Builder NonHyperBuilder::build() {
	if(getHasCalculated()) throw LogicError(getType() + ": has already been build.");
	return Builder(this);
}

} // namespace DG
} // namespace lib
} // namespace mod
