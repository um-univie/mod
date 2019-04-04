#include "Strategies.h"

#include <mod/Error.h>
#include <mod/rule/Rule.h>
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

#include <ostream>

namespace mod {
namespace dg {

//------------------------------------------------------------------------------
// GraphState
//------------------------------------------------------------------------------

Strategy::GraphState::GraphState(std::function<void(std::vector<std::shared_ptr<graph::Graph> >&) > fSubset,
		std::function<void(std::vector<std::shared_ptr<graph::Graph> >&) > fUniverse)
: subsetInit(false), universeInit(false), fSubset(fSubset), fUniverse(fUniverse) { }

const std::vector<std::shared_ptr<graph::Graph> > &Strategy::GraphState::getSubset() const {
	if(!subsetInit) {
		fSubset(subset);
		subsetInit = true;
	}
	return subset;
}

const std::vector<std::shared_ptr<graph::Graph> > &Strategy::GraphState::getUniverse() const {
	if(!universeInit) {
		fUniverse(universe);
		universeInit = true;
	}
	return universe;
}

//------------------------------------------------------------------------------
// Strategy
//------------------------------------------------------------------------------

struct Strategy::Pimpl {
	Pimpl(std::unique_ptr<lib::DG::Strategies::Strategy> strategy);
public:
	const std::unique_ptr<lib::DG::Strategies::Strategy> strategy;
};

Strategy::Strategy(std::unique_ptr<lib::DG::Strategies::Strategy> strategy) : p(new Pimpl(std::move(strategy))) { }

Strategy::~Strategy() { }

std::unique_ptr<Strategy> Strategy::clone() const {
	return std::unique_ptr<Strategy>(new Strategy(std::unique_ptr<lib::DG::Strategies::Strategy>(p->strategy->clone())));
}

std::ostream &operator<<(std::ostream &s, const Strategy &strat) {
	return s << "Strategy";
}

lib::DG::Strategies::Strategy &Strategy::getStrategy() {
	return *p->strategy;
}

//------------------------------------------------------------------------------
// Pimpl impl
//------------------------------------------------------------------------------

Strategy::Pimpl::Pimpl(std::unique_ptr<lib::DG::Strategies::Strategy> strategy) : strategy(std::move(strategy)) {
	assert(this->strategy);
}

//------------------------------------------------------------------------------
// Static
//------------------------------------------------------------------------------

std::shared_ptr<Strategy> Strategy::makeAdd(bool onlyUniverse, const std::vector<std::shared_ptr<graph::Graph> > &graphs) {
	return std::shared_ptr<Strategy>(new Strategy(std::make_unique<lib::DG::Strategies::Add>(graphs, onlyUniverse)));
}

std::shared_ptr<Strategy> Strategy::makeAdd(bool onlyUniverse, const std::shared_ptr<mod::Function<std::vector<std::shared_ptr<graph::Graph> >() > > generator) {
	return std::shared_ptr<Strategy>(new Strategy(std::make_unique<lib::DG::Strategies::Add>(generator, onlyUniverse)));
}

std::shared_ptr<Strategy> Strategy::makeExecute(std::shared_ptr<mod::Function<void(const Strategy::GraphState&)> > func) {
	return std::shared_ptr<Strategy>(new Strategy(std::make_unique<lib::DG::Strategies::Execute>(func)));
}

std::shared_ptr<Strategy> Strategy::makeFilter(bool alsoUniverse,
		std::shared_ptr<mod::Function<bool(std::shared_ptr<graph::Graph>, const Strategy::GraphState&, bool)> > filterFunc) {
	return std::shared_ptr<Strategy>(new Strategy(std::make_unique<lib::DG::Strategies::Filter>(filterFunc, alsoUniverse)));
}

std::shared_ptr<Strategy> Strategy::makeLeftPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > predicate,
		std::shared_ptr<Strategy> strat) {
	return std::shared_ptr<Strategy>(new Strategy(std::make_unique<lib::DG::Strategies::LeftPredicate>(predicate, strat->getStrategy().clone())));
}

std::shared_ptr<Strategy> Strategy::makeParallel(const std::vector<std::shared_ptr<Strategy> > &strategies) {
	if(strategies.empty()) {
		throw LogicError("Parallel strategy with empty list of substrategies.");
	}
	std::vector<lib::DG::Strategies::Strategy*> cloned;
	for(std::shared_ptr<Strategy> strat : strategies) cloned.push_back(strat->getStrategy().clone());
	return std::shared_ptr<Strategy>(new Strategy(std::make_unique<lib::DG::Strategies::Parallel>(cloned)));
}

std::shared_ptr<Strategy> Strategy::makeRepeat(std::size_t limit, std::shared_ptr<Strategy> strategy) {
	return std::shared_ptr<Strategy>(new Strategy(std::make_unique<lib::DG::Strategies::Repeat>(strategy->getStrategy().clone(), limit)));
}

std::shared_ptr<Strategy> Strategy::makeRevive(std::shared_ptr<Strategy> strategy) {
	return std::shared_ptr<Strategy>(new Strategy(std::make_unique<lib::DG::Strategies::Revive>(strategy->getStrategy().clone())));
}

std::shared_ptr<Strategy> Strategy::makeRightPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > predicate,
		std::shared_ptr<Strategy> strat) {
	return std::shared_ptr<Strategy>(new Strategy(std::make_unique<lib::DG::Strategies::RightPredicate>(predicate, strat->getStrategy().clone())));
}

std::shared_ptr<Strategy> Strategy::makeRule(std::shared_ptr<rule::Rule> rule) {
	return std::shared_ptr<Strategy>(new Strategy(std::make_unique<lib::DG::Strategies::Rule>(rule)));
}

std::shared_ptr<Strategy> Strategy::makeSequence(const std::vector<std::shared_ptr<Strategy> > &strategies) {
	std::vector< lib::DG::Strategies::Strategy*> cloned;
	for(std::shared_ptr<Strategy> strat : strategies) cloned.push_back(strat->getStrategy().clone());
	return std::shared_ptr<Strategy>(new Strategy(std::make_unique<lib::DG::Strategies::Sequence>(cloned)));
}

std::shared_ptr<Strategy> Strategy::makeSort(bool doUniverse,
		std::shared_ptr<mod::Function<bool(std::shared_ptr<graph::Graph>, std::shared_ptr<graph::Graph>, const Strategy::GraphState&)> > less) {
	return std::shared_ptr<Strategy>(new Strategy(std::make_unique<lib::DG::Strategies::Sort>(less, doUniverse)));
}

std::shared_ptr<Strategy> Strategy::makeTake(bool doUniverse, unsigned int limit) {
	return std::shared_ptr<Strategy>(new Strategy(std::make_unique<lib::DG::Strategies::Take>(limit, doUniverse)));
}

} // namespace dg
} // namespace mod