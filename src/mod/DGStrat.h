#ifndef MOD_DGSTRAT_H
#define MOD_DGSTRAT_H

#include <mod/DG.h>

#include <iosfwd>
#include <memory>
#include <vector>

namespace mod {
class Derivation;
class DerivationRef;
class DGStrat;
template<typename Sig> class Function;
class Graph;
class Rule;
namespace lib {
namespace DG {
namespace Strategies {
class Strategy;
} // namespace Strategies
} // namespace DG
} // namespace lib

// rst-class: DGStrat
// rst:
// rst:		Derivation graph strategies are used in :cpp:func:`DG::ruleComp`.
// rst:
// rst-class-start:

struct DGStrat {
	DGStrat(const DGStrat&) = delete;
	DGStrat &operator=(const DGStrat&) = delete;
public:

	// rst-nested: DGStrat::GraphState
	// rst:
	// rst:		This class represents a graph state with a subset :math:`S` and a universe :math:`U` fulfilling :math:`S\subseteq U`.
	// rst:
	// rst-nested-begin:

	struct GraphState {
		GraphState(std::function<void(std::vector<std::shared_ptr<Graph> >&) > fSubset,
				std::function<void(std::vector<std::shared_ptr<Graph> >&) > fUniverse,
				std::function<void(std::vector<DerivationRef>&) > fDerivationRefs);
		// rst:		.. function:: const std::vector<std::shared_ptr<Graph> > &getSubset() const
		// rst:
		// rst:			:returns: the subset :math:`\mathcal{S}`.
		// rst:
		const std::vector<std::shared_ptr<Graph> > &getSubset() const;
		// rst:		.. function:: const std::vector<std::shared_ptr<Graph> > &getUniverse() const
		// rst:
		// rst:			:returns: the universe :math:`\mathcal{U}`
		// rst:
		const std::vector<std::shared_ptr<Graph> > &getUniverse() const;
		// rst:		.. function:: const std::vector<DerivationRef> &getDerivationRefs() const
		// rst:
		// rst:			:returns: the references to all derivations in the underlying derivation graph.
		// rst:				I.e., some derivations might involve graphs not in the universe.
		// rst:
		const std::vector<DerivationRef> &getDerivationRefs() const;
	private:
		mutable bool subsetInit, universeInit, derivationRefsInit;
		mutable std::vector<std::shared_ptr<Graph> > subset, universe;
		mutable std::vector<DerivationRef> derivationRefs;
		std::function<void(std::vector<std::shared_ptr<Graph> >&) > fSubset, fUniverse;
		std::function<void(std::vector<DerivationRef>&) > fDerivationRefs;
	};
	// rst-nested-end:
private:
	DGStrat(std::unique_ptr<lib::DG::Strategies::Strategy> strategy);
public:
	~DGStrat();
	std::unique_ptr<DGStrat> clone() const;
	friend std::ostream &operator<<(std::ostream &s, const DGStrat &strat);
	lib::DG::Strategies::Strategy &getStrategy();
private:
	struct Pimpl;
	std::unique_ptr<Pimpl> p;
public:
	// rst: .. function:: static std::shared_ptr<DGStrat> makeAdd(bool onlyUniverse, const std::vector<std::shared_ptr<Graph> > &graphs)
	// rst:
	// rst:		:returns: an :ref:`strat-addUniverse` strategy if ``onlyUniverse`` is ``True``, otherwise an :ref:`strat-addSubset` strategy.
	static std::shared_ptr<DGStrat> makeAdd(bool onlyUniverse, const std::vector<std::shared_ptr<Graph> > &graphs);
	// rst: .. function:: static std::shared_ptr<DGStrat> makeAdd(bool onlyUniverse, const std::shared_ptr<Function<std::vector<std::shared_ptr<Graph> >() > > generator)
	// rst:
	// rst:		:returns: an :ref:`strat-addUniverse` strategy if ``onlyUniverse`` is ``True``, otherwise an :ref:`strat-addSubset` strategy.
	static std::shared_ptr<DGStrat> makeAdd(bool onlyUniverse, const std::shared_ptr<Function<std::vector<std::shared_ptr<Graph> >() > > generator);
	// rst: .. function:: static std::shared_ptr<DGStrat> makeExecute(std::shared_ptr<Function<void(const DGStrat::GraphState&)> > func)
	// rst:
	// rst:		:returns: an :ref:`strat-execute` strategy.
	static std::shared_ptr<DGStrat> makeExecute(std::shared_ptr<Function<void(const DGStrat::GraphState&)> > func);
	// rst: .. function:: static std::shared_ptr<DGStrat> makeFilter(bool alsoUniverse,	std::shared_ptr<Function<bool(std::shared_ptr<Graph>, const DGStrat::GraphState&, bool)> > filterFunc)
	// rst:
	// rst:		The filtering predicate will be called for each graph in either the subset or the universe.
	// rst:		The predicate is called with the graph and the graph state as arguments, and a bool stating whether or not
	// rst:		the call is the first in the filtering process.
	// rst:
	// rst:		:returns: a :ref:`strat-filterUniverse` strategy if ``onlyUniverse`` is ``True``, otherwise a :ref:`strat-filterSubset` strategy.
	static std::shared_ptr<DGStrat> makeFilter(bool alsoUniverse,
			std::shared_ptr<Function<bool(std::shared_ptr<Graph>, const DGStrat::GraphState&, bool)> > filterFunc);
	// rst: .. function:: static std::shared_ptr<DGStrat> makeLeftPredicate(std::shared_ptr<Function<bool(const Derivation&) > > predicate, std::shared_ptr<DGStrat> strat)
	// rst:
	// rst:		Even though the predicate is called with a :class:`Derivation` object, only the left side and the rule of the object is valid.
	// rst:
	// rst:		:returns: a :ref:`strat-leftPredicate` strategy.
	static std::shared_ptr<DGStrat> makeLeftPredicate(std::shared_ptr<Function<bool(const Derivation&) > > predicate,
			std::shared_ptr<DGStrat> strat);
	// rst: .. function:: static std::shared_ptr<DGStrat> makeParallel(const std::vector<std::shared_ptr<DGStrat> > &strategies)
	// rst:
	// rst:		:returns: a :ref:`strat-parallel` strategy.
	// rst:		:throws: :class:`LogicError` if `strategies` is empty.
	static std::shared_ptr<DGStrat> makeParallel(const std::vector<std::shared_ptr<DGStrat> > &strategies);
	// rst: .. function:: static std::shared_ptr<DGStrat> makeRepeat(unsigned int limit, std::shared_ptr<DGStrat> strategy)
	// rst:
	// rst:		:returns: a :ref:`strat-repeat` strategy.
	static std::shared_ptr<DGStrat> makeRepeat(std::size_t limit, std::shared_ptr<DGStrat> strategy);
	// rst: .. function:: static std::shared_ptr<DGStrat> makeRevive(std::shared_ptr<DGStrat> strategy)
	// rst:
	// rst:		:returns: a :ref:`strat-revive` strategy.
	static std::shared_ptr<DGStrat> makeRevive(std::shared_ptr<DGStrat> strategy);
	// rst: .. function:: static std::shared_ptr<DGStrat> makeRightPredicate(std::shared_ptr<Function<bool(const Derivation&) > > predicate, std::shared_ptr<DGStrat> strat)
	// rst:
	// rst:		:returns: a :ref:`strat-rightPredicate` strategy.
	static std::shared_ptr<DGStrat> makeRightPredicate(std::shared_ptr<Function<bool(const Derivation&) > > predicate,
			std::shared_ptr<DGStrat> strat);
	// rst: .. function:: static std::shared_ptr<DGStrat> makeRule(std::shared_ptr<Rule> rule)
	// rst:
	// rst:		:returns: a :ref:`strat-rule` strategy.
	static std::shared_ptr<DGStrat> makeRule(std::shared_ptr<Rule> rule);
	// rst: .. function:: static std::shared_ptr<DGStrat> makeSequence(const std::vector<std::shared_ptr<DGStrat> > &strategies)
	// rst:
	// rst:		:retunrs: a :ref:`strat-sequence` strategy.
	static std::shared_ptr<DGStrat> makeSequence(const std::vector<std::shared_ptr<DGStrat> > &strategies);
	// TODO: remove
	static std::shared_ptr<DGStrat> makeSort(bool doUniverse,
			std::shared_ptr<Function<bool(std::shared_ptr<Graph>, std::shared_ptr<Graph>, const DGStrat::GraphState&)> > less);
	// TODO: remove
	static std::shared_ptr<DGStrat> makeTake(bool doUniverse, unsigned int limit);
};
// rst-class-end:

} // namespace mod

#endif /* MOD_DGSTRAT_H */