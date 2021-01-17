#include <mod/py/Common.hpp>

#include <mod/graph/Automorphism.hpp>
#include <mod/graph/GraphInterface.hpp>

namespace mod::graph::Py {

void Automorphism_doExport() {
	py::object graphObj = py::scope().attr("Graph");
	py::scope graphScope = graphObj;

	// rst: .. class:: Graph.Aut
	// rst:
	// rst:			An automorphism of a graph.
	// rst:
	py::class_<Graph::Aut>("Aut", py::no_init)
			// rst:		.. method:: __getitem__(v)
			// rst:
			// rst:			:param Graph.Vertex v: the vertex to permute.
			// rst:			:returns: the image of the given vertex under the permutation.
			// rst:			:rtype: Graph.Vertex
			.def("__getitem__", &Graph::Aut::operator[])
			.def(str(py::self));

	// rst: .. class:: Graph.AutGroup
	// rst:
	// rst:		A class representing the automorphism group of a :class:`Graph`.
	// rst:
	py::class_<Graph::AutGroup>("AutGroup", py::no_init)
			// rst:		.. attribute:: gens
			// rst:
			// rst:			(Read-only) A range of the generators for the group.
			// rst:
			// rst:			:type: AutGroupGensRange
			.add_property("gens", &Graph::AutGroup::gens)
			.def(str(py::self));

	// rst: .. class:: Graph.AutGroupGensRange
	// rst:
	// rst:			A random-access range of :class:`Graph.Aut`.
	py::class_<Graph::AutGroup::Gens>("Graph.AutGroupGensRange", py::no_init)
			.def("__iter__", py::iterator<Graph::AutGroup::Gens>())
			.def("__getitem__", &Graph::AutGroup::Gens::operator[])
			.def("__len__", &Graph::AutGroup::Gens::size);
}

} // namespace mod::graph::Py