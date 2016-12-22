#include <mod/Py/Common.h>

#include <mod/GraphPrinter.h>

#include <iomanip>

namespace mod {
namespace Py {

void GraphPrinter_doExport() {
	// rst: .. py:class:: GraphPrinter
	// rst:
	// rst:		This class is used to configure how graphs are visualised.
	// rst:
	// rst:		.. warning:: Some of these options greatly alter how graphs are depicted
	// rst:			and the result may not accurately represent the underlying graph,
	// rst:			and may make non-molecules look like molecules.
	// rst:
	py::class_<mod::GraphPrinter, boost::noncopyable>("GraphPrinter")
			// rst:		.. py:method:: __init__(self)
			// rst:
			// rst:			The default constructor enables edges as bonds and raised charges.
			// rst:		.. py:method:: setMolDefault()
			// rst:
			// rst:			Shortcut for enabling all but thickening and index printing.
			.def("setMolDefault", &mod::GraphPrinter::setMolDefault)
			// rst:		.. py:method:: setReactionDefault()
			// rst:
			// rst:			Shortcut for enabling all but thickening, index printing and simplification of carbon atoms.
			.def("setReactionDefault", &mod::GraphPrinter::setReactionDefault)
			// rst:		.. py:method:: disableAll()
			// rst:
			// rst:			Disable all special printing features.
			.def("disableAll", &mod::GraphPrinter::disableAll)
			// rst:		.. py:method:: enableAll()
			// rst:
			// rst:			Enable all special printing features, except typewriter font.
			.def("enableAll", &mod::GraphPrinter::enableAll)
			// rst:		.. py:attribute:: edgesAsBonds
			// rst:
			// rst:			Control whether edges with special labels are drawn as chemical bonds.
			// rst:
			// rst:			:type: bool
			.add_property("edgesAsBonds", &mod::GraphPrinter::getEdgesAsBonds, &mod::GraphPrinter::setEdgesAsBonds)
			// rst:		.. py:attribute:: collapseHydrogens
			// rst:
			// rst:			Control whether vertices representing hydrogen atoms are collapsed into their neighbours labels.
			// rst:
			// rst:			:type: bool
			.add_property("collapseHydrogens", &mod::GraphPrinter::getCollapseHydrogens, &mod::GraphPrinter::setCollapseHydrogens)
			// rst:		.. py:attribute:: raiseCharges
			// rst:
			// rst:			Control whether a vertex label suffix encoding a charge is written as a superscript to the rest of the label.
			// rst:
			// rst:			:type: bool
			.add_property("raiseCharges", &mod::GraphPrinter::getRaiseCharges, &mod::GraphPrinter::setRaiseCharges)
			// rst:		.. py:attribute:: simpleCarbons
			// rst:
			// rst:			Control whether some vertices encoding carbon atoms are depicted without any label.
			// rst:
			// rst:			:type: bool
			.add_property("simpleCarbons", &mod::GraphPrinter::getSimpleCarbons, &mod::GraphPrinter::setSimpleCarbons)
			// rst:		.. py:attribute:: thick
			// rst:
			// rst:			Control whether all edges are drawn thicker than normal and all labels are written in bold.
			// rst:
			// rst:			:type: bool
			.add_property("thick", &mod::GraphPrinter::getThick, &mod::GraphPrinter::setThick)
			// rst:		.. py:attribute:: withColour
			// rst:
			// rst:			Control whether colour is applied to certain elements of the graph which are molecule-like.
			// rst:
			// rst:			:type: bool
			.add_property("withColour", &mod::GraphPrinter::getWithColour, &mod::GraphPrinter::setWithColour)
			// rst:		.. py:attribute:: withIndex
			// rst:
			// rst:			Control whether the underlying indices of the vertices are printed.
			// rst:
			// rst:			:type: bool
			.add_property("withIndex", &mod::GraphPrinter::getWithIndex, &mod::GraphPrinter::setWithIndex)
			// rst:		.. py:attribute:: withTexttt
			// rst:
			// rst:			Control whether the vertex and edge labels are written with typewriter font.
			// rst:
			// rst:			:type: bool
			.add_property("withTexttt", &mod::GraphPrinter::getWithTexttt, &mod::GraphPrinter::setWithTexttt)
			// rst:		.. py:attribute:: rotation
			// rst:
			// rst:			Rotation of internally computed coordinates.
			// rst:
			// rst:			:type: int
			.add_property("rotation", &mod::GraphPrinter::getRotation, &mod::GraphPrinter::setRotation)
			;
}

} // namespace Py
} // namespace mod
