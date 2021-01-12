#include <mod/py/Common.hpp>

#include <mod/Derivation.hpp>
#include <mod/rule/Rule.hpp>

namespace mod::Py {

void Derivation_doExport() {
	// rst: .. class:: Derivation
	// rst:
	// rst:		This class is a simple wrapper for passing data around.
	// rst:		It contains two multisets of graphs :math:`G` and :math:`H`,
	// rst:		and optionally a rule :math:`p`.
	// rst:		An object may thus implicitly store a set of direct derivations
	// rst:		:math:`G\Rightarrow^p H`, though the validity of the data is not checked.
	// rst:
	// rst:		An object of this class is implicitly convertible to a :class:`Derivations` object.
	// rst:
	py::class_<mod::Derivation>("Derivation")
			.def(str(py::self))
					// rst:		.. attribute:: left
					// rst:
					// rst:			Represents the multi-set of left graphs :math:`G`.
					// rst:
					// rst:			:type: list[Graph]
			.def_readwrite("left", &mod::Derivation::left)
					// rst:		.. attribute:: rule
					// rst:
					// rst:			Represents the transformation rule :math:`p`, or no rule at all.
					// rst:
					// rst:			:type: Rule
			.def_readwrite("rule", &mod::Derivation::r)
					// rst:		.. attribute:: right
					// rst:
					// rst:			Represents the multi-set of right graphs :math:`H`.
					// rst:
					// rst:			:type: list[Graph]
			.def_readwrite("right", &mod::Derivation::right);

	// rst: .. class:: Derivations
	// rst:
	// rst:		This class is a simple wrapper for passing data around.
	// rst:		It contains two multisets of graphs :math:`G` and :math:`H`,
	// rst:		and (possibly empty) set of rules.
	// rst:		An object may thus implicitly store a set of direct derivations
	// rst:		:math:`G\Rightarrow^p H` for each rule :math:`p`,
	// rst:		though the validity of the data is not checked.
	// rst:
	py::class_<mod::Derivations>("Derivations")
	      .def(py::init<Derivation>())
			.def(str(py::self))
					// rst:		.. attribute:: left
					// rst:
					// rst:			Represents the multi-set of left graphs :math:`G`.
					// rst:
					// rst:			:type: list[Graph]
			.def_readwrite("left", &mod::Derivations::left)
					// rst:		.. attribute:: rules
					// rst:
					// rst:			Represents a (possibly empty) set of transformation rules.
					// rst:
					// rst:			:type: list[Rule]
			.def_readwrite("rules", &mod::Derivations::rules)
					// rst:		.. attribute:: right
					// rst:
					// rst:			Represents the multi-set of right graphs :math:`H`.
					// rst:
					// rst:			:type: list[Graph]
			.def_readwrite("right", &mod::Derivations::right);

	py::implicitly_convertible<Derivation, Derivations>();
}

} // namespace mod::Py