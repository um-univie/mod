#include "DGStrat.h"

#include <mod/DG.h>
#include <mod/Rule.h>
#include <mod/lib/DG/Strategies/Strategy.h>
#include <mod/lib/DG/Strategies/Add.h>
#include <mod/lib/DG/Strategies/DerivationPredicates.h>
#include <mod/lib/DG/Strategies/Execute.h>
#include <mod/lib/DG/Strategies/Filter.h>
#include <mod/lib/DG/Strategies/Parallel.h>
#include <mod/lib/DG/Strategies/Repeat.h>
#include <mod/lib/DG/Strategies/Revive.h>
#include <mod/lib/DG/Strategies/Rule.h>
#include <mod/lib/DG/Strategies/Sequence.h>
#include <mod/lib/DG/Strategies/Sort.h>
#include <mod/lib/DG/Strategies/Take.h>
#include <mod/lib/IO/IO.h>

#include <jla_boost/Memory.hpp>

#include <ostream>

namespace mod {
//------------------------------------------------------------------------------
// GraphState
//------------------------------------------------------------------------------

DGStrat::GraphState::GraphState(std::function<void(std::vector<std::shared_ptr<Graph> >&) > fSubset,
		std::function<void(std::vector<std::shared_ptr<Graph> >&) > fUniverse,
		std::function<void(std::vector<DerivationRef>&) > fDerivationRefs)
: subsetInit(false), universeInit(false), derivationRefsInit(false),
fSubset(fSubset), fUniverse(fUniverse), fDerivationRefs(fDerivationRefs) { }

const std::vector<std::shared_ptr<Graph> > &DGStrat::GraphState::getSubset() const {
	if(!subsetInit) {
		fSubset(subset);
		subsetInit = true;
	}
	return subset;
}

const std::vector<std::shared_ptr<Graph> > &DGStrat::GraphState::getUniverse() const {
	if(!universeInit) {
		fUniverse(universe);
		universeInit = true;
	}
	return universe;
}

const std::vector<DerivationRef> &DGStrat::GraphState::getDerivationRefs() const {
	if(!derivationRefsInit) {
		fDerivationRefs(derivationRefs);
		derivationRefsInit = true;
	}
	return derivationRefs;
}

//------------------------------------------------------------------------------
// DGStrat
//------------------------------------------------------------------------------

struct DGStrat::Pimpl {
	Pimpl(std::unique_ptr<lib::DG::Strategies::Strategy> strategy);
public:
	const std::unique_ptr<lib::DG::Strategies::Strategy> strategy;
};

DGStrat::DGStrat(std::unique_ptr<lib::DG::Strategies::Strategy> strategy) : p(new Pimpl(std::move(strategy))) { }

DGStrat::~DGStrat() { }

std::unique_ptr<DGStrat> DGStrat::clone() const {
	return std::unique_ptr<DGStrat>(new DGStrat(std::unique_ptr<lib::DG::Strategies::Strategy>(p->strategy->clone())));
}

std::ostream &operator<<(std::ostream &s, const DGStrat &strat) {
	return s << "DGStrat";
}

lib::DG::Strategies::Strategy &DGStrat::getStrategy() {
	return *p->strategy;
}

//------------------------------------------------------------------------------
// Pimpl impl
//------------------------------------------------------------------------------

DGStrat::Pimpl::Pimpl(std::unique_ptr<lib::DG::Strategies::Strategy> strategy) : strategy(std::move(strategy)) {
	assert(this->strategy);
}

//------------------------------------------------------------------------------
// Static
//------------------------------------------------------------------------------

std::shared_ptr<DGStrat> DGStrat::makeAdd(bool onlyUniverse, const std::vector<std::shared_ptr<Graph> > &graphs) {
	return std::shared_ptr<DGStrat>(new DGStrat(make_unique<lib::DG::Strategies::Add>(graphs, onlyUniverse)));
}

std::shared_ptr<DGStrat> DGStrat::makeAdd(bool onlyUniverse, const std::shared_ptr<mod::Function<std::vector<std::shared_ptr<mod::Graph> >() > > generator) {
	return std::shared_ptr<DGStrat>(new DGStrat(make_unique<lib::DG::Strategies::Add>(generator, onlyUniverse)));
}

std::shared_ptr<DGStrat> DGStrat::makeExecute(std::shared_ptr<mod::Function<void(const mod::DGStrat::GraphState&)> > func) {
	return std::shared_ptr<DGStrat>(new DGStrat(make_unique<lib::DG::Strategies::Execute>(func)));
}

std::shared_ptr<DGStrat> DGStrat::makeFilter(bool alsoUniverse,
		std::shared_ptr<mod::Function<bool(std::shared_ptr<mod::Graph>, const mod::DGStrat::GraphState&, bool)> > filterFunc) {
	return std::shared_ptr<DGStrat>(new DGStrat(make_unique<lib::DG::Strategies::Filter>(filterFunc, alsoUniverse)));
}

std::shared_ptr<DGStrat> DGStrat::makeLeftPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > predicate,
		std::shared_ptr<DGStrat> strat) {
	return std::shared_ptr<DGStrat>(new DGStrat(make_unique<lib::DG::Strategies::LeftPredicate>(predicate, strat->getStrategy().clone())));
}

std::shared_ptr<DGStrat> DGStrat::makeParallel(const std::vector<std::shared_ptr<DGStrat> > &strategies) {
	if(strategies.empty()) {
		throw LogicError("Parallel strategy with empty list of substrategies.");
	}
	std::vector<lib::DG::Strategies::Strategy*> cloned;
	for(std::shared_ptr<DGStrat> strat : strategies) cloned.push_back(strat->getStrategy().clone());
	return std::shared_ptr<DGStrat>(new DGStrat(make_unique<lib::DG::Strategies::Parallel>(cloned)));
}

std::shared_ptr<DGStrat> DGStrat::makeRepeat(std::size_t limit, std::shared_ptr<DGStrat> strategy) {
	return std::shared_ptr<DGStrat>(new DGStrat(make_unique<lib::DG::Strategies::Repeat>(strategy->getStrategy().clone(), limit)));
}

std::shared_ptr<DGStrat> DGStrat::makeRevive(std::shared_ptr<DGStrat> strategy) {
	return std::shared_ptr<DGStrat>(new DGStrat(make_unique<lib::DG::Strategies::Revive>(strategy->getStrategy().clone())));
}

std::shared_ptr<DGStrat> DGStrat::makeRightPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > predicate,
		std::shared_ptr<DGStrat> strat) {
	return std::shared_ptr<DGStrat>(new DGStrat(make_unique<lib::DG::Strategies::RightPredicate>(predicate, strat->getStrategy().clone())));
}

std::shared_ptr<DGStrat> DGStrat::makeRule(std::shared_ptr<Rule> rule) {
	return std::shared_ptr<DGStrat>(new DGStrat(make_unique<lib::DG::Strategies::Rule>(rule)));
}

std::shared_ptr<DGStrat> DGStrat::makeSequence(const std::vector<std::shared_ptr<DGStrat> > &strategies) {
	std::vector< lib::DG::Strategies::Strategy*> cloned;
	for(std::shared_ptr<DGStrat> strat : strategies) cloned.push_back(strat->getStrategy().clone());
	return std::shared_ptr<DGStrat>(new DGStrat(make_unique<lib::DG::Strategies::Sequence>(cloned)));
}

std::shared_ptr<DGStrat> DGStrat::makeSort(bool doUniverse,
		std::shared_ptr<mod::Function<bool(std::shared_ptr<mod::Graph>, std::shared_ptr<mod::Graph>, const mod::DGStrat::GraphState&)> > less) {
	return std::shared_ptr<DGStrat>(new DGStrat(make_unique<lib::DG::Strategies::Sort>(less, doUniverse)));
}

std::shared_ptr<DGStrat> DGStrat::makeTake(bool doUniverse, unsigned int limit) {
	return std::shared_ptr<DGStrat>(new DGStrat(make_unique<lib::DG::Strategies::Take>(limit, doUniverse)));
}

} // namespace mod