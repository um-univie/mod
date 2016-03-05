#include <mod/Py/Common.h>

#include <mod/Derivation.h>
#include <mod/DG.h>
#include <mod/Function.h>
#include <mod/Graph.h>

namespace mod {
namespace Py {
namespace {

std::vector<std::shared_ptr<mod::Graph> > getGraphDatabase(std::shared_ptr<mod::DG> dg) {
	return std::vector<std::shared_ptr<mod::Graph> >(begin(dg->getGraphDatabase()), end(dg->getGraphDatabase()));
}

std::size_t DG_DerivationRefRange_len(const mod::DG::DerivationRefRange &range) {
	return std::distance(range.begin(), range.end());
}

void DGPrinter_pushVertexVisible(mod::DGPrinter &printer, std::shared_ptr<mod::Function<bool(std::shared_ptr<mod::Graph>, std::shared_ptr<mod::DG>)> > f) {
	printer.pushVertexVisible(mod::toStdFunction(f));
}

void DGPrinter_pushEdgeVisible(mod::DGPrinter &printer, std::shared_ptr<mod::Function<bool(mod::DerivationRef)> > f) {
	printer.pushEdgeVisible(mod::toStdFunction(f));
}

void DGPrinter_pushVertexLabel(mod::DGPrinter &printer, std::shared_ptr<mod::Function<std::string(std::shared_ptr<mod::Graph>, std::shared_ptr<mod::DG>)> > f) {
	printer.pushVertexLabel(mod::toStdFunction(f));
}

void DGPrinter_pushEdgeLabel(mod::DGPrinter &printer, std::shared_ptr<mod::Function<std::string(mod::DerivationRef)> > f) {
	printer.pushEdgeLabel(mod::toStdFunction(f));
}

void DGPrinter_pushVertexColour(mod::DGPrinter &printer, std::shared_ptr<mod::Function<std::string(std::shared_ptr<mod::Graph>, std::shared_ptr<mod::DG>)> > f, bool extendToEdges) {
	printer.pushVertexColour(mod::toStdFunction(f), extendToEdges);
}

void DGPrinter_pushEdgeColour(mod::DGPrinter &printer, std::shared_ptr<mod::Function<std::string(mod::DerivationRef)> > f) {
	printer.pushEdgeColour(mod::toStdFunction(f));
}

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
			// rst:			:type: unsigned int
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
			// rst:		.. py:attribute:: vertexGraphs
			// rst:
			// rst:			A subset of the graph database corresponding to the vertices of the actual derivation graph.
			// rst:
			// rst:			:type: list of :class:`Graph`
			.add_property("_vertexGraphs", py::make_function(&mod::DG::getVertexGraphs, py::return_value_policy<py::copy_const_reference>()))
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
			.def("printMatrix", &mod::DG::printMatrix)
			// rst:		.. py:method:: getDerivationRef(educts, products)
			// rst:
			// rst:			Query the derivation graph for a specific derivation.
			// rst:
			// rst:			:param educts: the educts of the derivation to find.
			// rst:			:type educts: list of :class:`Graph`
			// rst:			:param products: the products of the derivation to find.
			// rst:			:type products: list of :class:`Graph`
			// rst:			:returns: a reference to a derivation. If no matching derivation is found, the returned reference will not be valid.
			// rst:			:rtype: :class:`DerivationRef`
			.def("getDerivationRef", &mod::DG::getDerivationRef)
			// rst:		.. py:attribute:: derivations
			// rst:
			// rst:			(Read-only) An iterable of references to all derivations in the DG.
			// rst:
			// rst:			:type: :class:`DerivationRefRange`
			.add_property("derivations", (mod::DG::DerivationRefRange(mod::DG::*)() const) &mod::DG::derivations)
			// rst:		.. py:method:: inDerivations(g)
			// rst:
			// rst:			:param g: the graph corresponding to the vertex to return in-derivations for.
			// rst:			:type g: :class:`Graph`
			// rst:			:returns: an iterable of references to all derivations going in to the vertex corresponding to the graph ``g``.
			// rst:				A derivation reference is in the range as many times as the vertex is a member of the head-multiset of that derivation.
			// rst:			:rtype: :class:`DerivationRefRange`
			.def("inDerivations", &mod::DG::inDerivations)
			// rst:		.. py:method:: outDerivations(g)
			// rst:
			// rst:			:param g: the graph corresponding to the vertex to return out-derivations for.
			// rst:			:type g: :class:`Graph`
			// rst:			:returns: an iterable of references to all derivations going out of the vertex corresponding to the graph ``g``.
			// rst:				A derivation reference is in the range as many times as the vertex is a member of the tail-multiset of that derivation.
			// rst:			:rtype: :class:`DerivationRefRange`
			.def("outDerivations", &mod::DG::outDerivations)
			// rst:		.. py:method:: list()
			// rst:
			// rst:			List information about the calculation.
			.def("list", &mod::DG::list)
			// rst:		.. py:method:: listStats()
			// rst:
			// rst:			Lists various statistics for the derivation graph.
			.def("listStats", &mod::DG::listStats)
			// rst:		.. py:method:: listIncidence(g)
			// rst:
			// rst:			List the incident derivations of the vertex represented by the given graph.
			// rst:
			// rst:			:param g: the graph for which the corresponding derivation graph vertex should have its incident derivations listed.
			// rst:			:type g: :class:`Graph`
			.def("listIncidence", &mod::DG::listIncidence)
			.add_property("stratOutputSubset", &mod::DG::getStratOutputSubset)
			;

	py::class_<mod::DG::DerivationRefRange>("DerivationRefRange", py::no_init)
			.def("__iter__", py::iterator<mod::DG::DerivationRefRange>())
			.def("__len__", &DG_DerivationRefRange_len)
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
	// rst: .. py:method:: dgSBML(s)
	// rst:
	// rst:		Load a derivation graph from an SBML file.
	// rst:		
	// rst:		:param s: the name of the SBML file.
	// rst:		:type s: string
	// rst:		:returns: the loaded derivation graph.
	// rst:		:rtype: :class:`DG`
	py::def("dgSBML", &mod::DG::sbml);
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




	// rst: .. py:class:: DGVertex
	// rst:
	// rst:		A descriptor of either a vertex in a derivation graph, or a null vertex.
	// rst:
	py::class_<mod::DG::Vertex>("DGVertex", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst: 
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			// rst:		.. py:attribute:: id
			// rst:
			// rst:			(Read-only) The index of the vertex. It will be in the range :math:`[0, numVertices + numEdges[`.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("id", &mod::DG::Vertex::getId)
			// rst:		.. py:attribute:: dg
			// rst:
			// rst:			(Read-only) The derivation graph the vertex belongs to.
			// rst:
			// rst:			:type: :py:class:`DG`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("dg", &mod::DG::Vertex::getDG)
			// rst:		.. py:attribute: inDegree
			// rst:
			// rst:			(Read-only) The in-degree of the vertex, including multiplicity of target multisets.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("inDegree", &mod::DG::Vertex::inDegree)
			// rst:		.. py:attribute:: inEdges
			// rst:
			// rst:			(Read-only) A range of in-hyperedges for this vertex.
			// rst:
			// rst:			:type: :py:class:`InEdgeRange`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("inEdges", &mod::DG::Vertex::inEdges)
			// rst:		.. py:attribute: outDegree
			// rst:
			// rst:			(Read-only) The out-degree of the vertex, including multiplicity of source multisets.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("outDegree", &mod::DG::Vertex::outDegree)
			// rst:		.. py:attribute:: outEdges
			// rst:
			// rst:			(Read-only) A range of out-hyperedges for this vertex.
			// rst:
			// rst:			:type: :py:class:`OutEdgeRange`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("outEdges", &mod::DG::Vertex::outEdges)
			// rst:		.. py:attribute:: graph
			// rst:
			// rst:			(Read-only) The graph label of the vertex.
			// rst:
			// rst:			:type: :py:class:`Graph`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("graph", &mod::DG::Vertex::getGraph)
			;

	// rst: .. py:class:: DGHyperEdge
	// rst:
	// rst:		A descriptor of either a hyperedge in a derivation graph, or a null edge.
	// rst:
	py::class_<mod::DG::HyperEdge>("DGHyperEdge", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst:
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			// rst:		.. py:attribute:: id
			// rst:
			// rst:			(Read-only) The index of the hyperedge. It will be in the range :math:`[0, numVertices + numEdges[`.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("id", &mod::DG::HyperEdge::getId)
			// rst:		.. py:attribute:: dg
			// rst:
			// rst:			(Read-only) The derivation graph the edge belongs to.
			// rst:
			// rst:			:type: :py:class:`DG`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("dg", &mod::DG::HyperEdge::getDG)
			// rst:		.. py:attribute:: numSources
			// rst:
			// rst:			(Read-only) The number of sources of the hyperedge.
			// rst:
			// rst:			:type: int
			.add_property("numSources", &mod::DG::HyperEdge::numSources)
			// rst:		.. py:attribute:: sources
			// rst:
			// rst:			(Read-only) The sources of the hyperedge.
			// rst:
			// rst:			:type: :py:class:`DGSourceRange`
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("sources", &mod::DG::HyperEdge::sources)
			// rst:		.. py:attribute:: numTargets
			// rst:
			// rst:			(Read-only) The number of targerts of the hyperedge.
			// rst:
			// rst:			:type: int
			.add_property("numTargets", &mod::DG::HyperEdge::numTargets)
			// rst:		.. attribute:: targets
			// rst:
			// rst:			(Read-only) The targets of the hyperedge.
			// rst:
			// rst:			:type: :py:class:`DGTargetRange`
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("targets", &mod::DG::HyperEdge::targets)
			// rst:		.. py:attribute:: rules
			// rst:
			// rst:			(Read-only) The rules associated with the hyperedge.
			// rst:
			// rst:			:type: :py:class:`DGRuleRange`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("rules", &mod::DG::HyperEdge::rules)
			;





	py::class_<mod::DG::VertexRange>("DGVertexRange", py::no_init)
			.def("__iter__", py::iterator<mod::DG::VertexRange>())
			;
	py::class_<mod::DG::EdgeRange>("DGEdgeRange", py::no_init)
			.def("__iter__", py::iterator<mod::DG::EdgeRange>())
			;
	py::class_<mod::DG::InEdgeRange>("DGInEdgeRange", py::no_init)
			.def("__iter__", py::iterator<mod::DG::InEdgeRange>())
			;
	py::class_<mod::DG::OutEdgeRange>("DGOutEdgeRange", py::no_init)
			.def("__iter__", py::iterator<mod::DG::OutEdgeRange>())
			;
	py::class_<mod::DG::SourceRange>("DGSourceRange", py::no_init)
			.def("__iter__", py::iterator<mod::DG::SourceRange>())
			;
	py::class_<mod::DG::TargetRange>("DGTargetRange", py::no_init)
			.def("__iter__", py::iterator<mod::DG::TargetRange>())
			;
	py::class_<mod::DG::RuleRange>("DGRuleRange", py::no_init)
			.def("__iter__", py::iterator<mod::DG::RuleRange>())
			;










	// rst: .. py:class:: DerivationRef
	// rst:
	// rst: 	A reference to a derivation in a specific derivation graph, or an invalid reference.
	// rst:		
	py::class_<mod::DerivationRef>("DerivationRef")
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

	// rst: .. py:class:: DGPrintData
	// rst:
	// rst:		This class is used to hold extra data about how a specific derivation graph
	// rst:		is visualised.
	// rst:
	py::class_<mod::DGPrintData>("DGPrintData", py::no_init)
			// rst:		.. py:method:: __init__(self, dg)
			// rst:
			// rst:			:param dg: the derivation graph to hold data for.
			// rst:			:type dg: :class:`DG`
			.def(py::init<std::shared_ptr<mod::DG> >())
			// rst:		.. py:attribute:: dg
			// rst:
			// rst:			(Read-only) The derivation graph the object holds data for.
			.add_property("dg", &mod::DGPrintData::getDG)
			// rst:		.. py:method:: makeDuplicate(dRef, eDup)
			// rst:
			// rst:			Create another version of the given derivation and give it the given duplicate number.
			// rst:
			// rst:			:param dRef: a reference to the derivation to duplicate.
			// rst:			:type dRef: :class:`DerivationRef`
			// rst:			:param eDup: the duplicate number for the new version of the derivation.
			// rst:			:type eDup: unsigned int
			.def("makeDuplicate", &mod::DGPrintData::makeDuplicate)
			// rst:		.. py:method:: removeDuplicate(dRef, eDup)
			// rst:
			// rst:			Remove the version of the given derivation with the given duplicate number.
			// rst:
			// rst:			:param dRef: a reference to the derivation to duplicate.
			// rst:			:type dRef: :class:`DerivationRef`
			// rst:			:param eDup: the duplicate number for the version of the derivation to remove.
			// rst:			:type eDup: unsigned int
			.def("removeDuplicate", &mod::DGPrintData::removeDuplicate)
			// rst:		.. py:method:: reconnectTail(dRef, eDup, g, vDupTar)
			// rst:
			// rst:			Reconnect an arbitrary version of the tail specified by the given graph in the derivation duplicate given.
			// rst:
			// rst:			:param dRef: a reference to the derivation to reconnect.
			// rst:			:type dRef: :class:`DerivationRef`
			// rst:			:param eDup: the duplicate number of the derivation to reconnect.
			// rst:			:type eDup: unsigned int
			// rst:			:param g: a graph representing the tail vertex to reconnect.
			// rst:			:type g: :class:`Graph`
			// rst:			:param vDupTar: the new duplicate number for the tail vertex.
			// rst:			:type vDupTar: unsigned int
			.def("reconnectTail", &mod::DGPrintData::reconnectTail)
			// rst:		.. py:method:: reconnectHead(dRef, eDup, g, vDupTar)
			// rst:
			// rst:			Reconnect an arbitrary version of the head specified by the given graph in the derivation duplicate given.
			// rst:
			// rst:			:param dRef: a reference to the derivation to reconnect.
			// rst:			:type dRef: :class:`DerivationRef`
			// rst:			:param eDup: the duplicate number of the derivation to reconnect.
			// rst:			:type eDup: unsigned int
			// rst:			:param g: a graph representing the head vertex to reconnect.
			// rst:			:type g: :class:`Graph`
			// rst:			:param vDupTar: the new duplicate number for the tail vertex.
			// rst:			:type vDupTar: unsigned int
			.def("reconnectHead", &mod::DGPrintData::reconnectHead)
			;

	mod::GraphPrinter & (mod::DGPrinter::*DGPrinter_getGraphPrinter)() = &mod::DGPrinter::getGraphPrinter;
	// rst: .. py:class:: DGPrinter
	// rst:
	// rst:		This class is used to configure how derivation graphs are visualised,
	// rst:		how much is visualised and which extra properties are printed.
	// rst:
	py::class_<mod::DGPrinter, boost::noncopyable>("DGPrinter")
			// rst:		.. py:attribute:: graphPrinter
			// rst:
			// rst:			(Read-only) Retrieve the :class:`GraphPrinter` used when printing images of graphs.
			// rst:
			// rst:			:type: :class:`GraphPrinter`
			.add_property("graphPrinter", py::make_function(DGPrinter_getGraphPrinter, py::return_internal_reference<1>()))
			// rst:		.. py:attribute:: withShortcutEdges
			// rst:
			// rst:			Control whether or not hyperedges :math:`(T, H)` with :math:`|T| = |H| = 1`
			// rst:			are printed as a simple arrow without the usual rectangle.
			// rst:
			// rst:			:type: bool
			.add_property("withShortcutEdges", &mod::DGPrinter::getWithShortcutEdges, &mod::DGPrinter::setWithShortcutEdges)
			// rst:		.. py:attribute:: withGraphImages
			// rst:
			// rst:			Control whether or not each vertex is printed with a image of its graph in it.
			// rst:
			// rst:			:type: bool
			.add_property("withGraphImages", &mod::DGPrinter::getWithGraphImages, &mod::DGPrinter::setWithGraphImages)
			// rst:		.. py:attribute:: labelsAsLatexMath
			// rst:
			// rst:			Control whether or not labels on vertices and hyperedges are put inline :math:`\LaTeX` math.
			// rst:			Specifically, a label ``label`` is written as ``$\mathrm{label'}$``, with ``label'`` being
			// rst:			``label`` with all space characters escaped.
			// rst:
			// rst:			:type: bool
			.add_property("labelsAsLatexMath", &mod::DGPrinter::getLabelsAsLatexMath, &mod::DGPrinter::setLabelsAsLatexMath)
			// rst:		.. py:method:: pushVertexVisible(f)
			// rst:
			// rst:			Add another function controlling the visibility of vertices.
			// rst:			All visibility functions must return ``true`` for a vertex to be visible.
			// rst:
			// rst:			:param f: the function to push for specifying vertex visibility.
			// rst:			:type f: bool(:class:`Graph`, :class:`DG`)
			.def("pushVertexVisible", &DGPrinter_pushVertexVisible)
			// rst:		.. py:method:: popVertexVisible()
			// rst:
			// rst:			Remove the last pushed vertex visibility function.
			.def("popVertexVisible", &mod::DGPrinter::popVertexVisible)
			// rst:		.. py:method:: pushEdgeVisible(f)
			// rst:
			// rst:			Add another function controlling the visibility of hyperedges.
			// rst:			All visibility functions must return ``true`` for a hyperedge to be visible.
			// rst:
			// rst:			:param f: the function to push for specifying hyperedge visibility.
			// rst:			:type f: bool(:class:`DerivationRef`)
			.def("pushEdgeVisible", &DGPrinter_pushEdgeVisible)
			// rst:		.. py:method:: popEdgeVisible()
			// rst:
			// rst:			Remove the last pushed hyperedge visibility function.
			.def("popEdgeVisible", &mod::DGPrinter::popEdgeVisible)
			// rst:		.. py:attribute:: withShortcutEdgesAfterVisibility
			// rst:
			// rst:			Control whether or not hyperedges are depicted as simple arrows when all but 1 head and tail are hidden.
			// rst:
			// rst:			:type: bool
			.add_property("withShortcutEdgesAfterVisibility", &mod::DGPrinter::getWithShortcutEdgesAfterVisibility, &mod::DGPrinter::setWithShortcutEdgesAfterVisibility)
			// rst:		.. py:attribute:: vertexLabelSep
			// rst:
			// rst:			The string used as separator between each part of each vertex label.
			// rst:	
			// rst:			:type: string
			.add_property("vertexLabelSep",
			py::make_function(&mod::DGPrinter::getVertexLabelSep, py::return_value_policy<py::copy_const_reference>()),
			&mod::DGPrinter::setVertexLabelSep)
			// rst:		.. py:attribute:: edgeLabelSep
			// rst:
			// rst:			The string used as separator between each part of each edge label.
			// rst:
			// rst:			:type: string
			.add_property("edgeLabelSep",
			py::make_function(&mod::DGPrinter::getEdgeLabelSep, py::return_value_policy<py::copy_const_reference>()),
			&mod::DGPrinter::setEdgeLabelSep)
			// rst:		.. py:method:: pushVertexLabel(f)
			// rst:
			// rst:			Add another function for vertex labelling. The result of this function is added to the end of each label.
			// rst:
			// rst:			:param f: the function to push for labelling vertices.
			// rst:			:type f: string(:class:`Graph`, :class:`DG`)
			.def("pushVertexLabel", &DGPrinter_pushVertexLabel)
			// rst:		.. py:method:: popVertexLabel()
			// rst:
			// rst:			Remove the last pushed vertex labelling function.
			.def("popVertexLabel", &mod::DGPrinter::popVertexLabel)
			// rst:		.. py:method:: pushEdgeLabel(f)
			// rst:
			// rst:			Add another function for edge labelling. The result of this function is appended to each label.
			// rst:
			// rst:			:param f: the function to push for labelling edges.
			// rst:			:type f: string(:class:`DerivationRef`)
			.def("pushEdgeLabel", &DGPrinter_pushEdgeLabel)
			// rst:		.. py:method:: popEdgeLabel()
			// rst:
			// rst:			Remove the last pushed edge labelling function.
			.def("popEdgeLabel", &mod::DGPrinter::popEdgeLabel)
			// rst:		.. py:attribute:: withGraphName
			// rst:
			// rst:			Control whether or not graph names are appended to the vertex labels (see also :meth:`DGPrinter.pushVertexLabel`).
			// rst:
			// rst:			:type: bool
			.add_property("withGraphName", &mod::DGPrinter::getWithGraphName, &mod::DGPrinter::setWithGraphName)
			// rst:		.. py:attribute:: withRuleName
			// rst:
			// rst:			Control whether or not rule names are appended to the edge labels (see also :meth:`DGPrinter.pushEdgeLabel`).
			// rst:
			// rst:			:type: bool
			.add_property("withRuleName", &mod::DGPrinter::getWithRuleName, &mod::DGPrinter::setWithRuleName)
			// rst:		.. py:attribute:: withRuleId
			// rst:
			// rst:			Control whether or not rule IDs are appended to the edge labels (see also :meth:`DGPrinter.pushEdgeLabel`).
			// rst:
			// rst:			:type: bool
			.add_property("withRuleId", &mod::DGPrinter::getWithRuleId, &mod::DGPrinter::setWithRuleId)
			// rst:		.. py:method:: pushVertexColour(f, extendToEdges=True)
			// rst:
			// rst:			Add another function for colouring vertices. The final colour of a vertex is the result of the first colour function returning a non-empty string.
			// rst:			The functions are evaluated in the order they are pushed and the resulting string is used directly as a colour in Tikz.
			// rst:			A hyperedge is also coloured if at least one head and one tail *can* be coloured with a colour for which ``extendToEdges`` is ``True``.
			// rst:			In this case, the hyperedge (and a subset of the head and tail connectors) is coloured with the first applicable colour.
			// rst:			The edge extension of vertex colour takes lower precedence than explicitly added hyperedge colouring functions.
			// rst:
			// rst:			:param f: the function to push for colouring vertices.
			// rst:			:type f: string(:class:`Graph`, :class:`DG`)
			// rst:			:param bool extendToEdges: whether or not some hyperedges are coloured as well (see above).
			.def("pushVertexColour", &DGPrinter_pushVertexColour)
			// rst:		.. py:method:: popVertexColour()
			// rst:
			// rst:			Remove the last pushed vertex colouring function.
			.def("popVertexColour", &mod::DGPrinter::popVertexColour)
			// rst:		.. py:method:: pushEdgeColour(f)
			// rst:
			// rst:			Add another function for colouring hyperedges. The final colour of a hyperedge (and all of its head and tail connectors) is the result of the
			// rst:			first colour function returning a non-empty string.
			// rst:			
			// rst:			:param f: the function to push for colouring hyperedges.
			// rst:			:type f: string(:class:`DerivationRef`)
			.def("pushEdgeColour", &DGPrinter_pushEdgeColour)
			// rst:		.. py:method:: popEdgeColour()
			// rst:
			// rst:			Remove the last pushed hyperedge colouring function.
			.def("popEdgeColour", &mod::DGPrinter::popEdgeColour)
			;
}

} // namespace Py
} // namespace mod
