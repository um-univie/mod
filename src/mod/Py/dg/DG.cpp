#include <mod/Py/Common.h>

#include <mod/Derivation.h>
#include <mod/dg/DG.h>
#include <mod/dg/GraphInterface.h>
#include <mod/dg/Printer.h>
#include <mod/graph/Graph.h>
#include <mod/graph/Printer.h>

namespace mod {
namespace dg {
namespace Py {
namespace {

std::vector<std::shared_ptr<mod::graph::Graph> > getGraphDatabase(std::shared_ptr<DG> dg) {
	return std::vector<std::shared_ptr<mod::graph::Graph> >(begin(dg->getGraphDatabase()), end(dg->getGraphDatabase()));
}

DG::HyperEdge(DG::*findEdgeVertices)(const std::vector<DG::Vertex>&, const std::vector<DG::Vertex>&) const = &DG::findEdge;
DG::HyperEdge(DG::*findEdgeGraphs)(const std::vector<std::shared_ptr<mod::graph::Graph> >&, const std::vector<std::shared_ptr<mod::graph::Graph> >&) const = &DG::findEdge;

} // namespace 

void DG_doExport() {
	// rst: .. py:class:: DG
	// rst:
	// rst:		The main derivation graph class. A derivation graph is a directed hypergraph :math:`H = (V, E)`.
	// rst:		Each vertex is annotated with a graph, and each hyperedge is annotated with a transformation rule.
	// rst:		A derivation graph can either be calculated from a strategy or loaded from external data.
	// rst:
	py::class_<DG, std::shared_ptr<DG>, boost::noncopyable>("DG", py::no_init)
			// rst:		.. py:attribute:: id
			// rst:
			// rst:			The unique instance id among all :class:`DG` objects.
			// rst:
			// rst:			:type: int
			.add_property("id", &DG::getId)
			.def(str(py::self))
			//------------------------------------------------------------------
			// rst:		.. py:attribute:: numVertices
			// rst:
			// rst:			(Read-only) The number of vertices in the derivation graph.
			// rst:
			// rst:			:type: int
			.add_property("numVertices", &DG::numVertices)
			// rst:		.. py:attribute:: vertices
			// rst:
			// rst:			(Read-only) An iterable of all vertices in the derivation graph.
			// rst:
			// rst:			:type: :py:class:`DGVertexRange`
			.add_property("vertices", &DG::vertices)
			// rst:		.. py:attribute:: numEdges
			// rst:
			// rst:			(Read-only) The number of hyperedges in the derivation graph.
			// rst:
			// rst:			:type: int
			.add_property("numEdges", &DG::numEdges)
			// rst:		.. py:attribute:: edges
			// rst:
			// rst:			(Read-only) An iterable of all hyperedges in the derivation graph.
			// rst:
			// rst:			:type: :py:class:`DGEdgeRange`
			.add_property("edges", &DG::edges)
			//------------------------------------------------------------------
			// rst:		.. py:method:: findVertex(g)
			// rst:
			// rst:			:param g: the graph to find a vertex which has it associated.
			// rst:			:type g: :class:`Graph`
			// rst:			:returns: a vertex descriptor for which the given graph is associated,
			// rst:				or a null descriptor if no such vertex exists.
			// rst:			:rtype: :class:`DGVertex`
			.def("findVertex", &DG::findVertex)
			// rst:		.. py:method:: findEdge(sources, targets)
			// rst:		               findEdge(sourceGraphs, targetGraphs)
			// rst: 
			// rst:			:param sources: the list of source vertices the resulting hyperedge must have.
			// rst:			:type sources: list of :class:`DGVertex`
			// rst:			:param targets: the list of targets vertices the resulting hyperedge must have.
			// rst:			:type targets: list of :class:`DGVertex`
			// rst:			:param sourceGraphs: the list of graphs that must be associated with the source vertices the resulting hyperedge must have.
			// rst:			:type sourceGraphs: list of :class:`Graph`
			// rst:			:param targetGraphs: the list of graphs that must be associated with the targets vertices the resulting hyperedge must have.
			// rst:			:type targetGraphs: list of :class:`Graph`
			// rst:			:returns: a hyperedge with the given sources and targets.
			// rst:			  If no such hyperedge exists in the derivation graph then a null edge is returned.
			// rst:			  In the second version, the graphs are put through :meth:`findVertex` first.
			// rst:			:raises: :class:`LogicError` if a vertex descriptor is null, or does not belong to the derivation graph.
			.def("findEdge", findEdgeVertices)
			.def("findEdge", findEdgeGraphs)
			//------------------------------------------------------------------
			// rst:		.. py:method:: calc(printInfo=True)
			// rst:
			// rst:			Create the actual derivation graph.
			// rst:
			// rst:			:raises: :class:`LogicError` if created from :any:`dgRuleComp` and a dynamic add strategy adds a graph
			// rst:				isomorphic to an already known graph, but represented by a different object.
			.def("calc", &DG::calc, py::arg("printInfo") = true)
			// rst:		.. py:attribute:: graphDatabase
			// rst:
			// rst:			All graphs known to the derivation graph.
			// rst:
			// rst:			:type: list of :class:`Graph`
			.add_property("_graphDatabase", &getGraphDatabase)
			// rst:		.. py:attribute:: products
			// rst:
			// rst:			The subset of the vertex graphs which were discovered by the calculation.
			// rst:
			// rst:			:type: list of :class:`Graph`
			.add_property("_products", py::make_function(&DG::getProducts, py::return_value_policy<py::copy_const_reference>()))
			// rst:		.. py:method:: print(printer=DGPrinter(), data=None)
			// rst:
			// rst:			Print the derivation graph in style of a hypergraph. The appearance and structure
			// rst:			of the visualisation can optionally be configured by giving a DG printer and/or data object.
			// rst:
			// rst:			:param printer: the printer to use governing the appearance.
			// rst:			:type printer: :class:`DGPrinter`
			// rst:			:param data: the extra data to use encoding the structure of the graph.
			// rst:			:type data: :class:`DGPrintData`
			// rst:			:returns: the name of the PDF-file that will be compiled in post-processing and the name of the coordinate tex-file used.
			// rst:			:rtype: (string, string)
			.def("print", &DG::print)
			// rst:		.. py:method:: dump()
			// rst:
			// rst:			Export the derivation graph to an external file. The vertex graphs are exported as well.
			// rst:
			// rst:			:returns: the filename of the exported derivation graph.
			// rst:			:rtype: string
			// rst:			:raises: :py:class:`LogicError` if the DG has not been calculated.
			.def("dump", &DG::dump)
			// rst:		.. py:method:: list()
			// rst:
			// rst:			List information about the calculation.
			.def("list", &DG::list)
			// rst:		.. py:method:: listStats()
			// rst:
			// rst:			Lists various statistics for the derivation graph.
			// rst:
			// rst:			:raises: :py:class:`LogicError` if the DG has not been calculated.
			.def("listStats", &DG::listStats)
			.add_property("stratOutputSubset", &DG::getStratOutputSubset)
			;

	// rst: .. py:method:: dgDerivations(ders)
	// rst:
	// rst:		:param ders: the list of derivations to include in the derivation graph.
	// rst:		:type ders: list of :class:`Derivation`
	// rst:		:returns: a derivation graph from a list of :class:`Derivation`.
	// rst:		:rtype: :class:`DG`
	py::def("dgDerivations", (std::shared_ptr<DG> (*)(const std::vector<mod::Derivation> &)) & DG::derivations);
	// rst: .. py:method:: dgAbstract(s)
	// rst:
	// rst:		:param s: the abstract specification of a derivation graph.
	// rst:		:type s: string
	// rst:		:returns: Construct a derivation graph from an abstract set of derivations. See :cpp:func:`dg::DG::abstract` for the specification format.
	// rst:		:rtype: :class:`DG`
	py::def("dgAbstract", &DG::abstract);
	// rst: .. py:method:: dgRuleComp(graphs, strat, labelSettings=LabelSettings(LabelType.String, LabelRelation.Isomorphism), ignoreRuleLabelTypes=False)
	// rst:
	// rst:		Initialize a derivation graph defined by a strategy.
	// rst:
	// rst:		:param graphs: an initial graph database. Any derived graph isomorphic to one of these graphs will be replaced by the given graph.
	// rst:		:type graphs: list of :class:`Graph`
	// rst:		:param strat: the strategy which will be used for calculating the actual derivation graph.
	// rst:		:type strat: :class:`DGStrat`
	// rst:		:param labelSettings: the settings to use for monomorphism enumeration and isomorphism checked.
	// rst:		:type labelSettings: :class:`LabelSettings`
	// rst:		:param ignoreRuleLabelTypes: whether the rules in the given strategy should be checked to be in line with the given label type.
	// rst:		:type ignoreRuleLabelTypes: bool
	// rst:		:returns: the derivation graph object. The calculation method must be called to create the actual derivation graph.
	// rst:		:rtype: :class:`DG`
	// rst:		:raises: :class:`LogicError` if two graphs in ``graphs`` are different by isomorphic objects,
	// rst:			or similarly if a static add strategy adds an isomorphic graph.
	// rst:		:raises: :class:`LogicError` if :any:`ignoreRuleLabelTypes` is `False` and a rule in the given strategy
	// rst:			has an intended label type different from the given type in :any:`labelSettings`.
	py::def("dgRuleComp", &DG::ruleComp);
	// rst: .. py:method:: dgDump(graphs, rules, file)
	// rst:
	// rst:		Load a derivation graph dump.
	// rst:
	// rst:		:param graphs: Any graph in the dump which is isomorphic one of these graphs is replaced by the given graph.
	// rst:		:type graphs: list of :class:`Graph`
	// rst:		:param rules: As for the graphs the same procedure is done for the rules, however only using the name of the rule for comparison.
	// rst:		:type rules: list of :class:`Rule`
	// rst:		:returns: the loaded derivation graph.
	// rst:		:rtype: :class:`DG`
	py::def("dgDump", &DG::dumpImport);

	// rst: .. py:method:: diffDGs(dg1, dg2)
	// rst:
	// rst:		Compare two derivation graphs and lists the difference.
	// rst:		This is not a general isomorphism check; two vertices are equal if they have
	// rst:		the same graph attached. Edges are equal if the head and tail sets are equal
	// rst:		and if the attached rule is the same.
	// rst:
	// rst:		:param dg1: the first derivation graph.
	// rst:		:type dg1: :class:`DG`
	// rst:		:param dg2: the second derivation graph.
	// rst:		:type dg2: :class:`DG`
	py::def("diffDGs", &DG::diff);
}

} // namespace Py
} // namespace dg
} // namespace mod