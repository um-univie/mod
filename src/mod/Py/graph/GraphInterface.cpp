#include <mod/Py/Common.h>

#include <mod/Chem.h>
#include <mod/graph/GraphInterface.h>
#include <mod/graph/Printer.h>

namespace mod {
namespace graph {
namespace Py {

void GraphInterface_doExport() {
	std::string(Graph::Vertex::*printStereoWithoutOptions)() const = &Graph::Vertex::printStereo;
	std::string(Graph::Vertex::*printStereoWithOptions)(const graph::Printer&) const = &Graph::Vertex::printStereo;

	// rst: .. py:class:: GraphVertex
	// rst:
	// rst:		A descriptor of either a vertex in a graph, or a null vertex.
	// rst:
	py::class_<Graph::Vertex>("GraphVertex", py::no_init)
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
			.def("isNull", &Graph::Vertex::isNull)
			// rst:		.. py:attribute:: id
			// rst:
			// rst:			(Read-only) The index of the vertex. It will be in the range :math:`[0, numVertices[`.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("id", &Graph::Vertex::getId)
			// rst:		.. py:attribute:: graph
			// rst:
			// rst:			(Read-only) The graph the vertex belongs to.
			// rst:
			// rst:			:type: :py:class:`Graph`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("graph", &Graph::Vertex::getGraph)
			// rst:		.. py:attribute:: degree
			// rst:
			// rst:			(Read-only) The degree of the vertex.
			// rst:
			// rst:			:type: int
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("degree", &Graph::Vertex::getDegree)
			// rst:		.. py:attribute:: incidentEdges
			// rst:
			// rst:			(Read-only) A range of incident edges to this vertex.
			// rst:
			// rst:			:type: :py:class:`GraphIncidentEdgeRange`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("incidentEdges", &Graph::Vertex::incidentEdges)
			// rst:		.. py:attribute:: stringLabel
			// rst:
			// rst:			(Read-only) The string label of the vertex.
			// rst:
			// rst:			:type: string
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("stringLabel", py::make_function(&Graph::Vertex::getStringLabel, py::return_value_policy<py::copy_const_reference>()))
			// rst:		.. py:attribute:: atomId
			// rst:
			// rst:			(Read-only) The atom id of the vertex.
			// rst:
			// rst:			:type: :py:class:`AtomId`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("atomId", &Graph::Vertex::getAtomId)
			// rst:		.. py:attribute:: charge
			// rst:
			// rst:			(Read-only) The charge of the vertex.
			// rst:
			// rst:			:type: :py:class:`Charge`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("charge", &Graph::Vertex::getCharge)
			// rst:		.. py:attribute:: radical
			// rst:
			// rst:			(Read-only) The radical status of the vertex.
			// rst:
			// rst:			:type: bool
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("radical", &Graph::Vertex::getRadical)
			// rst:		.. py:method:: printStereo()
			// rst:                  printStereo(p)
			// rst:
			// rst:			Print the stereo configuration for the vertex.
			// rst:
			// rst:			:param p: the printing options used for the depiction.
			// rst:			:type p: :class:`GraphPrinter`
			// rst:			:returns: the name of the PDF-file that will be compiled in post-processing.
			// rst:			:rtype: string
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.def("printStereo", printStereoWithoutOptions)
			.def("printStereo", printStereoWithOptions)
			;

	// rst: .. py:class:: GraphEdge
	// rst:
	// rst:		A descriptor of either an edge in a graph, or a null edge.
	// rst:
	py::class_<Graph::Edge>("GraphEdge", py::no_init)
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
			.def("isNull", &Graph::Edge::isNull)
			// rst:		.. py:attribute:: graph
			// rst:
			// rst:			(Read-only) The graph the edge belongs to.
			// rst:
			// rst:			:type: :py:class:`Graph`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("graph", &Graph::Edge::getGraph)
			// rst:		.. py:attribute:: source
			// rst:
			// rst:			(Read-only) The source vertex of the edge.
			// rst:
			// rst:			:type: :py:class:`GraphVertex`
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("source", &Graph::Edge::source)
			// rst:		.. attribute:: target
			// rst:
			// rst:			(Read-only) The target vertex of the edge.
			// rst:
			// rst:			:type: :py:class:`GraphVertex`
			// rst: 		:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("target", &Graph::Edge::target)
			// rst:		.. py:attribute:: stringLabel
			// rst:
			// rst:			(Read-only) The string label of the edge.
			// rst:
			// rst:			:type: string
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("stringLabel", py::make_function(&Graph::Edge::getStringLabel, py::return_value_policy<py::copy_const_reference>()))
			// rst:		.. py:attribute:: bondType
			// rst:
			// rst:			(Read-only) The bond type of the edge.
			// rst:
			// rst:			:type: :py:class:`BondType`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("bondType", &Graph::Edge::getBondType)
			;

	py::class_<Graph::VertexRange>("GraphVertexRange", py::no_init)
			.def("__iter__", py::iterator<Graph::VertexRange>())
			.def("__getitem__", &Graph::VertexRange::operator[])
			;
	py::class_<Graph::EdgeRange>("GraphEdgeRange", py::no_init)
			.def("__iter__", py::iterator<Graph::EdgeRange>())
			;
	py::class_<Graph::IncidentEdgeRange>("GraphIncidentEdgeRange", py::no_init)
			.def("__iter__", py::iterator<Graph::IncidentEdgeRange>())
			;
}

} // namespace Py
} // namespace graph
} // namespace mod
