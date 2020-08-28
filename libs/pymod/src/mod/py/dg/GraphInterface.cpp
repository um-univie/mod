#include <mod/py/Common.hpp>

#include <mod/dg/GraphInterface.hpp>
#include <mod/graph/Graph.hpp>
#include <mod/graph/Printer.hpp>
#include <mod/rule/Rule.hpp>

#include <iostream>

namespace mod {
namespace dg {
namespace Py {

void GraphInterface_doExport() {
	// rst: .. py:class:: DGVertex
	// rst:
	// rst:		A descriptor of either a vertex in a derivation graph, or a null vertex.
	// rst:
	py::class_<DG::Vertex>("DGVertex", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst: 
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			.def("__hash__", &DG::Vertex::hash)
					// rst:		.. py::method:: __bool__(self)
					// rst:
					// rst:			:returns: ``not isNull()``
					// rst:			:rtype: bool
			.def("__bool__", &DG::Vertex::operator bool)
					// rst:		.. py:method:: isNull()
					// rst:
					// rst:			:returns: whether this is a null descriptor or not.
					// rst:			:rtype: bool
			.def("isNull", &DG::Vertex::isNull)
					// rst:		.. py:attribute:: id
					// rst:
					// rst:			(Read-only) The index of the vertex. It will be in the range :math:`[0, numVertices + numEdges[`.
					// rst:
					// rst:			:type: int
					// rst:			:raises: :class:`LogicError` if it is a null descriptor.
			.add_property("id", &DG::Vertex::getId)
					// rst:		.. py:attribute:: dg
					// rst:
					// rst:			(Read-only) The derivation graph the vertex belongs to.
					// rst:
					// rst:			:type: DG
					// rst:			:raises: :class:`LogicError` if it is a null descriptor.
			.add_property("dg", &DG::Vertex::getDG)
					// rst:		.. py:attribute:: inDegree
					// rst:
					// rst:			(Read-only) The in-degree of the vertex, including multiplicity of target multisets.
					// rst:
					// rst:			:type: int
					// rst:			:raises: :class:`LogicError` if it is a null descriptor.
			.add_property("inDegree", &DG::Vertex::inDegree)
					// rst:		.. py:attribute:: inEdges
					// rst:
					// rst:			(Read-only) A range of in-hyperedges for this vertex.
					// rst:
					// rst:			:type: DGInEdgeRange
					// rst:			:raises: :class:`LogicError` if it is a null descriptor.
			.add_property("inEdges", &DG::Vertex::inEdges)
					// rst:		.. py:attribute:: outDegree
					// rst:
					// rst:			(Read-only) The out-degree of the vertex, including multiplicity of source multisets.
					// rst:
					// rst:			:type: int
					// rst:			:raises: :class:`LogicError` if it is a null descriptor.
			.add_property("outDegree", &DG::Vertex::outDegree)
					// rst:		.. py:attribute:: outEdges
					// rst:
					// rst:			(Read-only) A range of out-hyperedges for this vertex.
					// rst:
					// rst:			:type: DGOutEdgeRange
					// rst:			:raises: :class:`LogicError` if it is a null descriptor.
			.add_property("outEdges", &DG::Vertex::outEdges)
					// rst:		.. py:attribute:: graph
					// rst:
					// rst:			(Read-only) The graph label of the vertex.
					// rst:
					// rst:			:type: Graph
					// rst:			:raises: :class:`LogicError` if it is a null descriptor.
			.add_property("graph", &DG::Vertex::getGraph);

	// rst: .. py:class:: DGHyperEdge
	// rst:
	// rst:		A descriptor of either a hyperedge in a derivation graph, or a null edge.
	// rst:
	py::class_<DG::HyperEdge>("DGHyperEdge", py::no_init)
			// rst:		.. py:function:: __init__(self)
			// rst:
			// rst:			Constructs a null descriptor.
			.def(py::init<>())
			.def(str(py::self))
			.def(py::self == py::self)
			.def(py::self != py::self)
			.def(py::self < py::self)
			.def("__hash__", &DG::HyperEdge::hash)
					// rst:		.. py::method:: __bool__(self)
					// rst:
					// rst:			:returns: ``not isNull()``
					// rst:			:rtype: bool
			.def("__bool__", &DG::HyperEdge::operator bool)
					// rst:		.. py:method:: isNull()
					// rst:
					// rst:			:returns: whether this is a null descriptor or not.
					// rst:			:rtype: bool
			.def("isNull", &DG::HyperEdge::isNull)
					// rst:		.. py:attribute:: id
					// rst:
					// rst:			(Read-only) The index of the hyperedge. It will be in the range :math:`[0, numVertices + numEdges[`.
					// rst:
					// rst:			:type: int
					// rst:			:raises: :class:`LogicError` if it is a null descriptor.
			.add_property("id", &DG::HyperEdge::getId)
					// rst:		.. py:attribute:: dg
					// rst:
					// rst:			(Read-only) The derivation graph the edge belongs to.
					// rst:
					// rst:			:type: DG
					// rst:			:raises: :class:`LogicError` if it is a null descriptor.
			.add_property("dg", &DG::HyperEdge::getDG)
					// rst:		.. py:attribute:: numSources
					// rst:
					// rst:			(Read-only) The number of sources of the hyperedge.
					// rst:
					// rst:			:type: int
			.add_property("numSources", &DG::HyperEdge::numSources)
					// rst:		.. py:attribute:: sources
					// rst:
					// rst:			(Read-only) The sources of the hyperedge.
					// rst:
					// rst:			:type: DGSourceRange
					// rst: 		:raises: :class:`LogicError` if it is a null descriptor.
			.add_property("sources", &DG::HyperEdge::sources)
					// rst:		.. py:attribute:: numTargets
					// rst:
					// rst:			(Read-only) The number of targerts of the hyperedge.
					// rst:
					// rst:			:type: int
			.add_property("numTargets", &DG::HyperEdge::numTargets)
					// rst:		.. attribute:: targets
					// rst:
					// rst:			(Read-only) The targets of the hyperedge.
					// rst:
					// rst:			:type: DGTargetRange
					// rst: 		:raises: :class:`LogicError` if it is a null descriptor.
			.add_property("targets", &DG::HyperEdge::targets)
					// rst:		.. py:attribute:: rules
					// rst:
					// rst:			(Read-only) The rules associated with the hyperedge.
					// rst:
					// rst:			:type: DGRuleRange
					// rst:			:raises: :class:`LogicError` if it is a null descriptor.
			.add_property("rules", &DG::HyperEdge::rules)
					// rst:		.. py:attribute:: inverse
					// rst:
					// rst:			(Read-only) A descriptor for the inverse hyperedge of this one, if it exists.
					// rst:			Otherwise a null descriptor is returned.
					// rst:
					// rst:			:type: DGHyperEdge
					// rst:			:raises: :class:`LogicError` if it is a null descriptor.
					// rst:			:raises: :class:`LogicError` if not ``dg.locked``.
			.add_property("inverse", &DG::HyperEdge::getInverse)
					// rst:		.. py:method:: print(printer=GraphPrinter(), nomatchColour="gray", matchColour="")
					// rst:
					// rst:			Print the derivations represented by the hyperedge.
					// rst:			All possible Double-Pushout diagrams are printed.
					// rst:
					// rst:			:param GraphPrinter printer: the printer to use for the figures.
					// rst:			:param str matchColour: the TikZ colour to use for the rule and its image in the bottom span.
					// rst:			:returns: A list with file data for each DPO diagram printed.
					// rst:				Each element is a pair of filename prefixes, where the first entry is completed by appending ``_derL``, ``_derK``, or ``_derR``.
					// rst:				The second entry is completed similarly by appending ``_derG``, ``_derD``, or ``_derH``.
					// rst:			:rtype: list[tuple[str, str]]
					// rst:			:raises: :class:`LogicError` if it is a null descriptor.
					// rst:			:raises: :class:`LogicError` if it has no rules.
			.def("print", &DG::HyperEdge::print, (
					py::arg("printer") = graph::Printer(),
					py::arg("nomatchColour") = "gray",
					py::arg("matchColour") = "")
			);

	py::class_<DG::VertexRange>("DGVertexRange", py::no_init)
			.def("__iter__", py::iterator<DG::VertexRange>());
	py::class_<DG::EdgeRange>("DGEdgeRange", py::no_init)
			.def("__iter__", py::iterator<DG::EdgeRange>());
	py::class_<DG::InEdgeRange>("DGInEdgeRange", py::no_init)
			.def("__iter__", py::iterator<DG::InEdgeRange>());
	py::class_<DG::OutEdgeRange>("DGOutEdgeRange", py::no_init)
			.def("__iter__", py::iterator<DG::OutEdgeRange>());
	py::class_<DG::SourceRange>("DGSourceRange", py::no_init)
			.def("__iter__", py::iterator<DG::SourceRange>());
	py::class_<DG::TargetRange>("DGTargetRange", py::no_init)
			.def("__iter__", py::iterator<DG::TargetRange>());
	py::class_<DG::RuleRange>("DGRuleRange", py::no_init)
			.def("__iter__", py::iterator<DG::RuleRange>())
			.def("__len__", &DG::RuleRange::size);
}

} // namespace Py
} // namespace dg
} // namespace mod