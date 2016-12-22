#include <mod/Py/Common.h>

#include <mod/Chem.h>
#include <mod/RuleGraphInterface.h>

namespace mod {
namespace Py {
namespace {

void exportLeft() {
	// rst: .. py:class:: RuleLeftGraph
	// rst:
	// rst:		A proxy object representing the left graph of the rule.
	// rst:
	py::class_<mod::Rule::LeftGraph>("RuleLeftGraph", py::no_init)
			// rst:		.. py:attribute:: rule
			// rst:
			// rst:			(Read-only) The rule the graph belongs to.
			// rst:
			// rst:			:type: :py:class:`Rule`
			.add_property("rule", &mod::Rule::LeftGraph::getRule)
			// rst:		.. py:attribute:: numVertices
			// rst:
			// rst:			(Read-only) The number of vertices in the graph.
			// rst:
			// rst:			:type: int
			.add_property("numVertices", &mod::Rule::LeftGraph::numVertices)
			// rst:		.. py:attribute:: vertices
			// rst:
			// rst:			(Read-only) An iterable of all vertices in the graph.
			// rst:
			// rst:			:type: :py:class:`RuleLeftGraphVertexRange`
			.add_property("vertices", &mod::Rule::LeftGraph::vertices)
			// rst:		.. py:attribute:: numEdges
			// rst:
			// rst:			(Read-only) The number of edges in the graph.
			// rst:
			// rst:			:type: int
			.add_property("numEdges", &mod::Rule::LeftGraph::numEdges)
			// rst:		.. py:attribute:: edges
			// rst:
			// rst:			(Read-only) An iterable of all edges in the graph.
			// rst:
			// rst:			:type: :py:class:`RuleLeftGraphEdgeRange`
			.add_property("edges", &mod::Rule::LeftGraph::edges)
			;

	// rst: .. py:class:: RuleLeftGraphVertex
	// rst:
	// rst:		A descriptor of either a vertex in a left side, or a null vertex.
	// rst:
	py::class_<mod::Rule::LeftGraph::Vertex>("RuleLeftGraphVertex", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst: 
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			// rst:		.. py:method:: isNull()
			// rst:
			// rst:			:returns: whether this is a null descriptor or not.
			// rst:			:rtype: bool
			.def("isNull", &mod::Rule::LeftGraph::Vertex::isNull)
			// rst:		.. py:attribute:: id
			// rst:
			// rst:			(Read-only) The index of the vertex. It will be in the range :math:`[0, numCoreVertices[`.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("id", &mod::Rule::LeftGraph::Vertex::getId)
			// rst:		.. py:attribute:: rule
			// rst:
			// rst:			(Read-only) The rule the vertex belongs to.
			// rst:
			// rst:			:type: :py:class:`Rule`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("rule", &mod::Rule::LeftGraph::Vertex::getRule)
			// rst:		.. py:attribute:: core
			// rst:
			// rst:			(Read-only) The descriptor for this vertex in the core graph.
			// rst:
			// rst:			:type: :py:class:`RuleVertex`
			.add_property("core", &mod::Rule::LeftGraph::Vertex::getCore)
			// rst:		.. py:attribute:: degree
			// rst:
			// rst:			(Read-only) The degree of the vertex.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("degree", &mod::Rule::LeftGraph::Vertex::getDegree)
			// rst:		.. py:attribute:: incidentEdges
			// rst:
			// rst:			(Read-only) A range of incident edges to this vertex.
			// rst:
			// rst:			:type: :py:class:`RuleLeftGraphIncidentEdgeRange`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("incidentEdges", &mod::Rule::LeftGraph::Vertex::incidentEdges)
			// rst:		.. py:attribute:: stringLabel
			// rst:
			// rst:			(Read-only) The string label of the vertex.
			// rst:
			// rst:			:type: string
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("stringLabel", py::make_function(&mod::Rule::LeftGraph::Vertex::getStringLabel, py::return_value_policy<py::copy_const_reference>()))
			// rst:		.. py:attribute:: atomId
			// rst:
			// rst:			(Read-only) The atom id of the vertex.
			// rst:
			// rst:			:type: :py:class:`AtomId`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("atomId", &mod::Rule::LeftGraph::Vertex::getAtomId)
			// rst:		.. py:attribute:: charge
			// rst:
			// rst:			(Read-only) The charge of the vertex.
			// rst:
			// rst:			:type: :py:class:`Charge`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("charge", &mod::Rule::LeftGraph::Vertex::getCharge)
			// rst:		.. py:attribute:: radical
			// rst:
			// rst:			(Read-only) The radical status of the vertex.
			// rst:
			// rst:			:type: bool
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("radical", &mod::Rule::LeftGraph::Vertex::getRadical)
			;

	// rst: .. py:class:: RuleLeftGraphEdge
	// rst:
	// rst:		A descriptor of either an edge in a left side, or a null edge.
	// rst:
	py::class_<mod::Rule::LeftGraph::Edge>("RuleLeftGraphEdge", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst:
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			// rst:		.. py:method:: isNull()
			// rst:
			// rst:			:returns: whether this is a null descriptor or not.
			// rst:			:rtype: bool
			.def("isNull", &mod::Rule::LeftGraph::Edge::isNull)
			// rst:		.. py:attribute:: graph
			// rst:
			// rst:			(Read-only) The rule the edge belongs to.
			// rst:
			// rst:			:type: :py:class:`Rule`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("rule", &mod::Rule::LeftGraph::Edge::getRule)
			// rst:		.. py:attribute:: core
			// rst:
			// rst:			(Read-only) The descriptor for this edge in the core graph.
			// rst:
			// rst:			:type: :py:class:`RuleEdge`
			.add_property("core", &mod::Rule::LeftGraph::Edge::getCore)
			// rst:		.. py:attribute:: source
			// rst:
			// rst:			(Read-only) The source vertex of the edge.
			// rst:
			// rst:			:type: :py:class:`RuleLeftGraphVertex`
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("source", &mod::Rule::LeftGraph::Edge::source)
			// rst:		.. attribute:: target
			// rst:
			// rst:			(Read-only) The target vertex of the edge.
			// rst:
			// rst:			:type: :py:class:`RuleLeftGraphVertex`
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("target", &mod::Rule::LeftGraph::Edge::target)
			// rst:		.. py:attribute:: stringLabel
			// rst:
			// rst:			(Read-only) The string label of the edge.
			// rst:
			// rst:			:type: string
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("stringLabel", py::make_function(&mod::Rule::LeftGraph::Edge::getStringLabel, py::return_value_policy<py::copy_const_reference>()))
			// rst:		.. py:attribute:: bondType
			// rst:
			// rst:			(Read-only) The bond type of the edge.
			// rst:
			// rst:			:type: :py:class:`BondType`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("bondType", &mod::Rule::LeftGraph::Edge::getBondType)
			;

	py::class_<mod::Rule::LeftGraph::VertexRange>("RuleLeftGraphVertexRange", py::no_init)
			.def("__iter__", py::iterator<mod::Rule::LeftGraph::VertexRange>())

			;
	py::class_<mod::Rule::LeftGraph::EdgeRange>("RuleLeftGraphEdgeRange", py::no_init)
			.def("__iter__", py::iterator<mod::Rule::LeftGraph::EdgeRange>())
			;
	py::class_<mod::Rule::LeftGraph::IncidentEdgeRange>("RuleLeftGraphIncidentEdgeRange", py::no_init)
			.def("__iter__", py::iterator<mod::Rule::LeftGraph::IncidentEdgeRange>())
			;
}

void exportContext() {
	// rst: .. py:class:: RuleContextGraph
	// rst:
	// rst:		A proxy object representing the context graph of the rule.
	// rst:
	py::class_<mod::Rule::ContextGraph>("RuleContextGraph", py::no_init)
			// rst:		.. py:attribute:: rule
			// rst:
			// rst:			(Read-only) The rule the graph belongs to.
			// rst:
			// rst:			:type: :py:class:`Rule`
			.add_property("rule", &mod::Rule::ContextGraph::getRule)
			// rst:		.. py:attribute:: numVertices
			// rst:
			// rst:			(Read-only) The number of vertices in the graph.
			// rst:
			// rst:			:type: int
			.add_property("numVertices", &mod::Rule::ContextGraph::numVertices)
			// rst:		.. py:attribute:: vertices
			// rst:
			// rst:			(Read-only) An iterable of all vertices in the graph.
			// rst:
			// rst:			:type: :py:class:`RuleContextGraphVertexRange`
			.add_property("vertices", &mod::Rule::ContextGraph::vertices)
			// rst:		.. py:attribute:: numEdges
			// rst:
			// rst:			(Read-only) The number of edges in the graph.
			// rst:
			// rst:			:type: int
			.add_property("numEdges", &mod::Rule::ContextGraph::numEdges)
			// rst:		.. py:attribute:: edges
			// rst:
			// rst:			(Read-only) An iterable of all edges in the graph.
			// rst:
			// rst:			:type: :py:class:`RuleContextGraphEdgeRange`
			.add_property("edges", &mod::Rule::ContextGraph::edges)
			;

	// rst: .. py:class:: RuleLeftGraphVertex
	// rst:
	// rst:		A descriptor of either a vertex in a context, or a null vertex.
	// rst:
	py::class_<mod::Rule::ContextGraph::Vertex>("RuleLeftGraphVertex", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst: 
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			// rst:		.. py:method:: isNull()
			// rst:
			// rst:			:returns: whether this is a null descriptor or not.
			// rst:			:rtype: bool
			.def("isNull", &mod::Rule::ContextGraph::Vertex::isNull)
			// rst:		.. py:attribute:: id
			// rst:
			// rst:			(Read-only) The index of the vertex. It will be in the range :math:`[0, numCoreVertices[`.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("id", &mod::Rule::ContextGraph::Vertex::getId)
			// rst:		.. py:attribute:: rule
			// rst:
			// rst:			(Read-only) The rule the vertex belongs to.
			// rst:
			// rst:			:type: :py:class:`Rule`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("rule", &mod::Rule::ContextGraph::Vertex::getRule)
			// rst:		.. py:attribute:: core
			// rst:
			// rst:			(Read-only) The descriptor for this vertex in the core graph.
			// rst:
			// rst:			:type: :py:class:`RuleVertex`
			.add_property("core", &mod::Rule::ContextGraph::Vertex::getCore)
			// rst:		.. py:attribute:: degree
			// rst:
			// rst:			(Read-only) The degree of the vertex.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("degree", &mod::Rule::ContextGraph::Vertex::getDegree)
			// rst:		.. py:attribute:: incidentEdges
			// rst:
			// rst:			(Read-only) A range of incident edges to this vertex.
			// rst:
			// rst:			:type: :py:class:`RuleContextGraphIncidentEdgeRange`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("incidentEdges", &mod::Rule::ContextGraph::Vertex::incidentEdges)
			;

	// rst: .. py:class:: RuleContextGraphEdge
	// rst:
	// rst:		A descriptor of either an edge in a context, or a null edge.
	// rst:
	py::class_<mod::Rule::ContextGraph::Edge>("RuleContextGraphEdge", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst:
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			// rst:		.. py:method:: isNull()
			// rst:
			// rst:			:returns: whether this is a null descriptor or not.
			// rst:			:rtype: bool
			.def("isNull", &mod::Rule::ContextGraph::Edge::isNull)
			// rst:		.. py:attribute:: graph
			// rst:
			// rst:			(Read-only) The rule the edge belongs to.
			// rst:
			// rst:			:type: :py:class:`Rule`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("rule", &mod::Rule::ContextGraph::Edge::getRule)
			// rst:		.. py:attribute:: core
			// rst:
			// rst:			(Read-only) The descriptor for this edge in the core graph.
			// rst:
			// rst:			:type: :py:class:`RuleEdge`
			.add_property("core", &mod::Rule::ContextGraph::Edge::getCore)
			// rst:		.. py:attribute:: source
			// rst:
			// rst:			(Read-only) The source vertex of the edge.
			// rst:
			// rst:			:type: :py:class:`RuleContextGraphVertex`
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("source", &mod::Rule::ContextGraph::Edge::source)
			// rst:		.. attribute:: target
			// rst:
			// rst:			(Read-only) The target vertex of the edge.
			// rst:
			// rst:			:type: :py:class:`RuleContextGraphVertex`
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("target", &mod::Rule::ContextGraph::Edge::target)
			;

	py::class_<mod::Rule::ContextGraph::VertexRange>("RuleContextGraphVertexRange", py::no_init)
			.def("__iter__", py::iterator<mod::Rule::ContextGraph::VertexRange>())

			;
	py::class_<mod::Rule::ContextGraph::EdgeRange>("RuleContextGraphEdgeRange", py::no_init)
			.def("__iter__", py::iterator<mod::Rule::ContextGraph::EdgeRange>())
			;
	py::class_<mod::Rule::ContextGraph::IncidentEdgeRange>("RuleContextGraphIncidentEdgeRange", py::no_init)
			.def("__iter__", py::iterator<mod::Rule::ContextGraph::IncidentEdgeRange>())
			;
}

void exportRight() {
	// rst: .. py:class:: RuleRightGraph
	// rst:
	// rst:		A proxy object representing the right graph of the rule.
	// rst:
	py::class_<mod::Rule::RightGraph>("RuleRightGraph", py::no_init)
			// rst:		.. py:attribute:: rule
			// rst:
			// rst:			(Read-only) The rule the graph belongs to.
			// rst:
			// rst:			:type: :py:class:`Rule`
			.add_property("rule", &mod::Rule::RightGraph::getRule)
			// rst:		.. py:attribute:: numVertices
			// rst:
			// rst:			(Read-only) The number of vertices in the graph.
			// rst:
			// rst:			:type: int
			.add_property("numVertices", &mod::Rule::RightGraph::numVertices)
			// rst:		.. py:attribute:: vertices
			// rst:
			// rst:			(Read-only) An iterable of all vertices in the graph.
			// rst:
			// rst:			:type: :py:class:`RuleRightGraphVertexRange`
			.add_property("vertices", &mod::Rule::RightGraph::vertices)
			// rst:		.. py:attribute:: numEdges
			// rst:
			// rst:			(Read-only) The number of edges in the graph.
			// rst:
			// rst:			:type: int
			.add_property("numEdges", &mod::Rule::RightGraph::numEdges)
			// rst:		.. py:attribute:: edges
			// rst:
			// rst:			(Read-only) An iterable of all edges in the graph.
			// rst:
			// rst:			:type: :py:class:`RuleRightGraphEdgeRange`
			.add_property("edges", &mod::Rule::RightGraph::edges)
			;

	// rst: .. py:class:: RuleRightGraphVertex
	// rst:
	// rst:		A descriptor of either a vertex in a right side, or a null vertex.
	// rst:
	py::class_<mod::Rule::RightGraph::Vertex>("RuleRightGraphVertex", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst: 
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			// rst:		.. py:method:: isNull()
			// rst:
			// rst:			:returns: whether this is a null descriptor or not.
			// rst:			:rtype: bool
			.def("isNull", &mod::Rule::RightGraph::Vertex::isNull)
			// rst:		.. py:attribute:: id
			// rst:
			// rst:			(Read-only) The index of the vertex. It will be in the range :math:`[0, numCoreVertices[`.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("id", &mod::Rule::RightGraph::Vertex::getId)
			// rst:		.. py:attribute:: rule
			// rst:
			// rst:			(Read-only) The rule the vertex belongs to.
			// rst:
			// rst:			:type: :py:class:`Rule`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("rule", &mod::Rule::RightGraph::Vertex::getRule)
			// rst:		.. py:attribute:: core
			// rst:
			// rst:			(Read-only) The descriptor for this vertex in the core graph.
			// rst:
			// rst:			:type: :py:class:`RuleVertex`
			.add_property("core", &mod::Rule::RightGraph::Vertex::getCore)
			// rst:		.. py:attribute:: degree
			// rst:
			// rst:			(Read-only) The degree of the vertex.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("degree", &mod::Rule::RightGraph::Vertex::getDegree)
			// rst:		.. py:attribute:: incidentEdges
			// rst:
			// rst:			(Read-only) A range of incident edges to this vertex.
			// rst:
			// rst:			:type: :py:class:`RuleRightGraphIncidentEdgeRange`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("incidentEdges", &mod::Rule::RightGraph::Vertex::incidentEdges)
			// rst:		.. py:attribute:: stringLabel
			// rst:
			// rst:			(Read-only) The string label of the vertex.
			// rst:
			// rst:			:type: string
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("stringLabel", py::make_function(&mod::Rule::RightGraph::Vertex::getStringLabel, py::return_value_policy<py::copy_const_reference>()))
			// rst:		.. py:attribute:: atomId
			// rst:
			// rst:			(Read-only) The atom id of the vertex.
			// rst:
			// rst:			:type: :py:class:`AtomId`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("atomId", &mod::Rule::RightGraph::Vertex::getAtomId)
			// rst:		.. py:attribute:: charge
			// rst:
			// rst:			(Read-only) The charge of the vertex.
			// rst:
			// rst:			:type: :py:class:`Charge`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("charge", &mod::Rule::RightGraph::Vertex::getCharge)
			// rst:		.. py:attribute:: radical
			// rst:
			// rst:			(Read-only) The radical status of the vertex.
			// rst:
			// rst:			:type: bool
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("radical", &mod::Rule::RightGraph::Vertex::getRadical)
			;

	// rst: .. py:class:: RuleRightGraphEdge
	// rst:
	// rst:		A descriptor of either an edge in a right side, or a null edge.
	// rst:
	py::class_<mod::Rule::RightGraph::Edge>("RuleRightGraphEdge", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst:
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			// rst:		.. py:method:: isNull()
			// rst:
			// rst:			:returns: whether this is a null descriptor or not.
			// rst:			:rtype: bool
			.def("isNull", &mod::Rule::RightGraph::Edge::isNull)
			// rst:		.. py:attribute:: graph
			// rst:
			// rst:			(Read-only) The rule the edge belongs to.
			// rst:
			// rst:			:type: :py:class:`Rule`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("rule", &mod::Rule::RightGraph::Edge::getRule)
			// rst:		.. py:attribute:: core
			// rst:
			// rst:			(Read-only) The descriptor for this edge in the core graph.
			// rst:
			// rst:			:type: :py:class:`RuleEdge`
			.add_property("core", &mod::Rule::RightGraph::Edge::getCore)
			// rst:		.. py:attribute:: source
			// rst:
			// rst:			(Read-only) The source vertex of the edge.
			// rst:
			// rst:			:type: :py:class:`RuleRightGraphVertex`
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("source", &mod::Rule::RightGraph::Edge::source)
			// rst:		.. attribute:: target
			// rst:
			// rst:			(Read-only) The target vertex of the edge.
			// rst:
			// rst:			:type: :py:class:`RuleRightGraphVertex`
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("target", &mod::Rule::RightGraph::Edge::target)
			// rst:		.. py:attribute:: stringLabel
			// rst:
			// rst:			(Read-only) The string label of the edge.
			// rst:
			// rst:			:type: string
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("stringLabel", py::make_function(&mod::Rule::RightGraph::Edge::getStringLabel, py::return_value_policy<py::copy_const_reference>()))
			// rst:		.. py:attribute:: bondType
			// rst:
			// rst:			(Read-only) The bond type of the edge.
			// rst:
			// rst:			:type: :py:class:`BondType`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("bondType", &mod::Rule::RightGraph::Edge::getBondType)
			;

	py::class_<mod::Rule::RightGraph::VertexRange>("RuleRightGraphVertexRange", py::no_init)
			.def("__iter__", py::iterator<mod::Rule::RightGraph::VertexRange>())

			;
	py::class_<mod::Rule::RightGraph::EdgeRange>("RuleRightGraphEdgeRange", py::no_init)
			.def("__iter__", py::iterator<mod::Rule::RightGraph::EdgeRange>())
			;
	py::class_<mod::Rule::RightGraph::IncidentEdgeRange>("RuleRightGraphIncidentEdgeRange", py::no_init)
			.def("__iter__", py::iterator<mod::Rule::RightGraph::IncidentEdgeRange>())
			;
}

void exportCore() {
	// rst: .. py:class:: RuleVertex
	// rst:
	// rst:		A descriptor of either a vertex in a rule, or a null vertex.
	// rst:
	py::class_<mod::Rule::Vertex>("RuleVertex", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst: 
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			// rst:		.. py:method:: isNull()
			// rst:
			// rst:			:returns: whether this is a null descriptor or not.
			// rst:			:rtype: bool
			.def("isNull", &mod::Rule::Vertex::isNull)
			// rst:		.. py:attribute:: id
			// rst:
			// rst:			(Read-only) The index of the vertex. It will be in the range :math:`[0, numVertices[`.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("id", &mod::Rule::Vertex::getId)
			// rst:		.. py:attribute:: rule
			// rst:
			// rst:			(Read-only) The rule the vertex belongs to.
			// rst:
			// rst:			:type: :py:class:`Rule`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("rule", &mod::Rule::Vertex::getRule)
			// rst:		.. py:attribute:: left
			// rst:
			// rst:			(Read-only) A null descriptor if this vertex is not in the left graph,
			// rst:			otherwise the descriptor of this vertex in the left graph.
			.add_property("left", &mod::Rule::Vertex::getLeft)
			// rst:		.. py:attribute:: context
			// rst:
			// rst:			(Read-only) A null descriptor if this vertex is not in the context graph,
			// rst:			otherwise the descriptor of this vertex in the context graph.
			.add_property("context", &mod::Rule::Vertex::getContext)
			// rst:		.. py:attribute:: left
			// rst:
			// rst:			(Read-only) A null descriptor if this vertex is not in the right graph,
			// rst:			otherwise the descriptor of this vertex in the right graph.
			.add_property("right", &mod::Rule::Vertex::getRight)
			// rst:		.. py:attribute:: degree
			// rst:
			// rst:			(Read-only) The degree of the vertex.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("degree", &mod::Rule::Vertex::getDegree)
			// rst:		.. py:attribute:: incidentEdges
			// rst:
			// rst:			(Read-only) A range of incident edges to this vertex.
			// rst:
			// rst:			:type: :py:class:`RuleIncidentEdgeRange`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("incidentEdges", &mod::Rule::Vertex::incidentEdges)
			;

	// rst: .. py:class:: RuleEdge
	// rst:
	// rst:		A descriptor of either an edge in a rule, or a null edge.
	// rst:
	py::class_<mod::Rule::Edge>("RuleEdge", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst:
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			// rst:		.. py:method:: isNull()
			// rst:
			// rst:			:returns: whether this is a null descriptor or not.
			// rst:			:rtype: bool
			.def("isNull", &mod::Rule::Edge::isNull)
			// rst:		.. py:attribute:: rule
			// rst:
			// rst:			(Read-only) The rule the edge belongs to.
			// rst:
			// rst:			:type: :py:class:`Rule`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("rule", &mod::Rule::Edge::getRule)
			// rst:		.. py:attribute:: left
			// rst:
			// rst:			(Read-only) A null descriptor if this edge is not in the left graph,
			// rst:			otherwise the descriptor of this edge in the left graph.
			.add_property("left", &mod::Rule::Edge::getLeft)
			// rst:		.. py:attribute:: context
			// rst:
			// rst:			(Read-only) A null descriptor if this edge is not in the context graph,
			// rst:			otherwise the descriptor of this edge in the context graph.
			.add_property("context", &mod::Rule::Edge::getContext)
			// rst:		.. py:attribute:: left
			// rst:
			// rst:			(Read-only) A null descriptor if this edge is not in the right graph,
			// rst:			otherwise the descriptor of this edge in the right graph.
			.add_property("right", &mod::Rule::Edge::getRight)
			// rst:		.. py:attribute:: source
			// rst:
			// rst:			(Read-only) The source vertex of the edge.
			// rst:
			// rst:			:type: :py:class:`RuleVertex`
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("source", &mod::Rule::Edge::source)
			// rst:		.. attribute:: target
			// rst:
			// rst:			(Read-only) The target vertex of the edge.
			// rst:
			// rst:			:type: :py:class:`RuleVertex`
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("target", &mod::Rule::Edge::target)
			;

	py::class_<mod::Rule::VertexRange>("RuleVertexRange", py::no_init)
			.def("__iter__", py::iterator<mod::Rule::VertexRange>())
			.def("__getitem__", &mod::Rule::VertexRange::operator[])
			;
	py::class_<mod::Rule::EdgeRange>("RuleEdgeRange", py::no_init)
			.def("__iter__", py::iterator<mod::Rule::EdgeRange>())
			;
	py::class_<mod::Rule::IncidentEdgeRange>("RuleIncidentEdgeRange", py::no_init)
			.def("__iter__", py::iterator<mod::Rule::IncidentEdgeRange>())
			;
}

} // namespace

void RuleGraphInterface_doExport() {
	exportLeft();
	exportContext();
	exportRight();
	exportCore();
}

} // namespace Py
} // namespace mod
