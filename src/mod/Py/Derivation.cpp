#include <mod/Py/Common.h>

#include <mod/Derivation.h>

namespace mod {
namespace Py {
namespace {

const mod::Derivation::GraphList &getLeft(const mod::Derivation &d) {
	return d.left;
}

void setLeft(mod::Derivation &d, mod::Derivation::GraphList list) {
	d.left = list;
}

std::shared_ptr<mod::Rule> getRule(const mod::Derivation &d) {
	return d.rule;
}

void setRule(mod::Derivation &d, std::shared_ptr<mod::Rule> r) {
	d.rule = r;
}

const mod::Derivation::GraphList &getRight(const mod::Derivation &d) {
	return d.right;
}

void setRight(mod::Derivation &d, mod::Derivation::GraphList list) {
	d.right = list;
}

} // namespace 

void Derivation_doExport() {
	// rst: .. py:class:: Derivation
	// rst:
	// rst:		This class represents a derivation :math:`G\Rightarrow^p H`, with :math:`G` and :math:`H` being multisets of graphs.
	// rst:
	py::class_<mod::Derivation>("Derivation")
			.def(str(py::self))
			// rst:		.. py:attribute:: left
			// rst:
			// rst:			Represents the multi-set of left graphs :math:`G` in a derivation :math:`G\Rightarrow^p H`.
			// rst:
			// rst:			:type: list of :class:`Graph`
			.add_property("_left", py::make_function(&getLeft, py::return_value_policy<py::copy_const_reference>()), &setLeft)
			// rst:		.. py:attribute:: rule
			// rst:
			// rst:			Represents the transformation rule :math:`p` in a derivation :math:`G\Rightarrow^p H`.
			// rst:
			// rst:			:type: :class:`Rule`
			.add_property("rule", &getRule, &setRule)
			// rst:		.. py:attribute:: right
			// rst:
			// rst:			Represents the multi-set of right graphs :math:`G` in a derivation :math:`G\Rightarrow^p H`.
			// rst:
			// rst:			:type: list of :class:`Graph`
			.add_property("_right", py::make_function(&getRight, py::return_value_policy<py::copy_const_reference>()), &setRight)
			;
}

} // namespace Py
} // namespace mod