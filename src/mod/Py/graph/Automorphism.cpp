#include <mod/Py/Common.h>

#include <mod/graph/Automorphism.h>
#include <mod/graph/GraphInterface.h>

namespace mod {
namespace graph {
namespace Py {

void Automorphism_doExport() {
	// rst: .. py:class:: GraphAut
	// rst:
	// rst:			An automorphism of a graph.
	// rst:
	py::class_<Graph::Aut>("GraphAut", py::no_init)
			// rst:		.. py:method:: __getitem__(v)
			// rst:
			// rst:			:param v: the vertex to permute.
			// rst:			:type v: :py:class:`GraphVertex`
			// rst:			:returns: the image of the given vertex under the permutation.
			// rst:			:rtype: :py:class:`GraphVertex`
			.def("__getitem__", &Graph::Aut::operator[])
			.def(str(py::self))
			;

	// rst: .. py:class:: GraphAutGroup
	// rst:
	// rst:		A class representing the automorphism group of a :class:`Graph`.
	// rst:
	py::class_<Graph::AutGroup>("GraphAutGroup", py::no_init)
			// rst:		.. attribute:: gens
			// rst:
			// rst:			(Read-only) A range of the generators for the group.
			// rst:
			// rst:			:type: :py:class:`GraphAutGroupGensRange`
			.add_property("gens", &Graph::AutGroup::gens)
			.def(str(py::self))
			;

	// rst: .. py:class:: GraphAutGroupGensRange
	// rst:
	// rst:			A random-access range of :py:class:`GraphAut`.
	py::class_<Graph::AutGroup::Gens>("GraphAutGroupGensRange", py::no_init)
			.def("__iter__", py::iterator<Graph::AutGroup::Gens>())
			.def("__getitem__", &Graph::AutGroup::Gens::operator[])
			.def("__len__", &Graph::AutGroup::Gens::size)
			;
}

} // namespace Py
} // namespace graph
} // namespace mod