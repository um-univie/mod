#include <mod/Py/Common.h>

#include <mod/DGGraphInterface.h>
#include <mod/Graph.h>
#include <mod/GraphPrinter.h>
#include <mod/Rule.h>

namespace mod {
namespace Py {

void DGGraphInterface_doExport() {
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
			.def(py::self < py::self)
			// rst:		.. py:method:: isNull()
			// rst:
			// rst:			:returns: whether this is a null descriptor or not.
			// rst:			:rtype: bool
			.def("isNull", &mod::DG::Vertex::isNull)
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
			.def(py::self < py::self)
			// rst:		.. py:method:: isNull()
			// rst:
			// rst:			:returns: whether this is a null descriptor or not.
			// rst:			:rtype: bool
			.def("isNull", &mod::DG::HyperEdge::isNull)
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
			// rst:		.. py::attribute:: inverse
			// rst:
			// rst:			(Read-only) A descriptor for the inverse hyperedge of this one, if it exists.
			// rst:			Otherwise a null descriptor is returned.
			// rst:
			// rst:			:type: :py:class:`DGHyperEdge`
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.add_property("inverse", &mod::DG::HyperEdge::getInverse)
			// rst: 	.. py:method:: print(printer=GraphPrinter(), matchColour="Melon")
			// rst:
			// rst:			Print the derivations represented by the hyperedge.
			// rst:			All possible Double-Pushout diagrams are printed.
			// rst:
			// rst: 		:param printer: the printer to use for the figures.
			// rst: 		:type printer: :class:`GraphPrinter`
			// rst:			:param matchColour: the TikZ colour to use for the rule and its image in the bottom span.
			// rst:			:type matchColour: string
			// rst:			:raises: :py:class:`LogicError` if it is a null descriptor.
			.def("print", &mod::DG::HyperEdge::print)
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
			.def("__len__", &mod::DG::RuleRange::size)
			;
}

} // namespace Py
} // namespace mod
