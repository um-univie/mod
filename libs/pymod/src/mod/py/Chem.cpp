#include <mod/py/Common.hpp>

#include <mod/Chem.hpp>

#include <iostream>

namespace mod {
namespace Py {
namespace {

std::string bondTypeToString(mod::BondType b) {
	std::stringstream ss;
	ss << b;
	return ss.str();
}

} // namespace

void Chem_doExport() {
	// rst: .. py:class:: AtomId
	// rst:
	// rst:		Representation of the chemical element of an atom.
	// rst:
	py::class_<mod::AtomId>("AtomId", py::no_init)
			// rst:		.. py:method:: __init__(self)
			// rst:
			// rst:			Construct an :py:const:`AtomIds.Invalid` atom id.
			.def(py::init<>())
			// rst:		.. py:method:: __init__(self, id)
			// rst:
			// rst:			Construct a specific atom id. Pre-condition: the id must be at most that of :py:const:`AtomIds.Max`.
			.def(py::init<unsigned char>())
			// rst:		.. py:attribute:: symbol
			// rst:
			// rst:		    (Read-only) The symbol represented by the atom id.
			// rst:
			// rst:			:type: string
			// rst:			:raises: :py:class:`LogicError` if the id is invalid.
			.add_property("symbol", &mod::AtomId::symbol)
			// rst: 	.. py:method:: __int__(self)
			// rst:
			// rst: 		Implicit conversion to an integer type.
			.def(int_(py::self))
			.def(str(py::self))
			.def(py::self == py::self)
			;

	// rst: .. py:class:: Isotope
	// rst:
	// rst:		Representation of the isotope of an atom.
	// rst:
	py::class_<mod::Isotope>("Isotope", py::no_init)
			// rst:		.. py:method:: __init__(self)
			// rst:
			// rst:			Construct a representation of the most abundant isotope.
			// rst:		
			// rst:			.. note:: This is different from explicitly specifying the isotope that is the most abundant one.
			.def(py::init<>())
			// rst:		.. py:method:: __init__(self, i)
			// rst:
			// rst:			Construct a specific isotope. Pre-condition: the isotope must either be at least 1 or be -1.
			// rst:			Passing -1 is equivalent to default-construction.
			.def(py::init<int>())
			// rst:		.. py:method:: __init__(self)
			// rst:
			// rst:			Implicit conversion to an integer type.
			.def(int_(py::self))
			.def(str(py::self))
			.def(py::self == py::self)
			;

	// rst: .. py:class:: Charge
	// rst:
	// rst:		Representation of the charge of an atom.
	// rst:
	py::class_<mod::Charge>("Charge", py::no_init)
			// rst:		.. py:method:: __init__(self)
			// rst:
			// rst:			Construct a neutral charge.
			.def(py::init<>())
			// rst:		.. py:method:: __init__(self, c)
			// rst:
			// rst:			Construct a specific charge. Pre-condition: the charge must be in the range :math:`[-9, 9]`.
			.def(py::init<signed char>())
			// rst:		.. py:method:: __init__(self)
			// rst:
			// rst:			Implicit conversion to an integer type.
			.def(int_(py::self))
			.def(str(py::self))
			.def(py::self == py::self)
			;

	// rst: .. py:class:: AtomData
	// rst:
	// rst:		Representation of basic data of an atom.
	// rst:
	py::class_<mod::AtomData>("AtomData", py::no_init)
			// rst:		.. py:method:: __init__(self)
			// rst:
			// rst:			Construct atom data with default values:
			// rst:
			// rst:			- :cpp:var:`AtomIds::Invalid` atom id,
			// rst:			- :cpp:expr:`Isotope()` as isotope,
			// rst:			- neutral charge, and
			// rst:			- no radical.
			.def(py::init<>())
			// rst:		.. py:method:: __init__(self, atomId)
			// rst:
			// rst:			Construct atom data the given atom id, and otherwise default values (see above).
			.def(py::init<AtomId>())
			// rst:		.. py:method:: __init__(self, atomId, charge)
			// rst:
			// rst:			Construct atom data with given atom id, charge, and radical, but with default isotope.
			.def(py::init<AtomId, Charge, bool>())
			// rst:		.. py:method:: __init__(self, atomId, charge)
			// rst:
			// rst:			Construct atom data with given atom id, isotope, charge, and radical.
			.def(py::init<AtomId, Isotope, Charge, bool>())
			// rst:		.. py:attribute: atomId
			// rst:
			// rst:			(Read-only) The atom id.
			// rst:
			// rst:			:type: :py:class:`AtomId`
			.add_property("atomId", &mod::AtomData::getAtomId)
			// rst:		.. py:attribute: isotope
			// rst:
			// rst:			(Read-only) The isotope.
			// rst:
			// rst:			:type: :py:class:`Isotope`
			.add_property("isotope", &mod::AtomData::getIsotope)
			// rst:		.. py:attribute:: charge
			// rst:
			// rst:			(Read-only) The charge.
			// rst:
			// rst:			:type: :py:class:`Charge`
			.add_property("charge", &mod::AtomData::getCharge)
			// rst:		.. py::attribute:: radical
			// rst:
			// rst:			(Read-only) The radical.
			// rst:
			// rst:			:type: bool
			.add_property("radical", &mod::AtomData::getRadical)
			// rst:		.. py::method:: __str__(self)
			// rst:
			// rst:			:returns: A string representation of the atom data adhering to the string encoding of atoms (see :ref:`mol-enc`).
			// rst:			:raises: :py::class:`LogicError` if the atom id is :py:const:`AtomIds.Invalid`.
			.def(str(py::self))
			.def(py::self == py::self)
			;

	// rst: .. py:class:: BondType
	// rst:
	// rst:		Representation of a bond type.
	// rst:
	py::enum_<mod::BondType>("BondType")
			// rst:		.. py:attribute:: Invalid
			.value("Invalid", mod::BondType::Invalid)
			// rst:		.. py:attribute:: Single
			.value("Single", mod::BondType::Single)
			// rst:		.. py:attribute:: Aromatic
			.value("Aromatic", mod::BondType::Aromatic)
			// rst:		.. py:attribute:: Double
			.value("Double", mod::BondType::Double)
			// rst:		.. py:attribute:: Triple
			.value("Triple", mod::BondType::Triple)
			// rst:		.. py:method:: __str__(self)
			// rst:
			// rst:			:returns: A string representation of the bond type adhering to the string encoding of bonds (see :ref:`mol-enc`).
			// rst:			:raises: :py:class:`LogicError` if the bond type is :py:const:`Invalid`.
			;
	// TOOD: py::enum_ does not support overriding of methods, so we set __str__ on the Python side.
	py::def("_bondTypeToString", &bondTypeToString);



	// rst: .. py:class:: AtomIds
	// rst:
	// rst:		This class contains constants for each chemical element, both as their abbreviations and their full names.
	// rst:		Two additional constants are provided for convenience.
	// rst:
	// rst:		.. py:attribute:: Invalid
	// rst:
	// rst:			Constant for the invalid atom id.
	// rst:
	// rst:			:type: :py:class:`AtomId`
	// rst:
	// rst:		.. py:attribute:: Max
	// rst:
	// rst:			An atom id equal to the atom id with the maximum valid id.
	// rst:
	// rst:			:type: :py:class:`AtomId`
	// rst:

	struct Dummy {
	};
	auto atomIds = py::class_<Dummy, boost::noncopyable>("AtomIds", py::no_init);
	atomIds.def_readonly("Invalid", &mod::AtomIds::Invalid);
	atomIds.def_readonly("Max", &mod::AtomIds::Max);
#define MOD_toString(s) MOD_toString1(s)
#define MOD_toString1(s) #s
#define MDO_CHEM_atomIdIter(r, data, t)                                         \
	atomIds.def_readonly(MOD_toString(BOOST_PP_TUPLE_ELEM(3, 1, t)), &mod::AtomIds::BOOST_PP_TUPLE_ELEM(3, 1, t));
	BOOST_PP_SEQ_FOR_EACH(MDO_CHEM_atomIdIter, ~, MOD_CHEM_ATOM_DATA())
#undef MDO_CHEM_atomIdIter
#define MDO_CHEM_atomIdIter(r, data, t)                                         \
	atomIds.def_readonly(MOD_toString(BOOST_PP_TUPLE_ELEM(3, 2, t)), &mod::AtomIds::BOOST_PP_TUPLE_ELEM(3, 2, t));
			BOOST_PP_SEQ_FOR_EACH(MDO_CHEM_atomIdIter, ~, MOD_CHEM_ATOM_DATA())
#undef MDO_CHEM_atomIdIter
}

} // namespace Py
} // namespace mod