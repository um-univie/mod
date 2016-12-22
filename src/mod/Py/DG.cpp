#include <mod/Py/Common.h>

#include <mod/Derivation.h>
#include <mod/DG.h>
#include <mod/DGGraphInterface.h>
#include <mod/DGPrinter.h>
#include <mod/Graph.h>
#include <mod/GraphPrinter.h>

namespace mod {
namespace Py {
namespace {

std::vector<std::shared_ptr<mod::Graph> > getGraphDatabase(std::shared_ptr<mod::DG> dg) {
	return std::vector<std::shared_ptr<mod::Graph> >(begin(dg->getGraphDatabase()), end(dg->getGraphDatabase()));
}

mod::DG::HyperEdge (mod::DG::*findEdgeVertices)(const std::vector<mod::DG::Vertex>&, const std::vector<mod::DG::Vertex>&) const = &mod::DG::findEdge;
mod::DG::HyperEdge (mod::DG::*findEdgeGraphs)(const std::vector<std::shared_ptr<Graph> >&, const std::vector<std::shared_ptr<Graph> >&) const = &mod::DG::findEdge;

} // namespace 

void DG_doExport() {
	// rst: .. py:class:: DG
	// rst:
	// rst:		The main derivation graph class. A derivation graph is a directed hypergraph :math:`H = (V, E)`.
	// rst:		Each vertex is annotated with a graph, and each hyperedge is annotated with a transformation rule.
	// rst:		A derivation graph can either be calculated from a strategy or loaded from external data.
	// rst:
	py::class_<mod::DG, std::shared_ptr<mod::DG>, boost::noncopyable>("DG", py::no_init)
			// rst:		.. py:attribute:: id
			// rst:
			// rst:			The unique instance id among all :class:`DG` objects.
			// rst:
			// rst:			:type: int
			.add_property("id", &mod::DG::getId)
			.def(str(py::self))
			//------------------------------------------------------------------
			// rst:		.. py:attribute:: numVertices
			// rst:
			// rst:			(Read-only) The number of vertices in the derivation graph.
			// rst:
			// rst:			:type: int
			.add_property("numVertices", &mod::DG::numVertices)
			// rst:		.. py:attribute:: vertices
			// rst:
			// rst:			(Read-only) An iterable of all vertices in the derivation graph.
			// rst:
			// rst:			:type: :py:class:`DGVertexRange`
			.add_property("vertices", &mod::DG::vertices)
			// rst:		.. py:attribute:: numEdges
			// rst:
			// rst:			(Read-only) The number of hyperedges in the derivation graph.
			// rst:
			// rst:			:type: int
			.add_property("numEdges", &mod::DG::numEdges)
			// rst:		.. py:attribute:: edges
			// rst:
			// rst:			(Read-only) An iterable of all hyperedges in the derivation graph.
			// rst:
			// rst:			:type: :py:class:`DGEdgeRange`
			.add_property("edges", &mod::DG::edges)
			//------------------------------------------------------------------
			// rst:		.. py:method:: findVertex(g)
			// rst:
			// rst:			:param g: the graph to find a vertex which has it associated.
			// rst:			:type g: :class:`Graph`
			// rst:			:returns: a vertex descriptor for which the given graph is associated,
			// rst:				or a null descriptor if no such vertex exists.
			// rst:			:rtype: :class:`DGVertex`
			.def("findVertex", &mod::DG::findVertex)
			// rst:		.. py:method:: findEdge(sources, targets)
			// rst:		               findEdge(sourceGraphs, targetGraphs)
			// rst: 
			// rst:			:param sources: the list of source vertices the resulting hyperedge must have.
			// rst:			:type sources: list of :class:`Vertex`
			// rst:			:param targets: the list of targets vertices the resulting hyperedge must have.
			// rst:			:type targets: list of :class:`Vertex`
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
			// rst:		.. py:method:: calc()
			// rst:
			// rst:			Create the actual derivation graph.
			.def("calc", &mod::DG::calc)
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
			.add_property("_products", py::make_function(&mod::DG::getProducts, py::return_value_policy<py::copy_const_reference>()))
			// rst:		.. py:method:: print(printer=DGPrinter(), data=None)
			// rst:
			// rst:			Print the derivation graph in style of a hypergraph. The appearance and structure
			// rst:			of the visualisation can optionally be configured by giving a DG printer and/or data object.
			// rst:
			// rst:			:param printer: the printer to use governing the appearance.
			// rst:			:type printer: :class:`DGPrinter`
			// rst:			:param data: the extra data to use encoding the structure of the graph.
			// rst:			:type data: :class:`DGPrintData`
			.def("print", &mod::DG::print)
			// rst:		.. py:method:: dump()
			// rst:
			// rst:			Export the derivation graph to an external file. The vertex graphs are exported as well.
			// rst:
			// rst:			:returns: the filename of the exported derivation graph.
			// rst:			:rtype: string
			.def("dump", &mod::DG::dump)
			// rst:		.. py:method:: list()
			// rst:
			// rst:			List information about the calculation.
			.def("list", &mod::DG::list)
			// rst:		.. py:method:: listStats()
			// rst:
			// rst:			Lists various statistics for the derivation graph.
			.def("listStats", &mod::DG::listStats)
			.add_property("stratOutputSubset", &mod::DG::getStratOutputSubset)
			;

	// rst: .. py:method:: dgDerivations(ders)
	// rst:
	// rst:		:param ders: the list of derivations to include in the derivation graph.
	// rst:		:type ders: list of :class:`Derivation`
	// rst:		:returns: a derivation graph from a list of :class:`Derivation`.
	// rst:		:rtype: :class:`DG`
	py::def("dgDerivations", (std::shared_ptr<mod::DG> (*)(const std::vector<mod::Derivation> &)) & mod::DG::derivations);
	// rst: .. py:method:: dgAbstract(s)
	// rst:
	// rst:		:param s: the abstract specification of a derivation graph.
	// rst:		:type s: string
	// rst:		:returns: Construct a derivation graph from an abstract set of derivations. See :cpp:func:`DG::abstract` for the specification format.
	// rst:		:rtype: :class:`DG`
	py::def("dgAbstract", &mod::DG::abstract);
	// rst: .. py:method:: dgRuleComp(graphs, strat)
	// rst:
	// rst:		Initialize a derivation graph defined by a strategy.
	// rst:
	// rst:		:param graphs: an initial graph database. Any derived graph isomorphic to one of these graphs will be replaced by the given graph.
	// rst:		:type graphs: list of :class:`Graph`
	// rst:		:param strat: the strategy which will be used for calculating the actual derivation graph.
	// rst:		:type strat: :class:`DGStrat`
	// rst:		:returns: the derivation graph object. The calculation method must be called to create the actual derivation graph.
	// rst:		:rtype: :class:`DG`
	py::def("dgRuleComp", &mod::DG::ruleComp);
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
	py::def("dgDump", &mod::DG::dumpImport);

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
	py::def("diffDGs", &mod::DG::diff);


	// rst: .. py:class:: DerivationRef
	// rst:
	// rst: 	A reference to a derivation in a specific derivation graph, or an invalid reference.
	// rst:		
	py::implicitly_convertible<mod::DerivationRef, mod::DG::HyperEdge>();
	py::implicitly_convertible<mod::DG::HyperEdge, mod::DerivationRef>();
	py::class_<mod::DerivationRef>("DerivationRef")
			.def(py::init<mod::DG::HyperEdge>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self < py::self)
			// rst: 	.. py:attribute:: id
			// rst:
			// rst: 		(Read-only) If the reference is valid, this is the id of the pointed to derivation.
			// rst:
			// rst: 		:type: unsigned int
			.add_property("id", &mod::DerivationRef::getId)
			// rst: 	.. py:attribute:: dg
			// rst:
			// rst: 		(Read-only) If the reference is valid, this is the derivation graph the reference points into.
			// rst:
			// rst: 		:type: :class:`DG`
			.add_property("dg", &mod::DerivationRef::getDG)
			// rst: 	.. py:method:: isValid()
			// rst:
			// rst: 		:returns: whether or not the reference is valid.
			// rst: 		:rtype: bool
			.def("isValid", &mod::DerivationRef::isValid)
			// rst: 	.. py:attribute:: derivation
			// rst:
			// rst: 		(Read-only) If the reference is valid, this is the pointed to derivation.
			// rst:
			// rst: 		:type: :class:`Derivation`
			.add_property("derivation", &mod::DerivationRef::operator*)
			// rst: 	.. py:method:: print(printer=GraphPrinter(), matchColour="ForestGreen")
			// rst:
			// rst: 		Print the derivation pointed to by the derivation reference.
			// rst: 		All possible Double-Pushout diagrams for the derivation are printed.
			// rst:
			// rst: 		:param printer: the printer to use for the figures.
			// rst: 		:type printer: :class:`GraphPrinter`
			// rst:			:param matchColour: the TikZ colour to use for the rule and its image in the bottom span.
			// rst:			:type matchColour: string
			.def("print", &mod::DerivationRef::print)
			;
}

} // namespace Py
} // namespace mod
