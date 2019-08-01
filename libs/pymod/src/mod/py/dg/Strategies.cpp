#include <mod/py/Common.hpp>

#include <mod/dg/DG.hpp>
#include <mod/dg/Strategies.hpp>

// rst: This section describes two interfaces for the derivation graph strategies; the basic API and an embedded language which is built on the basic API.
// rst: Usually the embedded strategy language is easiest and sufficient for constructing strategies.
// rst:
// rst: The semantics of the individual strategies are described in :ref:`dgStrat`.
// rst: Note that a :py:class:`DGStrat` is a representation of a strategy and must be given to a derivation graph to be evaluated.
// rst:
// rst:
// rst: The Embedded Strategy Language
// rst: ###############################
// rst:
// rst: The strategy language is really a collection of proxy classes with a lot of operator overloading, thus
// rst: the normal syntax and semantics of Python applies.
// rst:
// rst: The following is the grammar for the strategies.
// rst:
// rst: .. productionlist::
// rst:    strat: `strats`
// rst:         : `strat` ">>" `strat`
// rst:         : `rule`
// rst:         : "addSubset(" `graphs` ")"
// rst:         : "addUniverse(" `graphs` ")"
// rst:         : "execute(" `executeFunc` ")"
// rst:         : "filterSubset(" `filterPred` ")"
// rst:         : "filterUniverse(" `filterPred` ")"
// rst:         : "leftPredicate[" `derivationPred` "](" `strat` ")"
// rst:         : "rightPredicate[" `derivationPred` "](" `strat` ")"
// rst:         : "repeat" [ "[" `int` "]" ] "(" `strat` ")"
// rst:         : "revive(" `strat` ")"
// rst:
// rst: A ``strats`` must be an iterable of :token:`strat`.
// rst: A ``graphs`` can either be a single :class:`Graph`, an iterable of graphs, or a function taking no arguments and returning a list of graphs.
// rst:

namespace mod {
namespace dg {
namespace Py {

void Strategy_doExport() {
	// rst: The Basic API
	// rst: #################
	// rst:
	// rst: .. py:class:: DGStratGraphState
	// rst:
	// rst:		This class represents a graph state with a subset :math:`S` and a universe :math:`U` fulfilling :math:`S\subseteq U`.
	// rst:
	py::class_<Strategy::GraphState>("DGStratGraphState", py::no_init)
			// rst:		.. py:attribute:: subset
			// rst:
			// rst:			The subset :math:`\mathcal{S}`.
			// rst:
			// rst:			:type: list of :py:class:`Graph`
			// rst:
			.add_property("_subset", py::make_function(&Strategy::GraphState::getSubset, py::return_internal_reference<1>()))
			// rst:		.. py:attribute:: universe
			// rst:
			// rst:			The universe :math:`\mathcal{U}`.
			// rst:
			// rst:			:type: list of :py:class:`Graph`
			// rst:
			.add_property("_universe", py::make_function(&Strategy::GraphState::getUniverse, py::return_internal_reference<1>()))
			;


	std::shared_ptr<Strategy> (*makeAdd_static)(bool, const std::vector<std::shared_ptr<graph::Graph> >&) = &Strategy::makeAdd;
	std::shared_ptr<Strategy> (*makeAdd_dynamic)(bool, const std::shared_ptr < mod::Function < std::vector<std::shared_ptr<graph::Graph> >() > >) = &Strategy::makeAdd;

	// rst: .. py:class:: DGStrat
	// rst: 
	py::class_<Strategy, std::shared_ptr<Strategy>, boost::noncopyable>("DGStrat", py::no_init)
			.def(str(py::self))

			// factory functions
			// rst:		.. py:staticmethod:: makeAddStatic(onlyUniverse, graphs)
			// rst:
			// rst:			:param bool onlyUniverse: if the strategy is :ref:`strat-addUniverse` or :ref:`strat-addSubset`.
			// rst:			:param graphs: the graphs to be added by the strategy.
			// rst:			:type graphs: list of :class:`Graph`
			// rst:			:returns: an :ref:`strat-addUniverse` strategy if ``onlyUniverse`` is ``True``, otherwise an :ref:`strat-addSubset` strategy.
			// rst:			:rtype: :class:`DGStrat`
			.def("makeAddStatic", makeAdd_static).staticmethod("makeAddStatic")
			// rst:		.. py:staticmethod:: makeAddDynamic(onlyUniverse, graphsFunc)
			// rst:
			// rst:			:param bool onlyUniverse: if the strategy is :ref:`strat-addUniverse` or :ref:`strat-addSubset`.
			// rst:			:param graphsFunc: a function returning the graphs to be added by the strategy.
			// rst:			:type graphsFunc: list of :class:`Graph` ()
			// rst:			:returns: an :ref:`strat-addUniverse` strategy if ``onlyUniverse`` is ``True``, otherwise an :ref:`strat-addSubset` strategy.
			// rst:			:rtype: :class:`DGStrat`
			.def("makeAddDynamic", makeAdd_dynamic).staticmethod("makeAddDynamic")
			// rst:		.. py:staticmethod:: makeExecute(func)
			// rst:
			// rst:			:param func: A function being executed when the strategy is evaluated.
			// rst:			:type func: void(:class:`DGStratGraphState`)
			// rst:			:returns: an :ref:`strat-execute` strategy.
			// rst:			:rtype: :class:`DGStrat`
			.def("makeExecute", &Strategy::makeExecute).staticmethod("makeExecute")
			// rst:		.. py:staticmethod:: makeFilter(alsoUniverse, p)
			// rst:
			// rst:			:param alsoUniverse: if the strategy is :ref:`strat-filterUniverse` or :ref:`strat-filterSubset`.
			// rst:			:type alsoUniverse: bool
			// rst:			:param p: the filtering predicate being called for each graph in either the subset or the universe.
			// rst:				The predicate is called with the graph and the graph state as arguments, and a bool stating whether or not
			// rst:				the call is the first in the filtering process.
			// rst:			:type p: bool(:class:`Graph`, :class:`DGStratGraphState`, bool)
			// rst:			:returns: a :ref:`strat-filterUniverse` strategy if ``onlyUniverse`` is ``True``, otherwise a :ref:`strat-filterSubset` strategy.
			// rst:			:rtype: :class:`DGStrat`
			.def("makeFilter", &Strategy::makeFilter).staticmethod("makeFilter")
			// rst:		.. py:staticmethod:: makeLeftPredicate(p, strat)
			// rst:
			// rst:			:param p: the predicate to be called on each candidate derivation.
			// rst:				Even though the predicate is called with a :class:`Derivation` object, only the left side and the rule of the object is valid.
			// rst:			:type p: bool(:class:`Derivation`)
			// rst:			:param strat: the sub-strategy to be evaluated under the constraints of the left predicate.
			// rst:			:type strat: :class:`DGStrat`
			// rst:			:returns: a :ref:`strat-leftPredicate` strategy.
			// rst:			:rtype: :class:`DGStrat`
			.def("makeLeftPredicate", &Strategy::makeLeftPredicate).staticmethod("makeLeftPredicate")
			// rst:		.. py:staticmethod:: makeParallel(strats)
			// rst:
			// rst:			:param strats: the sub-strategies to evaluate.
			// rst:			:type strats: list of :class:`DGStrat`
			// rst:			:returns: a :ref:`strat-parallel` strategy.
			// rst:			:rtype: :class:`DGStrat`
			// rst:			:raises: :class:`LogicError` if `strats` is empty.
			.def("makeParallel", &Strategy::makeParallel).staticmethod("makeParallel")
			// rst:		.. py:staticmethod:: makeRepeat(limit, strat)
			// rst:
			// rst:			:param limit: the maximum number of iterations.
			// rst:			:type limit: unsigned int
			// rst:			:param strat: the strategy to be repeated.
			// rst:			:type strat: :class:`DGStrat`
			// rst:			:returns: a :ref:`strat-repeat` strategy.
			// rst:			:rtype: :class:`DGStrat`
			.def("makeRepeat", &Strategy::makeRepeat).staticmethod("makeRepeat")
			// rst:		.. py:staticmethod:: makeRevive(strat)
			// rst:
			// rst:			:param strat: the strategy to encapsulate.
			// rst:			:type strat: :class:`DGStrat`
			// rst:			:returns: a :ref:`strat-revive` strategy.
			// rst:			:rtype: :class:`DGStrat`
			.def("makeRevive", &Strategy::makeRevive).staticmethod("makeRevive")
			// rst:		.. py:staticmethod:: makeRightPredicate(p, strat)
			// rst:
			// rst:			:param p: the predicate to be called on each candidate derivation.
			// rst:			:type p: bool(:class:`Derivation`)
			// rst:			:param strat: the sub-strategy to be evaluated under the constraints of the right predicate.
			// rst:			:type strat: :class:`DGStrat`
			// rst:			:returns: a :ref:`strat-rightPredicate` strategy.
			// rst:			:rtype: :class:`DGStrat`
			.def("makeRightPredicate", &Strategy::makeRightPredicate).staticmethod("makeRightPredicate")
			// rst:		.. py:staticmethod:: makeRule(r)
			// rst:
			// rst:			:param r: the rule to make into a strategy.
			// rst:			:type r: :class:`Rule`
			// rst:			:returns: a :ref:`strat-rule` strategy.
			// rst:			:rtype: :class:`DGStrat`
			.def("makeRule", &Strategy::makeRule).staticmethod("makeRule")
			// rst:		.. py:staticmethod:: makeSequence(strats)
			// rst:
			// rst:			:param strats: the strategies to evaluate in sequence.
			// rst:			:type strats: list of :class:`DGStrat`
			// rst:			:retunrs: a :ref:`strat-sequence` strategy.
			// rst:			:rtype: :class:`DGStrat`
			.def("makeSequence", &Strategy::makeSequence).staticmethod("makeSequence")
			.def("makeSort", &Strategy::makeSort).staticmethod("makeSort") // TODO: remove
			.def("makeTake", &Strategy::makeTake).staticmethod("makeTake") // TODO: remove
			;
}

} // namespace Py
} // namespace dg
} // namespace mod