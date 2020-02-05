#include <mod/py/Common.hpp>

#include <mod/Derivation.hpp>
#include <mod/dg/Builder.hpp>

namespace mod {
namespace dg {
namespace Py {
namespace {

// see https://stackoverflow.com/questions/19062657/is-there-a-way-to-wrap-the-function-return-value-object-in-python-using-move-i

std::shared_ptr<ExecuteResult>
Builder_execute(std::shared_ptr<Builder> b, std::shared_ptr<Strategy> strategy, int verbosity, bool ignoreRuleLabelTypes) {
	return std::make_shared<ExecuteResult>(b->execute(strategy, verbosity, ignoreRuleLabelTypes));
}

} // namespace

void Builder_doExport() {
	using AddDerivation = DG::HyperEdge (Builder::*)(const Derivations &, IsomorphismPolicy);
	// rst: .. py:class:: DGBuilder
	// rst:
	// rst:		An RAII-style object obtained from :py:meth:`DG.build`.
	// rst:		On destruction of an active builder object the owning :py:class:`DG` will be locked
	// rst:		for further modifications.
	// rst:
	// rst:		The object can be used as a context manager:
	// rst:
	// rst:		.. code-block:: python
	// rst:
	// rst:			dg = DG()
	// rst:			with dg.build() as b:
	// rst:				# b is a DGBuilder
	// rst:			# b has now been destructed and dg is locked.
	// rst:
	// rst:		Otherwise one can manually use ``del`` on the obtained builder to trigger the destruction.
	// rst:
	py::class_<Builder, std::shared_ptr<Builder>, boost::noncopyable>("DGBuilder", py::no_init)
			// rst:		.. py:method:: addDerivation(d, graphPolicy=IsomorphismPolicy.Check)
			// rst:
			// rst:			Adds a hyperedge corresponding to the given derivation to the associated :class:`DG`.
			// rst:			If it already exists, only add the given rules to the edge.
			// rst:
			// rst:			:param Derivations d: a derivation to add a hyperedge for.
			// rst:			:param IsomorphismPolicy graphPolicy: the isomorphism policy for adding the given graphs.
			// rst:			:returns: the hyperedge corresponding to the given derivation.
			// rst:			:rtype: DGHyperEdge
			// rst:			:raises: :class:`LogicError` if ``d.left`` is empty.
			// rst:			:raises: :class:`LogicError` if ``d.right`` is empty.
			// rst:			:raises: :class:`LogicError` if ``graphPolicy == IsomorphismPolicy.Check`` and a given graph object
			// rst:				is different but isomorphic to another given graph object or to a graph object already
			// rst:				in the internal graph database in the associated derivation graph.
			.def("addDerivation", static_cast<AddDerivation>(&Builder::addDerivation))
					// rst:		.. py:method:: execute(strategy, *, verbosity=2, ignoreRuleLabelTypes=False)
					// rst:
					// rst:			Execute the given strategy (:ref:`dgStrat`) and as a side-effect add
					// rst:			vertices and hyperedges to the underlying derivation graph.
					// rst:
					// rst:			:param DGStrat strategy: the strategy to execute.
					// rst:			:param int verbosity: the level of verbosity of printed information during calculation.
					// rst:				See :cpp:func:`dg::Builder::execute` for explanations of the levels.
					// rst:			:param bool ignoreRuleLabelTypes: whether rules in the strategy should be checked beforehand for
					// rst:				whether they have an associated :class:`LabelType` which matches the one in the underlying derivation graph.
					// rst:			:returns: a proxy object for accessing the result of the exeuction.
					// rst:			:rtype: DGExecuteResult
					// rst:			:throws: :class:`LogicError` if a static "add" strategy has :attr:`IsomorphismPolicy.Check` as graph policy,
					// rst:				and it tries to add a graph object isomorphic to an already known, but different, graph object in the database.
					// rst:				This is checked before execution, so there is strong exception guarantee.
					// rst:			:throws: :class:`LogicError` if a dynamic "add" strategy has :attr:`IsomorphismPolicy.Check` as graph policy,
					// rst:				and it tries to add a graph object isomorphic to an already known, but different, graph object in the database.
					// rst:
					// rst:				.. warning:: This is checked during execution, so while the basic exception guarantee is provided,
					// rst:					there may be modifications to the underlying derivation graph.
					// rst:			:throws: :class:`LogicError`: if ``ignoreRuleLabelTypes`` is ``False``, which is the default,
					// rst:				and a rule in the given strategy has an associated :class:`LabelType` which is different from the one
					// rst:				in the derivation graph.
			.def("execute", &Builder_execute)
					// rst:		.. py:method:: addAbstract(description)
					// rst:
					// rst:			Add vertices and hyperedges based on the given abstract description.
					// rst:			The description must adhere to the grammar described at :ref:`dg_abstract-desc`.
					// rst:
					// rst:			For each vertex named in the description a graph object with no vertices will be created,
					// rst:			and its name set to the given identifier.
					// rst:
					// rst:			:param str description: the description to parse into abstract derivations.
					// rst:			:raises: :class:`InputError` if the description could not be parsed.
			.def("addAbstract", &Builder::addAbstract);

	// rst: .. py:class:: DGExecuteResult
	// rst:
	// rst:		The result from calling :func:`DGBuilder.execute`.
	// rst:
	py::class_<ExecuteResult, std::shared_ptr<ExecuteResult>, boost::noncopyable>("DGExecuteResult", py::no_init)
			// rst:		.. py:attribute:: subset
			// rst:		                  universe
			// rst:
			// rst:			(Read-only) Respectively the subset and the universe computed
			// rst:			by the strategy execution (see also :ref:`dgStrat`).
			// rst:
			// rst:			:type: list[Graph]
			.add_property("subset", py::make_function(&ExecuteResult::getSubset,
			                                          py::return_value_policy<py::copy_const_reference>()))
			.add_property("universe", py::make_function(&ExecuteResult::getUniverse,
			                                            py::return_value_policy<py::copy_const_reference>()))
					// rst:		.. method:: list(*, withUniverse=False)
					// rst:
					// rst:			Output information from the execution of the strategy.
					// rst:
					// rst:			:param bool withUniverse: The universe lists can be rather long. As default they are omitted when listing.
			.def("list", &ExecuteResult::list);
}

} // namespace Py
} // namespace dg
} // namespace mod