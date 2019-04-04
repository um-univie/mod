#ifndef MOD_DG_STRATEGIES_H
#define MOD_DG_STRATEGIES_H

#include <mod/dg/DG.h>
#include <mod/dg/GraphInterface.h>
#include <mod/rule/ForwardDecl.h>

#include <functional>
#include <iosfwd>
#include <memory>
#include <vector>

namespace mod {
struct Derivation;
template<typename Sig> class Function;
namespace dg {

// rst-class: dg::Strategy
// rst:
// rst:		Derivation graph strategies are used in :cpp:func:`DG::ruleComp`.
// rst:
// rst-class-start:

struct Strategy {
	Strategy(const Strategy&) = delete;
	Strategy &operator=(const Strategy&) = delete;
public:

	// rst-nested: dg::Strategy::GraphState
	// rst:
	// rst:		This class represents a graph state with a subset :math:`S` and a universe :math:`U` fulfilling :math:`S\subseteq U`.
	// rst:
	// rst-nested-start:

	struct GraphState {
		GraphState(std::function<void(std::vector<std::shared_ptr<graph::Graph> >&) > fSubset,
				std::function<void(std::vector<std::shared_ptr<graph::Graph> >&) > fUniverse);
		// rst:		.. function:: const std::vector<std::shared_ptr<graph::Graph> > &getSubset() const
		// rst:
		// rst:			:returns: the subset :math:`\mathcal{S}`.
		// rst:
		const std::vector<std::shared_ptr<graph::Graph> > &getSubset() const;
		// rst:		.. function:: const std::vector<std::shared_ptr<graph::Graph> > &getUniverse() const
		// rst:
		// rst:			:returns: the universe :math:`\mathcal{U}`
		// rst:
		const std::vector<std::shared_ptr<graph::Graph> > &getUniverse() const;
	private:
		mutable bool subsetInit, universeInit;
		mutable std::vector<std::shared_ptr<graph::Graph> > subset, universe;
		std::function<void(std::vector<std::shared_ptr<graph::Graph> >&) > fSubset, fUniverse;
	};
	// rst-nested-end:
private:
	Strategy(std::unique_ptr<lib::DG::Strategies::Strategy> strategy);
public:
	~Strategy();
	std::unique_ptr<Strategy> clone() const;
	friend std::ostream &operator<<(std::ostream &s, const Strategy &strat);
	lib::DG::Strategies::Strategy &getStrategy();
private:
	struct Pimpl;
	std::unique_ptr<Pimpl> p;
public:
	// rst: .. function:: static std::shared_ptr<Strategy> makeAdd(bool onlyUniverse, const std::vector<std::shared_ptr<graph::Graph> > &graphs)
	// rst:
	// rst:		:returns: an :ref:`strat-addUniverse` strategy if ``onlyUniverse`` is ``True``, otherwise an :ref:`strat-addSubset` strategy.
	static std::shared_ptr<Strategy> makeAdd(bool onlyUniverse, const std::vector<std::shared_ptr<graph::Graph> > &graphs);
	// rst: .. function:: static std::shared_ptr<Strategy> makeAdd(bool onlyUniverse, const std::shared_ptr<Function<std::vector<std::shared_ptr<graph::Graph> >() > > generator)
	// rst:
	// rst:		:returns: an :ref:`strat-addUniverse` strategy if ``onlyUniverse`` is ``True``, otherwise an :ref:`strat-addSubset` strategy.
	static std::shared_ptr<Strategy> makeAdd(bool onlyUniverse, const std::shared_ptr<Function<std::vector<std::shared_ptr<graph::Graph> >() > > generator);
	// rst: .. function:: static std::shared_ptr<Strategy> makeExecute(std::shared_ptr<Function<void(const Strategy::GraphState&)> > func)
	// rst:
	// rst:		:returns: an :ref:`strat-execute` strategy.
	static std::shared_ptr<Strategy> makeExecute(std::shared_ptr<Function<void(const Strategy::GraphState&)> > func);
	// rst: .. function:: static std::shared_ptr<Strategy> makeFilter(bool alsoUniverse,	std::shared_ptr<Function<bool(std::shared_ptr<graph::Graph>, const Strategy::GraphState&, bool)> > filterFunc)
	// rst:
	// rst:		The filtering predicate will be called for each graph in either the subset or the universe.
	// rst:		The predicate is called with the graph and the graph state as arguments, and a bool stating whether or not
	// rst:		the call is the first in the filtering process.
	// rst:
	// rst:		:returns: a :ref:`strat-filterUniverse` strategy if ``onlyUniverse`` is ``True``, otherwise a :ref:`strat-filterSubset` strategy.
	static std::shared_ptr<Strategy> makeFilter(bool alsoUniverse,
			std::shared_ptr<Function<bool(std::shared_ptr<graph::Graph>, const Strategy::GraphState&, bool)> > filterFunc);
	// rst: .. function:: static std::shared_ptr<Strategy> makeLeftPredicate(std::shared_ptr<Function<bool(const Derivation&) > > predicate, std::shared_ptr<Strategy> strat)
	// rst:
	// rst:		Even though the predicate is called with a :class:`Derivation` object, only the left side and the rule of the object is valid.
	// rst:
	// rst:		:returns: a :ref:`strat-leftPredicate` strategy.
	static std::shared_ptr<Strategy> makeLeftPredicate(std::shared_ptr<Function<bool(const Derivation&) > > predicate, std::shared_ptr<Strategy> strat);
	// rst: .. function:: static std::shared_ptr<Strategy> makeParallel(const std::vector<std::shared_ptr<Strategy> > &strategies)
	// rst:
	// rst:		:returns: a :ref:`strat-parallel` strategy.
	// rst:		:throws: :class:`LogicError` if `strategies` is empty.
	static std::shared_ptr<Strategy> makeParallel(const std::vector<std::shared_ptr<Strategy> > &strategies);
	// rst: .. function:: static std::shared_ptr<Strategy> makeRepeat(unsigned int limit, std::shared_ptr<Strategy> strategy)
	// rst:
	// rst:		:returns: a :ref:`strat-repeat` strategy.
	static std::shared_ptr<Strategy> makeRepeat(std::size_t limit, std::shared_ptr<Strategy> strategy);
	// rst: .. function:: static std::shared_ptr<Strategy> makeRevive(std::shared_ptr<Strategy> strategy)
	// rst:
	// rst:		:returns: a :ref:`strat-revive` strategy.
	static std::shared_ptr<Strategy> makeRevive(std::shared_ptr<Strategy> strategy);
	// rst: .. function:: static std::shared_ptr<Strategy> makeRightPredicate(std::shared_ptr<Function<bool(const Derivation&) > > predicate, std::shared_ptr<Strategy> strat)
	// rst:
	// rst:		:returns: a :ref:`strat-rightPredicate` strategy.
	static std::shared_ptr<Strategy> makeRightPredicate(std::shared_ptr<Function<bool(const Derivation&) > > predicate,
			std::shared_ptr<Strategy> strat);
	// rst: .. function:: static std::shared_ptr<Strategy> makeRule(std::shared_ptr<rule::Rule> r)
	// rst:
	// rst:		:returns: a :ref:`strat-rule` strategy.
	static std::shared_ptr<Strategy> makeRule(std::shared_ptr<rule::Rule> r);
	// rst: .. function:: static std::shared_ptr<Strategy> makeSequence(const std::vector<std::shared_ptr<Strategy> > &strategies)
	// rst:
	// rst:		:retunrs: a :ref:`strat-sequence` strategy.
	static std::shared_ptr<Strategy> makeSequence(const std::vector<std::shared_ptr<Strategy> > &strategies);
	// TODO: remove
	static std::shared_ptr<Strategy> makeSort(bool doUniverse,
			std::shared_ptr<Function<bool(std::shared_ptr<graph::Graph>, std::shared_ptr<graph::Graph>, const Strategy::GraphState&)> > less);
	// TODO: remove
	static std::shared_ptr<Strategy> makeTake(bool doUniverse, unsigned int limit);
};
// rst-class-end:

} // namespace dg
} // namespace mod

#endif /* MOD_DG_STRATEGIES_H */