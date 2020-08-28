#include <mod/py/Common.hpp>

#include <mod/graph/Printer.hpp>

#include <iomanip>

namespace mod {
namespace graph {
namespace Py {

void Printer_doExport() {
	// rst: .. py:class:: GraphPrinter
	// rst:
	// rst:		This class is used to configure how graphs are visualised.
	// rst:
	// rst:		.. warning:: Some of these options greatly alter how graphs are depicted
	// rst:			and the result may not accurately represent the underlying graph,
	// rst:			and may make non-molecules look like molecules.
	// rst:
	// rst:		.. py:method:: __init__(self)
	// rst:
	// rst:			The default constructor enables edges as bonds, raised charges, and raised isotopes.
	py::class_<Printer>("GraphPrinter")
			.def(py::self == py::self)
			.def(py::self != py::self)
			// rst:		.. py:method:: setMolDefault()
			// rst:
			// rst:			Shortcut for enabling all but thickening and index printing.
			.def("setMolDefault", &Printer::setMolDefault)
			// rst:		.. py:method:: setReactionDefault()
			// rst:
			// rst:			Shortcut for enabling all but thickening, index printing and simplification of carbon atoms.
			.def("setReactionDefault", &Printer::setReactionDefault)
			// rst:		.. py:method:: disableAll()
			// rst:
			// rst:			Disable all special printing features.
			.def("disableAll", &Printer::disableAll)
			// rst:		.. py:method:: enableAll()
			// rst:
			// rst:			Enable all special printing features, except typewriter font.
			.def("enableAll", &Printer::enableAll)
			// rst:		.. py:attribute:: edgesAsBonds
			// rst:
			// rst:			Control whether edges with special labels are drawn as chemical bonds.
			// rst:
			// rst:			:type: bool
			.add_property("edgesAsBonds", &Printer::getEdgesAsBonds, &Printer::setEdgesAsBonds)
			// rst:		.. py:attribute:: collapseHydrogens
			// rst:
			// rst:			Control whether vertices representing hydrogen atoms are collapsed into their neighbours labels.
			// rst:
			// rst:			:type: bool
			.add_property("collapseHydrogens", &Printer::getCollapseHydrogens, &Printer::setCollapseHydrogens)
			// rst:		.. py:attribute:: raiseCharges
			// rst:
			// rst:			Control whether a vertex label suffix encoding a charge is written as a superscript to the rest of the label.
			// rst:
			// rst:			:type: bool
			.add_property("raiseCharges", &Printer::getRaiseCharges, &Printer::setRaiseCharges)
			// rst:		.. py:attribute:: simpleCarbons
			// rst:
			// rst:			Control whether some vertices encoding carbon atoms are depicted without any label.
			// rst:
			// rst:			:type: bool
			.add_property("simpleCarbons", &Printer::getSimpleCarbons, &Printer::setSimpleCarbons)
			// rst:		.. py:attribute:: thick
			// rst:
			// rst:			Control whether all edges are drawn thicker than normal and all labels are written in bold.
			// rst:
			// rst:			:type: bool
			.add_property("thick", &Printer::getThick, &Printer::setThick)
			// rst:		.. py:attribute:: withColour
			// rst:
			// rst:			Control whether colour is applied to certain elements of the graph which are molecule-like.
			// rst:
			// rst:			:type: bool
			.add_property("withColour", &Printer::getWithColour, &Printer::setWithColour)
			// rst:		.. py:attribute:: withIndex
			// rst:
			// rst:			Control whether the underlying indices of the vertices are printed.
			// rst:
			// rst:			:type: bool
			.add_property("withIndex", &Printer::getWithIndex, &Printer::setWithIndex)
			// rst:		.. py:attribute:: withTexttt
			// rst:
			// rst:			Control whether the vertex and edge labels are written with typewriter font.
			// rst:
			// rst:			:type: bool
			.add_property("withTexttt", &Printer::getWithTexttt, &Printer::setWithTexttt)
			// rst:		.. py:attribute:: withRawStereo
			// rst:
			// rst:			Control whether the vertices and edges are annotated with the raw stereo properties.
			// rst:
			// rst:			:type: bool
			.add_property("withRawStereo", &Printer::getWithRawStereo, &Printer::setWithRawStereo)
			// rst:		.. py:attribute:: withPrettyStereo
			// rst:
			// rst:			Control whether the vertices and edges are annotated with stylized stereo properties.
			// rst:
			// rst:			:type: bool
			.add_property("withPrettyStereo", &Printer::getWithPrettyStereo, &Printer::setWithPrettyStereo)
			// rst:		.. py:attribute:: rotation
			// rst:
			// rst:			Rotation of internally computed coordinates.
			// rst:
			// rst:			:type: int
			.add_property("rotation", &Printer::getRotation, &Printer::setRotation)
			// rst:		.. py:attribute:: mirror
			// rst:
			// rst:			Mirror internally computed coordinates in the y-axis.
			// rst:
			// rst:			:type: bool
			.add_property("mirror", &Printer::getMirror, &Printer::setMirror)
			;
}

} // namespace Py
} // namespace graph
} // namespace mod