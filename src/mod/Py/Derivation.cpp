#include <mod/Py/Common.h>

#include <mod/Derivation.h>
#include <mod/Rule.h>

namespace mod {
namespace Py {
namespace {

std::shared_ptr<mod::Rule> getRule(const mod::Derivation &d) {
	return d.rule;
}

void setRule(mod::Derivation &d, std::shared_ptr<mod::Rule> r) {
	d.rule = r;
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
			.def_readwrite("_left", &mod::Derivation::left)
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
			.def_readwrite("_right", &mod::Derivation::right)
			;
}

} // namespace Py
} // namespace mod