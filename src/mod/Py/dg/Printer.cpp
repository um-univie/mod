#include <mod/Py/Common.h>

#include <mod/Function.h>
#include <mod/dg/DG.h>
#include <mod/dg/GraphInterface.h>
#include <mod/dg/Printer.h>
#include <mod/graph/Printer.h>

namespace mod {
namespace dg {
namespace Py {
namespace {

void Printer_pushVertexVisible(Printer &printer, std::shared_ptr<mod::Function<bool(std::shared_ptr<graph::Graph>, std::shared_ptr<DG>)> > f) {
	printer.pushVertexVisible(mod::toStdFunction(f));
}

void Printer_pushEdgeVisible(Printer &printer, std::shared_ptr<mod::Function<bool(DG::HyperEdge)> > f) {
	printer.pushEdgeVisible(mod::toStdFunction(f));
}

void Printer_pushVertexLabel(Printer &printer, std::shared_ptr<mod::Function<std::string(std::shared_ptr<graph::Graph>, std::shared_ptr<DG>)> > f) {
	printer.pushVertexLabel(mod::toStdFunction(f));
}

void Printer_pushEdgeLabel(Printer &printer, std::shared_ptr<mod::Function<std::string(DG::HyperEdge)> > f) {
	printer.pushEdgeLabel(mod::toStdFunction(f));
}

void Printer_pushVertexColour(Printer &printer, std::shared_ptr<mod::Function<std::string(std::shared_ptr<graph::Graph>, std::shared_ptr<DG>)> > f, bool extendToEdges) {
	printer.pushVertexColour(mod::toStdFunction(f), extendToEdges);
}

void Printer_pushEdgeColour(Printer &printer, std::shared_ptr<mod::Function<std::string(DG::HyperEdge)> > f) {
	printer.pushEdgeColour(mod::toStdFunction(f));
}

} // namespace 

void Printer_doExport() {
	// rst: .. py:class:: DGPrintData
	// rst:
	// rst:		This class is used to hold extra data about how a specific derivation graph
	// rst:		is visualised.
	// rst:
	py::class_<PrintData>("DGPrintData", py::no_init)
			// rst:		.. py:method:: __init__(self, dg)
			// rst:
			// rst:			:param dg: the derivation graph to hold data for.
			// rst:			:type dg: :class:`DG`
			.def(py::init<std::shared_ptr<DG> >())
			// rst:		.. py:attribute:: dg
			// rst:
			// rst:			(Read-only) The derivation graph the object holds data for.
			.add_property("dg", &PrintData::getDG)
			// rst:		.. py:method:: makeDuplicate(e, eDup)
			// rst:
			// rst:			Create another version of the given derivation and give it the given duplicate number.
			// rst:
			// rst:			:param e: a reference to the derivation to duplicate.
			// rst:			:type e: :class:`DGHyperEdge`
			// rst:			:param eDup: the duplicate number for the new version of the derivation.
			// rst:			:type eDup: unsigned int
			.def("makeDuplicate", &PrintData::makeDuplicate)
			// rst:		.. py:method:: removeDuplicate(e, eDup)
			// rst:
			// rst:			Remove the version of the given derivation with the given duplicate number.
			// rst:
			// rst:			:param e: a reference to the derivation to duplicate.
			// rst:			:type e: :class:`DGHyperEdge`
			// rst:			:param eDup: the duplicate number for the version of the derivation to remove.
			// rst:			:type eDup: unsigned int
			.def("removeDuplicate", &PrintData::removeDuplicate)
			// rst:		.. py:method:: reconnectTail(e, eDup, g, vDupTar)
			// rst:
			// rst:			Reconnect an arbitrary version of the tail specified by the given graph in the derivation duplicate given.
			// rst:
			// rst:			:param e: a reference to the derivation to reconnect.
			// rst:			:type e: :class:`DGHyperEdge`
			// rst:			:param eDup: the duplicate number of the derivation to reconnect.
			// rst:			:type eDup: unsigned int
			// rst:			:param g: a graph representing the tail vertex to reconnect.
			// rst:			:type g: :class:`Graph`
			// rst:			:param vDupTar: the new duplicate number for the tail vertex.
			// rst:			:type vDupTar: unsigned int
			.def("reconnectTail", &PrintData::reconnectTail)
			// rst:		.. py:method:: reconnectHead(e, eDup, g, vDupTar)
			// rst:
			// rst:			Reconnect an arbitrary version of the head specified by the given graph in the derivation duplicate given.
			// rst:
			// rst:			:param e: a reference to the derivation to reconnect.
			// rst:			:type e: :class:`DGHyperEdge`
			// rst:			:param eDup: the duplicate number of the derivation to reconnect.
			// rst:			:type eDup: unsigned int
			// rst:			:param g: a graph representing the head vertex to reconnect.
			// rst:			:type g: :class:`Graph`
			// rst:			:param vDupTar: the new duplicate number for the tail vertex.
			// rst:			:type vDupTar: unsigned int
			.def("reconnectHead", &PrintData::reconnectHead)
			;

	graph::Printer & (Printer::*Printer_getGraphPrinter)() = &Printer::getGraphPrinter;
	// rst: .. py:class:: DGPrinter
	// rst:
	// rst:		This class is used to configure how derivation graphs are visualised,
	// rst:		how much is visualised and which extra properties are printed.
	// rst:
	py::class_<Printer, boost::noncopyable>("DGPrinter")
			// rst:		.. py:attribute:: graphPrinter
			// rst:
			// rst:			(Read-only) Retrieve the :class:`GraphPrinter` used when printing images of graphs.
			// rst:
			// rst:			:type: :class:`GraphPrinter`
			.add_property("graphPrinter", py::make_function(Printer_getGraphPrinter, py::return_internal_reference<1>()))
			// rst:		.. py:attribute:: withShortcutEdges
			// rst:
			// rst:			Control whether or not hyperedges :math:`(T, H)` with :math:`|T| = |H| = 1`
			// rst:			are printed as a simple arrow without the usual rectangle.
			// rst:
			// rst:			:type: bool
			.add_property("withShortcutEdges", &Printer::getWithShortcutEdges, &Printer::setWithShortcutEdges)
			// rst:		.. py:attribute:: withGraphImages
			// rst:
			// rst:			Control whether or not each vertex is printed with a image of its graph in it.
			// rst:
			// rst:			:type: bool
			.add_property("withGraphImages", &Printer::getWithGraphImages, &Printer::setWithGraphImages)
			// rst:		.. py:attribute:: labelsAsLatexMath
			// rst:
			// rst:			Control whether or not labels on vertices and hyperedges are put inline :math:`\LaTeX` math.
			// rst:			Specifically, a label ``label`` is written as ``$\mathrm{label'}$``, with ``label'`` being
			// rst:			``label`` with all space characters escaped.
			// rst:
			// rst:			:type: bool
			.add_property("labelsAsLatexMath", &Printer::getLabelsAsLatexMath, &Printer::setLabelsAsLatexMath)
			// rst:		.. py:method:: pushVertexVisible(f)
			// rst:
			// rst:			Add another function controlling the visibility of vertices.
			// rst:			All visibility functions must return ``true`` for a vertex to be visible.
			// rst:
			// rst:			:param f: the function to push for specifying vertex visibility.
			// rst:			:type f: bool(:class:`Graph`, :class:`DG`)
			.def("pushVertexVisible", &Printer_pushVertexVisible)
			// rst:		.. py:method:: popVertexVisible()
			// rst:
			// rst:			Remove the last pushed vertex visibility function.
			.def("popVertexVisible", &Printer::popVertexVisible)
			// rst:		.. py:method:: pushEdgeVisible(f)
			// rst:
			// rst:			Add another function controlling the visibility of hyperedges.
			// rst:			All visibility functions must return ``true`` for a hyperedge to be visible.
			// rst:
			// rst:			:param f: the function to push for specifying hyperedge visibility.
			// rst:			:type f: bool(:class:`DGHyperEdge`)
			.def("pushEdgeVisible", &Printer_pushEdgeVisible)
			// rst:		.. py:method:: popEdgeVisible()
			// rst:
			// rst:			Remove the last pushed hyperedge visibility function.
			.def("popEdgeVisible", &Printer::popEdgeVisible)
			// rst:		.. py:attribute:: withShortcutEdgesAfterVisibility
			// rst:
			// rst:			Control whether or not hyperedges are depicted as simple arrows when all but 1 head and tail are hidden.
			// rst:
			// rst:			:type: bool
			.add_property("withShortcutEdgesAfterVisibility", &Printer::getWithShortcutEdgesAfterVisibility, &Printer::setWithShortcutEdgesAfterVisibility)
			// rst:		.. py:attribute:: vertexLabelSep
			// rst:
			// rst:			The string used as separator between each part of each vertex label.
			// rst:	
			// rst:			:type: string
			.add_property("vertexLabelSep",
			py::make_function(&Printer::getVertexLabelSep, py::return_value_policy<py::copy_const_reference>()),
			&Printer::setVertexLabelSep)
			// rst:		.. py:attribute:: edgeLabelSep
			// rst:
			// rst:			The string used as separator between each part of each edge label.
			// rst:
			// rst:			:type: string
			.add_property("edgeLabelSep",
			py::make_function(&Printer::getEdgeLabelSep, py::return_value_policy<py::copy_const_reference>()),
			&Printer::setEdgeLabelSep)
			// rst:		.. py:method:: pushVertexLabel(f)
			// rst:
			// rst:			Add another function for vertex labelling. The result of this function is added to the end of each label.
			// rst:
			// rst:			:param f: the function to push for labelling vertices.
			// rst:			:type f: string(:class:`Graph`, :class:`DG`)
			.def("pushVertexLabel", &Printer_pushVertexLabel)
			// rst:		.. py:method:: popVertexLabel()
			// rst:
			// rst:			Remove the last pushed vertex labelling function.
			.def("popVertexLabel", &Printer::popVertexLabel)
			// rst:		.. py:method:: pushEdgeLabel(f)
			// rst:
			// rst:			Add another function for edge labelling. The result of this function is appended to each label.
			// rst:
			// rst:			:param f: the function to push for labelling edges.
			// rst:			:type f: string(:class:`DGHyperEdge`)
			.def("pushEdgeLabel", &Printer_pushEdgeLabel)
			// rst:		.. py:method:: popEdgeLabel()
			// rst:
			// rst:			Remove the last pushed edge labelling function.
			.def("popEdgeLabel", &Printer::popEdgeLabel)
			// rst:		.. py:attribute:: withGraphName
			// rst:
			// rst:			Control whether or not graph names are appended to the vertex labels (see also :meth:`DGPrinter.pushVertexLabel`).
			// rst:
			// rst:			:type: bool
			.add_property("withGraphName", &Printer::getWithGraphName, &Printer::setWithGraphName)
			// rst:		.. py:attribute:: withRuleName
			// rst:
			// rst:			Control whether or not rule names are appended to the edge labels (see also :meth:`DGPrinter.pushEdgeLabel`).
			// rst:
			// rst:			:type: bool
			.add_property("withRuleName", &Printer::getWithRuleName, &Printer::setWithRuleName)
			// rst:		.. py:attribute:: withRuleId
			// rst:
			// rst:			Control whether or not rule IDs are appended to the edge labels (see also :meth:`DGPrinter.pushEdgeLabel`).
			// rst:
			// rst:			:type: bool
			.add_property("withRuleId", &Printer::getWithRuleId, &Printer::setWithRuleId)
			// rst:		.. py:attribute:: withInlineGraphs
			// rst:
			// rst:			Control whether or not graph depictions should be precompiled, or be included inline in the DG figure.
			// rst:
			// rst:			:type: bool
			.add_property("withInlineGraphs", &Printer::getWithInlineGraphs, &Printer::setWithInlineGraphs)
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
			.def("pushVertexColour", &Printer_pushVertexColour)
			// rst:		.. py:method:: popVertexColour()
			// rst:
			// rst:			Remove the last pushed vertex colouring function.
			.def("popVertexColour", &Printer::popVertexColour)
			// rst:		.. py:method:: pushEdgeColour(f)
			// rst:
			// rst:			Add another function for colouring hyperedges. The final colour of a hyperedge (and all of its head and tail connectors) is the result of the
			// rst:			first colour function returning a non-empty string.
			// rst:			
			// rst:			:param f: the function to push for colouring hyperedges.
			// rst:			:type f: string(:class:`DGHyperEdge`)
			.def("pushEdgeColour", &Printer_pushEdgeColour)
			// rst:		.. py:method:: popEdgeColour()
			// rst:
			// rst:			Remove the last pushed hyperedge colouring function.
			.def("popEdgeColour", &Printer::popEdgeColour)
			;
}

} // namespace Py
} // namespace dg
} // namespace mod