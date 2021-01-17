#include <mod/py/Common.hpp>

#include <mod/Chem.hpp>
#include <mod/graph/GraphInterface.hpp>
#include <mod/graph/Printer.hpp>

namespace mod::graph::Py {

void GraphInterface_doExport() {
	std::string(Graph::Vertex::*
	printStereoWithoutOptions)() const = &Graph::Vertex::printStereo;
	std::string(Graph::Vertex::*
	printStereoWithOptions)(const graph::Printer&) const = &Graph::Vertex::printStereo;

	py::object graphObj = py::scope().attr("Graph");
	py::scope graphScope = graphObj;

	// rst: The :class:`Graph` class implements the :class:`protocols.LabelledGraph` protocol,
	// rst: and thus have the following nested types.
	// rst:
	// rst: .. class:: Graph.Vertex
	// rst:
	// rst:		Implements the :class:`protocols.LabelledGraph.Vertex` protocol.
	// rst:		Additionally, the :attr:`id <protocols.Graph.Vertex.id>` is in the range :math:`[0, numVertices[`.
	// rst:
	py::class_<Graph::Vertex>("Vertex", py::no_init)
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			.def("__hash__", &Graph::Vertex::hash)
			.def("__bool__", &Graph::Vertex::operator bool)
			.def("isNull", &Graph::Vertex::isNull)
			.add_property("id", &Graph::Vertex::getId)
			.add_property("graph", &Graph::Vertex::getGraph)
			.add_property("degree", &Graph::Vertex::getDegree)
			.add_property("incidentEdges", &Graph::Vertex::incidentEdges)
			.add_property("stringLabel", py::make_function(&Graph::Vertex::getStringLabel,
			                                               py::return_value_policy<py::copy_const_reference>()))
			.add_property("atomId", &Graph::Vertex::getAtomId)
			.add_property("isotope", &Graph::Vertex::getIsotope)
			.add_property("charge", &Graph::Vertex::getCharge)
			.add_property("radical", &Graph::Vertex::getRadical)
			.def("printStereo", printStereoWithoutOptions)
			.def("printStereo", printStereoWithOptions);

	// rst: .. class:: Graph.Edge
	// rst:
	// rst:		Implements the :class:`protocols.LabelledGraph.Edge` protocol.
	// rst:
	py::class_<Graph::Edge>("Edge", py::no_init)
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			.def("__bool__", &Graph::Edge::operator bool)
			.def("isNull", &Graph::Edge::isNull)
			.add_property("graph", &Graph::Edge::getGraph)
			.add_property("source", &Graph::Edge::source)
			.add_property("target", &Graph::Edge::target)
			.add_property("stringLabel", py::make_function(&Graph::Edge::getStringLabel,
			                                               py::return_value_policy<py::copy_const_reference>()))
			.add_property("bondType", &Graph::Edge::getBondType);

	// rst: .. class:: Graph.VertexRange
	// rst:
	// rst:		Implements the :class:`protocols.Graph.VertexRange` protocol,
	// rst:		in addition to the following functionality.
	// rst:
	py::class_<Graph::VertexRange>("VertexRange", py::no_init)
			.def("__iter__", py::iterator<Graph::VertexRange>())
			// rst:		.. method:: __getitem__(i)
			// rst:
			// rst:			:returns: the ``i``\ th vertex of the graph.
			// rst:			:rtype: Graph.Vertex
			.def("__getitem__", &Graph::VertexRange::operator[]);
	// rst: .. class:: Graph.EdgeRange
	// rst:
	// rst:		Implements the :class:`protocols.Graph.EdgeRange` protocol.
	// rst:
	py::class_<Graph::EdgeRange>("EdgeRange", py::no_init)
			.def("__iter__", py::iterator<Graph::EdgeRange>());
	// rst: .. class:: Graph.IncidentEdgeRange
	// rst:
	// rst:		Implements the :class:`protocols.Graph.IncidentEdgeRange` protocol.
	// rst:
	py::class_<Graph::IncidentEdgeRange>("IncidentEdgeRange", py::no_init)
			.def("__iter__", py::iterator<Graph::IncidentEdgeRange>());
}

} // namespace mod::graph::Py
