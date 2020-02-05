#include <mod/py/Common.hpp>

#include <mod/Chem.hpp>
#include <mod/graph/Graph.hpp>
#include <mod/graph/Printer.hpp>
#include <mod/rule/GraphInterface.hpp>

namespace mod {
namespace rule {
namespace Py {
namespace {

void exportLeft() {
	std::string(Rule::LeftGraph::Vertex::*printStereoWithoutOptions)() const = &Rule::LeftGraph::Vertex::printStereo;
	std::string(Rule::LeftGraph::Vertex::*printStereoWithOptions)(const graph::Printer&) const = &Rule::LeftGraph::Vertex::printStereo;

	// rst: .. py:class:: RuleLeftGraph
	// rst:
	// rst:		A proxy object representing the left graph of the rule.
	// rst:
	py::class_<Rule::LeftGraph>("RuleLeftGraph", py::no_init)
			// rst:		.. py:attribute:: rule
			// rst:
			// rst:			(Read-only) The rule the graph belongs to.
			// rst:
			// rst:			:type: Rule
			.add_property("rule", &Rule::LeftGraph::getRule)
			// rst:		.. py:attribute:: numVertices
			// rst:
			// rst:			(Read-only) The number of vertices in the graph.
			// rst:
			// rst:			:type: int
			.add_property("numVertices", &Rule::LeftGraph::numVertices)
			// rst:		.. py:attribute:: vertices
			// rst:
			// rst:			(Read-only) An iterable of all vertices in the graph.
			// rst:
			// rst:			:type: RuleLeftGraphVertexRange
			.add_property("vertices", &Rule::LeftGraph::vertices)
			// rst:		.. py:attribute:: numEdges
			// rst:
			// rst:			(Read-only) The number of edges in the graph.
			// rst:
			// rst:			:type: int
			.add_property("numEdges", &Rule::LeftGraph::numEdges)
			// rst:		.. py:attribute:: edges
			// rst:
			// rst:			(Read-only) An iterable of all edges in the graph.
			// rst:
			// rst:			:type: RuleLeftGraphEdgeRange
			.add_property("edges", &Rule::LeftGraph::edges)
			;

	// rst: .. py:class:: RuleLeftGraphVertex
	// rst:
	// rst:		A descriptor of either a vertex in a left side, or a null vertex.
	// rst:
	py::class_<Rule::LeftGraph::Vertex>("RuleLeftGraphVertex", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst: 
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			.def("__hash__", &Rule::LeftGraph::Vertex::hash)
			// rst:		.. py::method:: __bool__(self)
			// rst:
			// rst:			:returns: ``not isNull()``
			// rst:			:rtype: bool
			.def("__bool__", &Rule::LeftGraph::Vertex::operator bool)
			// rst:		.. py:method:: isNull()
			// rst:
			// rst:			:returns: whether this is a null descriptor or not.
			// rst:			:rtype: bool
			.def("isNull", &Rule::LeftGraph::Vertex::isNull)
			// rst:		.. py:attribute:: id
			// rst:
			// rst:			(Read-only) The index of the vertex. It will be in the range :math:`[0, numCoreVertices[`.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("id", &Rule::LeftGraph::Vertex::getId)
			// rst:		.. py:attribute:: rule
			// rst:
			// rst:			(Read-only) The rule the vertex belongs to.
			// rst:
			// rst:			:type: Rule
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("rule", &Rule::LeftGraph::Vertex::getRule)
			// rst:		.. py:attribute:: core
			// rst:
			// rst:			(Read-only) The descriptor for this vertex in the core graph.
			// rst:
			// rst:			:type: RuleVertex
			.add_property("core", &Rule::LeftGraph::Vertex::getCore)
			// rst:		.. py:attribute:: degree
			// rst:
			// rst:			(Read-only) The degree of the vertex.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("degree", &Rule::LeftGraph::Vertex::getDegree)
			// rst:		.. py:attribute:: incidentEdges
			// rst:
			// rst:			(Read-only) A range of incident edges to this vertex.
			// rst:
			// rst:			:type: RuleLeftGraphIncidentEdgeRange
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("incidentEdges", &Rule::LeftGraph::Vertex::incidentEdges)
			// rst:		.. py:attribute:: stringLabel
			// rst:
			// rst:			(Read-only) The string label of the vertex.
			// rst:
			// rst:			:type: str
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("stringLabel", py::make_function(&Rule::LeftGraph::Vertex::getStringLabel, py::return_value_policy<py::copy_const_reference>()))
			// rst:		.. py:attribute:: atomId
			// rst:
			// rst:			(Read-only) The atom id of the vertex.
			// rst:
			// rst:			:type: AtomId
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("atomId", &Rule::LeftGraph::Vertex::getAtomId)
			// rst:		.. py:attribute:: isotope
			// rst:
			// rst:			(Read-only) The isotope of the vertex.
			// rst:
			// rst:			:type: Isotope
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("isotope", &Rule::LeftGraph::Vertex::getIsotope)
			// rst:		.. py:attribute:: charge
			// rst:
			// rst:			(Read-only) The charge of the vertex.
			// rst:
			// rst:			:type: Charge
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("charge", &Rule::LeftGraph::Vertex::getCharge)
			// rst:		.. py:attribute:: radical
			// rst:
			// rst:			(Read-only) The radical status of the vertex.
			// rst:
			// rst:			:type: bool
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("radical", &Rule::LeftGraph::Vertex::getRadical)
			// rst:		.. py:method:: printStereo()
			// rst:                  printStereo(p)
			// rst:
			// rst:			Print the stereo configuration for the vertex.
			// rst:
			// rst:			:param GraphPrinter p: the printing options used for the depiction.
			// rst:			:returns: the name of the PDF-file that will be compiled in post-processing.
			// rst:			:rtype: str
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.def("printStereo", printStereoWithoutOptions)
			.def("printStereo", printStereoWithOptions)
			;

	// rst: .. py:class:: RuleLeftGraphEdge
	// rst:
	// rst:		A descriptor of either an edge in a left side, or a null edge.
	// rst:
	py::class_<Rule::LeftGraph::Edge>("RuleLeftGraphEdge", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst:
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			// rst:		.. py::method:: __bool__(self)
			// rst:
			// rst:			:returns: ``not isNull()``
			// rst:			:rtype: bool
			.def("__bool__", &Rule::LeftGraph::Edge::operator bool)
			// rst:		.. py:method:: isNull()
			// rst:
			// rst:			:returns: whether this is a null descriptor or not.
			// rst:			:rtype: bool
			.def("isNull", &Rule::LeftGraph::Edge::isNull)
			// rst:		.. py:attribute:: graph
			// rst:
			// rst:			(Read-only) The rule the edge belongs to.
			// rst:
			// rst:			:type: Rule
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("rule", &Rule::LeftGraph::Edge::getRule)
			// rst:		.. py:attribute:: core
			// rst:
			// rst:			(Read-only) The descriptor for this edge in the core graph.
			// rst:
			// rst:			:type: RuleEdge
			.add_property("core", &Rule::LeftGraph::Edge::getCore)
			// rst:		.. py:attribute:: source
			// rst:
			// rst:			(Read-only) The source vertex of the edge.
			// rst:
			// rst:			:type: RuleLeftGraphVertex
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("source", &Rule::LeftGraph::Edge::source)
			// rst:		.. attribute:: target
			// rst:
			// rst:			(Read-only) The target vertex of the edge.
			// rst:
			// rst:			:type: RuleLeftGraphVertex
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("target", &Rule::LeftGraph::Edge::target)
			// rst:		.. py:attribute:: stringLabel
			// rst:
			// rst:			(Read-only) The string label of the edge.
			// rst:
			// rst:			:type: str
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("stringLabel", py::make_function(&Rule::LeftGraph::Edge::getStringLabel, py::return_value_policy<py::copy_const_reference>()))
			// rst:		.. py:attribute:: bondType
			// rst:
			// rst:			(Read-only) The bond type of the edge.
			// rst:
			// rst:			:type: BondType
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("bondType", &Rule::LeftGraph::Edge::getBondType)
			;

	py::class_<Rule::LeftGraph::VertexRange>("RuleLeftGraphVertexRange", py::no_init)
			.def("__iter__", py::iterator<Rule::LeftGraph::VertexRange>())

			;
	py::class_<Rule::LeftGraph::EdgeRange>("RuleLeftGraphEdgeRange", py::no_init)
			.def("__iter__", py::iterator<Rule::LeftGraph::EdgeRange>())
			;
	py::class_<Rule::LeftGraph::IncidentEdgeRange>("RuleLeftGraphIncidentEdgeRange", py::no_init)
			.def("__iter__", py::iterator<Rule::LeftGraph::IncidentEdgeRange>())
			;
}

void exportContext() {
	// rst: .. py:class:: RuleContextGraph
	// rst:
	// rst:		A proxy object representing the context graph of the rule.
	// rst:
	py::class_<Rule::ContextGraph>("RuleContextGraph", py::no_init)
			// rst:		.. py:attribute:: rule
			// rst:
			// rst:			(Read-only) The rule the graph belongs to.
			// rst:
			// rst:			:type: Rule
			.add_property("rule", &Rule::ContextGraph::getRule)
			// rst:		.. py:attribute:: numVertices
			// rst:
			// rst:			(Read-only) The number of vertices in the graph.
			// rst:
			// rst:			:type: int
			.add_property("numVertices", &Rule::ContextGraph::numVertices)
			// rst:		.. py:attribute:: vertices
			// rst:
			// rst:			(Read-only) An iterable of all vertices in the graph.
			// rst:
			// rst:			:type: RuleContextGraphVertexRange
			.add_property("vertices", &Rule::ContextGraph::vertices)
			// rst:		.. py:attribute:: numEdges
			// rst:
			// rst:			(Read-only) The number of edges in the graph.
			// rst:
			// rst:			:type: int
			.add_property("numEdges", &Rule::ContextGraph::numEdges)
			// rst:		.. py:attribute:: edges
			// rst:
			// rst:			(Read-only) An iterable of all edges in the graph.
			// rst:
			// rst:			:type: RuleContextGraphEdgeRange
			.add_property("edges", &Rule::ContextGraph::edges)
			;

	// rst: .. py:class:: RuleLeftGraphVertex
	// rst:
	// rst:		A descriptor of either a vertex in a context, or a null vertex.
	// rst:
	py::class_<Rule::ContextGraph::Vertex>("RuleLeftGraphVertex", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst: 
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			.def("__hash__", &Rule::ContextGraph::Vertex::hash)
			// rst:		.. py::method:: __bool__(self)
			// rst:
			// rst:			:returns: ``not isNull()``
			// rst:			:rtype: bool
			.def("__bool__", &Rule::ContextGraph::Vertex::operator bool)
			// rst:		.. py:method:: isNull()
			// rst:
			// rst:			:returns: whether this is a null descriptor or not.
			// rst:			:rtype: bool
			.def("isNull", &Rule::ContextGraph::Vertex::isNull)
			// rst:		.. py:attribute:: id
			// rst:
			// rst:			(Read-only) The index of the vertex. It will be in the range :math:`[0, numCoreVertices[`.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("id", &Rule::ContextGraph::Vertex::getId)
			// rst:		.. py:attribute:: rule
			// rst:
			// rst:			(Read-only) The rule the vertex belongs to.
			// rst:
			// rst:			:type: Rule
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("rule", &Rule::ContextGraph::Vertex::getRule)
			// rst:		.. py:attribute:: core
			// rst:
			// rst:			(Read-only) The descriptor for this vertex in the core graph.
			// rst:
			// rst:			:type: RuleVertex
			.add_property("core", &Rule::ContextGraph::Vertex::getCore)
			// rst:		.. py:attribute:: degree
			// rst:
			// rst:			(Read-only) The degree of the vertex.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("degree", &Rule::ContextGraph::Vertex::getDegree)
			// rst:		.. py:attribute:: incidentEdges
			// rst:
			// rst:			(Read-only) A range of incident edges to this vertex.
			// rst:
			// rst:			:type: RuleContextGraphIncidentEdgeRange
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("incidentEdges", &Rule::ContextGraph::Vertex::incidentEdges)
			;

	// rst: .. py:class:: RuleContextGraphEdge
	// rst:
	// rst:		A descriptor of either an edge in a context, or a null edge.
	// rst:
	py::class_<Rule::ContextGraph::Edge>("RuleContextGraphEdge", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst:
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			// rst:		.. py::method:: __bool__(self)
			// rst:
			// rst:			:returns: ``not isNull()``
			// rst:			:rtype: bool
			.def("__bool__", &Rule::ContextGraph::Edge::operator bool)
			// rst:		.. py:method:: isNull()
			// rst:
			// rst:			:returns: whether this is a null descriptor or not.
			// rst:			:rtype: bool
			.def("isNull", &Rule::ContextGraph::Edge::isNull)
			// rst:		.. py:attribute:: graph
			// rst:
			// rst:			(Read-only) The rule the edge belongs to.
			// rst:
			// rst:			:type: Rule
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("rule", &Rule::ContextGraph::Edge::getRule)
			// rst:		.. py:attribute:: core
			// rst:
			// rst:			(Read-only) The descriptor for this edge in the core graph.
			// rst:
			// rst:			:type: RuleEdge
			.add_property("core", &Rule::ContextGraph::Edge::getCore)
			// rst:		.. py:attribute:: source
			// rst:
			// rst:			(Read-only) The source vertex of the edge.
			// rst:
			// rst:			:type: RuleContextGraphVertex
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("source", &Rule::ContextGraph::Edge::source)
			// rst:		.. attribute:: target
			// rst:
			// rst:			(Read-only) The target vertex of the edge.
			// rst:
			// rst:			:type: RuleContextGraphVertex
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("target", &Rule::ContextGraph::Edge::target)
			;

	py::class_<Rule::ContextGraph::VertexRange>("RuleContextGraphVertexRange", py::no_init)
			.def("__iter__", py::iterator<Rule::ContextGraph::VertexRange>())

			;
	py::class_<Rule::ContextGraph::EdgeRange>("RuleContextGraphEdgeRange", py::no_init)
			.def("__iter__", py::iterator<Rule::ContextGraph::EdgeRange>())
			;
	py::class_<Rule::ContextGraph::IncidentEdgeRange>("RuleContextGraphIncidentEdgeRange", py::no_init)
			.def("__iter__", py::iterator<Rule::ContextGraph::IncidentEdgeRange>())
			;
}

void exportRight() {
	std::string(Rule::RightGraph::Vertex::*printStereoWithoutOptions)() const = &Rule::RightGraph::Vertex::printStereo;
	std::string(Rule::RightGraph::Vertex::*printStereoWithOptions)(const graph::Printer&) const = &Rule::RightGraph::Vertex::printStereo;

	// rst: .. py:class:: RuleRightGraph
	// rst:
	// rst:		A proxy object representing the right graph of the rule.
	// rst:
	py::class_<Rule::RightGraph>("RuleRightGraph", py::no_init)
			// rst:		.. py:attribute:: rule
			// rst:
			// rst:			(Read-only) The rule the graph belongs to.
			// rst:
			// rst:			:type: Rule
			.add_property("rule", &Rule::RightGraph::getRule)
			// rst:		.. py:attribute:: numVertices
			// rst:
			// rst:			(Read-only) The number of vertices in the graph.
			// rst:
			// rst:			:type: int
			.add_property("numVertices", &Rule::RightGraph::numVertices)
			// rst:		.. py:attribute:: vertices
			// rst:
			// rst:			(Read-only) An iterable of all vertices in the graph.
			// rst:
			// rst:			:type: RuleRightGraphVertexRange
			.add_property("vertices", &Rule::RightGraph::vertices)
			// rst:		.. py:attribute:: numEdges
			// rst:
			// rst:			(Read-only) The number of edges in the graph.
			// rst:
			// rst:			:type: int
			.add_property("numEdges", &Rule::RightGraph::numEdges)
			// rst:		.. py:attribute:: edges
			// rst:
			// rst:			(Read-only) An iterable of all edges in the graph.
			// rst:
			// rst:			:type: RuleRightGraphEdgeRange
			.add_property("edges", &Rule::RightGraph::edges)
			;

	// rst: .. py:class:: RuleRightGraphVertex
	// rst:
	// rst:		A descriptor of either a vertex in a right side, or a null vertex.
	// rst:
	py::class_<Rule::RightGraph::Vertex>("RuleRightGraphVertex", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst: 
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			.def("__hash__", &Rule::RightGraph::Vertex::hash)
			// rst:		.. py::method:: __bool__(self)
			// rst:
			// rst:			:returns: ``not isNull()``
			// rst:			:rtype: bool
			.def("__bool__", &Rule::RightGraph::Vertex::operator bool)
			// rst:		.. py:method:: isNull()
			// rst:
			// rst:			:returns: whether this is a null descriptor or not.
			// rst:			:rtype: bool
			.def("isNull", &Rule::RightGraph::Vertex::isNull)
			// rst:		.. py:attribute:: id
			// rst:
			// rst:			(Read-only) The index of the vertex. It will be in the range :math:`[0, numCoreVertices[`.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("id", &Rule::RightGraph::Vertex::getId)
			// rst:		.. py:attribute:: rule
			// rst:
			// rst:			(Read-only) The rule the vertex belongs to.
			// rst:
			// rst:			:type: Rule
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("rule", &Rule::RightGraph::Vertex::getRule)
			// rst:		.. py:attribute:: core
			// rst:
			// rst:			(Read-only) The descriptor for this vertex in the core graph.
			// rst:
			// rst:			:type: RuleVertex
			.add_property("core", &Rule::RightGraph::Vertex::getCore)
			// rst:		.. py:attribute:: degree
			// rst:
			// rst:			(Read-only) The degree of the vertex.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("degree", &Rule::RightGraph::Vertex::getDegree)
			// rst:		.. py:attribute:: incidentEdges
			// rst:
			// rst:			(Read-only) A range of incident edges to this vertex.
			// rst:
			// rst:			:type: RuleRightGraphIncidentEdgeRange
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("incidentEdges", &Rule::RightGraph::Vertex::incidentEdges)
			// rst:		.. py:attribute:: stringLabel
			// rst:
			// rst:			(Read-only) The string label of the vertex.
			// rst:
			// rst:			:type: str
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("stringLabel", py::make_function(&Rule::RightGraph::Vertex::getStringLabel, py::return_value_policy<py::copy_const_reference>()))
			// rst:		.. py:attribute:: atomId
			// rst:
			// rst:			(Read-only) The atom id of the vertex.
			// rst:
			// rst:			:type: AtomId
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("atomId", &Rule::RightGraph::Vertex::getAtomId)
			// rst:		.. py:attribute:: isotope
			// rst:
			// rst:			(Read-only) The isotope of the vertex.
			// rst:
			// rst:			:type: Isotope
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("isotope", &Rule::RightGraph::Vertex::getIsotope)
			// rst:		.. py:attribute:: charge
			// rst:
			// rst:			(Read-only) The charge of the vertex.
			// rst:
			// rst:			:type: Charge
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("charge", &Rule::RightGraph::Vertex::getCharge)
			// rst:		.. py:attribute:: radical
			// rst:
			// rst:			(Read-only) The radical status of the vertex.
			// rst:
			// rst:			:type: bool
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("radical", &Rule::RightGraph::Vertex::getRadical)
			// rst:		.. py:method:: printStereo()
			// rst:                  printStereo(p)
			// rst:
			// rst:			Print the stereo configuration for the vertex.
			// rst:
			// rst:			:param GraphPrinter p: the printing options used for the depiction.
			// rst:			:returns: the name of the PDF-file that will be compiled in post-processing.
			// rst:			:rtype: str
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.def("printStereo", printStereoWithoutOptions)
			.def("printStereo", printStereoWithOptions)
			;

	// rst: .. py:class:: RuleRightGraphEdge
	// rst:
	// rst:		A descriptor of either an edge in a right side, or a null edge.
	// rst:
	py::class_<Rule::RightGraph::Edge>("RuleRightGraphEdge", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst:
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			// rst:		.. py::method:: __bool__(self)
			// rst:
			// rst:			:returns: ``not isNull()``
			// rst:			:rtype: bool
			.def("__bool__", &Rule::RightGraph::Edge::operator bool)
			// rst:		.. py:method:: isNull()
			// rst:
			// rst:			:returns: whether this is a null descriptor or not.
			// rst:			:rtype: bool
			.def("isNull", &Rule::RightGraph::Edge::isNull)
			// rst:		.. py:attribute:: graph
			// rst:
			// rst:			(Read-only) The rule the edge belongs to.
			// rst:
			// rst:			:type: Rule
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("rule", &Rule::RightGraph::Edge::getRule)
			// rst:		.. py:attribute:: core
			// rst:
			// rst:			(Read-only) The descriptor for this edge in the core graph.
			// rst:
			// rst:			:type: RuleEdge
			.add_property("core", &Rule::RightGraph::Edge::getCore)
			// rst:		.. py:attribute:: source
			// rst:
			// rst:			(Read-only) The source vertex of the edge.
			// rst:
			// rst:			:type: RuleRightGraphVertex
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("source", &Rule::RightGraph::Edge::source)
			// rst:		.. attribute:: target
			// rst:
			// rst:			(Read-only) The target vertex of the edge.
			// rst:
			// rst:			:type: RuleRightGraphVertex
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("target", &Rule::RightGraph::Edge::target)
			// rst:		.. py:attribute:: stringLabel
			// rst:
			// rst:			(Read-only) The string label of the edge.
			// rst:
			// rst:			:type: str
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("stringLabel", py::make_function(&Rule::RightGraph::Edge::getStringLabel, py::return_value_policy<py::copy_const_reference>()))
			// rst:		.. py:attribute:: bondType
			// rst:
			// rst:			(Read-only) The bond type of the edge.
			// rst:
			// rst:			:type: BondType
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("bondType", &Rule::RightGraph::Edge::getBondType)
			;

	py::class_<Rule::RightGraph::VertexRange>("RuleRightGraphVertexRange", py::no_init)
			.def("__iter__", py::iterator<Rule::RightGraph::VertexRange>())

			;
	py::class_<Rule::RightGraph::EdgeRange>("RuleRightGraphEdgeRange", py::no_init)
			.def("__iter__", py::iterator<Rule::RightGraph::EdgeRange>())
			;
	py::class_<Rule::RightGraph::IncidentEdgeRange>("RuleRightGraphIncidentEdgeRange", py::no_init)
			.def("__iter__", py::iterator<Rule::RightGraph::IncidentEdgeRange>())
			;
}

void exportCore() {
	// rst: .. py:class:: RuleVertex
	// rst:
	// rst:		A descriptor of either a vertex in a rule, or a null vertex.
	// rst:
	py::class_<Rule::Vertex>("RuleVertex", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst: 
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			.def("__hash__", &Rule::Vertex::hash)
			// rst:		.. py::method:: __bool__(self)
			// rst:
			// rst:			:returns: ``not isNull()``
			// rst:			:rtype: bool
			.def("__bool__", &Rule::Vertex::operator bool)
			// rst:		.. py:method:: isNull()
			// rst:
			// rst:			:returns: whether this is a null descriptor or not.
			// rst:			:rtype: bool
			.def("isNull", &Rule::Vertex::isNull)
			// rst:		.. py:attribute:: id
			// rst:
			// rst:			(Read-only) The index of the vertex. It will be in the range :math:`[0, numVertices[`.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("id", &Rule::Vertex::getId)
			// rst:		.. py:attribute:: rule
			// rst:
			// rst:			(Read-only) The rule the vertex belongs to.
			// rst:
			// rst:			:type: Rule
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("rule", &Rule::Vertex::getRule)
			// rst:		.. py:attribute:: left
			// rst:
			// rst:			(Read-only) A null descriptor if this vertex is not in the left graph,
			// rst:			otherwise the descriptor of this vertex in the left graph.
			.add_property("left", &Rule::Vertex::getLeft)
			// rst:		.. py:attribute:: context
			// rst:
			// rst:			(Read-only) A null descriptor if this vertex is not in the context graph,
			// rst:			otherwise the descriptor of this vertex in the context graph.
			.add_property("context", &Rule::Vertex::getContext)
			// rst:		.. py:attribute:: left
			// rst:
			// rst:			(Read-only) A null descriptor if this vertex is not in the right graph,
			// rst:			otherwise the descriptor of this vertex in the right graph.
			.add_property("right", &Rule::Vertex::getRight)
			// rst:		.. py:attribute:: degree
			// rst:
			// rst:			(Read-only) The degree of the vertex.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("degree", &Rule::Vertex::getDegree)
			// rst:		.. py:attribute:: incidentEdges
			// rst:
			// rst:			(Read-only) A range of incident edges to this vertex.
			// rst:
			// rst:			:type: RuleIncidentEdgeRange
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("incidentEdges", &Rule::Vertex::incidentEdges)
			// rst:		.. py:method:: get2DX(withHydrogens=True)
			// rst:
			// rst:			:returns: the x-coordinate in a 2D depiction of the rule.
			// rst:				Different sets of coordinates exists for rendering with and wihout certain hydrogens.
			// rst:			:rtype: float
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor, or
			// rst:				if ``withHydrogens`` is ``True`` and the vertex is a "clean" hydrogen.
			.def("get2DX", &Rule::Vertex::get2DX, py::arg("withHydrogens") = true)
			// rst:		.. py:method:: get2DY(withHydrogens=True)
			// rst:
			// rst:			:returns: the y-coordinate in a 2D depiction of the rule.
			// rst:				Different sets of coordinates exists for rendering with and wihout certain hydrogens.
			// rst:			:rtype: float
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor, or
			// rst:				if ``withHydrogens`` is ``True`` and the vertex is a "clean" hydrogen.
			.def("get2DY", &Rule::Vertex::get2DY, py::arg("withHydrogens") = true)
			;

	// rst: .. py:class:: RuleEdge
	// rst:
	// rst:		A descriptor of either an edge in a rule, or a null edge.
	// rst:
	py::class_<Rule::Edge>("RuleEdge", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst:
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			// rst:		.. py::method:: __bool__(self)
			// rst:
			// rst:			:returns: ``not isNull()``
			// rst:			:rtype: bool
			.def("__bool__", &Rule::Edge::operator bool)
			// rst:		.. py:method:: isNull()
			// rst:
			// rst:			:returns: whether this is a null descriptor or not.
			// rst:			:rtype: bool
			.def("isNull", &Rule::Edge::isNull)
			// rst:		.. py:attribute:: rule
			// rst:
			// rst:			(Read-only) The rule the edge belongs to.
			// rst:
			// rst:			:type: Rule
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("rule", &Rule::Edge::getRule)
			// rst:		.. py:attribute:: left
			// rst:
			// rst:			(Read-only) A null descriptor if this edge is not in the left graph,
			// rst:			otherwise the descriptor of this edge in the left graph.
			.add_property("left", &Rule::Edge::getLeft)
			// rst:		.. py:attribute:: context
			// rst:
			// rst:			(Read-only) A null descriptor if this edge is not in the context graph,
			// rst:			otherwise the descriptor of this edge in the context graph.
			.add_property("context", &Rule::Edge::getContext)
			// rst:		.. py:attribute:: left
			// rst:
			// rst:			(Read-only) A null descriptor if this edge is not in the right graph,
			// rst:			otherwise the descriptor of this edge in the right graph.
			.add_property("right", &Rule::Edge::getRight)
			// rst:		.. py:attribute:: source
			// rst:
			// rst:			(Read-only) The source vertex of the edge.
			// rst:
			// rst:			:type: RuleVertex
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("source", &Rule::Edge::source)
			// rst:		.. attribute:: target
			// rst:
			// rst:			(Read-only) The target vertex of the edge.
			// rst:
			// rst:			:type: RuleVertex
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("target", &Rule::Edge::target)
			;

	py::class_<Rule::VertexRange>("RuleVertexRange", py::no_init)
			.def("__iter__", py::iterator<Rule::VertexRange>())
			.def("__getitem__", &Rule::VertexRange::operator[])
			;
	py::class_<Rule::EdgeRange>("RuleEdgeRange", py::no_init)
			.def("__iter__", py::iterator<Rule::EdgeRange>())
			;
	py::class_<Rule::IncidentEdgeRange>("RuleIncidentEdgeRange", py::no_init)
			.def("__iter__", py::iterator<Rule::IncidentEdgeRange>())
			;
}

} // namespace

void GraphInterface_doExport() {
	exportLeft();
	exportContext();
	exportRight();
	exportCore();
}

} // namespace Py
} // namespace rule
} // namespace mod