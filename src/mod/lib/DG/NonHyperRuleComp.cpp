//
//template< typename T >
//struct always_false {
//
//	enum {
//		value = false
//	};
//};
//
//namespace std {
//
//template<>
//struct less<const mod::lib::Graph::Single*> {
//
//	template<typename T>
//	bool operator()(const mod::lib::Graph::Single* a, const mod::lib::Graph::Single * b) {
//		static_assert(always_false<T>::value, "test");
//		return a < b;
//	}
//};
//
//} // namespace std

#include "NonHyperRuleComp.h"

#include <mod/Config.h>
#include <mod/DG.h>
#include <mod/Derivation.h>
#include <mod/Function.h>
#include <mod/lib/DG/Strategies/GraphState.h>
#include <mod/lib/DG/Strategies/Strategy.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/IO/IO.h>

#include <boost/foreach.hpp>

namespace mod {
namespace lib {
namespace DG {

struct NonHyperRuleComp::ExecutionEnv : public Strategies::ExecutionEnv {

	ExecutionEnv(NonHyperRuleComp &owner)
	: owner(owner) { }

	bool addGraph(std::shared_ptr<mod::Graph> g) {
		return owner.addGraph(g);
	}

	bool addGraphAsVertex(std::shared_ptr<mod::Graph> g) {
		return owner.addGraphAsVertex(g);
	}

	bool doExit() const {
		return owner.doExit;
	}

	bool checkLeftPredicate(const mod::Derivation &d) const {

		BOOST_REVERSE_FOREACH(std::shared_ptr < mod::Function<bool(const mod::Derivation&)> > pred, owner.leftPredicates) {
			bool result = (*pred)(d);
			if(!result) return false;
		}
		return true;
	}

	bool checkRightPredicate(const mod::Derivation &d) const {

		BOOST_REVERSE_FOREACH(std::shared_ptr < mod::Function<bool(const mod::Derivation&)> > pred, owner.rightPredicates) {
			bool result = (*pred)(d);
			if(!result) return false;
		}
		return true;
	}

	std::shared_ptr<mod::Graph> checkIfNew(std::unique_ptr<lib::Graph::GraphType> g, std::unique_ptr<lib::Graph::PropString> pString) const {
		return owner.checkIfNew(std::move(g), std::move(pString)).first;
	}

	void giveProductStatus(std::shared_ptr<mod::Graph> g) {
		owner.giveProductStatus(g);
	}

	bool addProduct(std::shared_ptr<mod::Graph> g) {
		bool isProduct = owner.addProduct(g);
		if(owner.getProducts().size() >= getConfig().dg.productLimit.get()) {
			IO::log() << "DG::RuleComp:\tproduct limit reached, aborting rest of rule application" << std::endl
					<< "\t(further rule application in the strategy is skipped)" << std::endl;
			owner.doExit = true;
		}
		return isProduct;
	}

	const lib::Graph::Merge * addToMergeStore(const lib::Graph::Merge *g) {
		return owner.addToMergeStore(g);
	}

	bool isDerivation(const lib::Graph::Base *left, const lib::Graph::Base *right, const lib::Rules::Real *r) const {
		return owner.isDerivation(left, right, r).second;
	}

	bool suggestDerivation(const lib::Graph::Base *left, const lib::Graph::Base *right, const lib::Rules::Real *r) {
		return owner.suggestDerivation(left, right, r).second;
	}

	void pushLeftPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > pred) {
		owner.leftPredicates.push_back(pred);
	}

	void pushRightPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > pred) {
		owner.rightPredicates.push_back(pred);
	}

	void popLeftPredicate() {
		owner.leftPredicates.pop_back();
	}

	void popRightPredicate() {
		owner.rightPredicates.pop_back();
	}
public:

	void fillDerivationRefs(std::vector<mod::DerivationRef>& refs) const {
		refs = owner.getAllDerivationRefs();
	}
public:
	NonHyperRuleComp &owner;
};

NonHyperRuleComp::NonHyperRuleComp(const std::vector<std::shared_ptr<mod::Graph> > &graphDatabase,
		Strategies::Strategy *strategy)
: NonHyper(graphDatabase), strategy(strategy), input(new Strategies::GraphState()), doExit(false) {
	env.reset(new ExecutionEnv(*this));
	strategy->setExecutionEnv(*env);
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
