#include "Chem.h"

#include <mod/Error.h>
#include <mod/lib/Chem/MoleculeUtil.h>

#include <ostream>

namespace mod {

std::ostream &operator<<(std::ostream &s, AtomId atomId) {
	return s << static_cast<unsigned int> (atomId);
}

std::ostream &operator<<(std::ostream &s, Charge charge) {
	return s << static_cast<int> (charge);
}

//------------------------------------------------------------------------------
// AtomData
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &s, const AtomData &data) {
	if(data.atomId == AtomIds::Invalid) throw LogicError("Can not print atom data with atom id AtomIds::Invalid.");
	s << lib::Chem::symbolFromAtomId(data.atomId);
	if(data.charge == 0) return s;
	if(data.charge > 1 || data.charge < -1) s << std::abs(data.charge);
	if(data.charge < 0) s << '-';
	else s << '+';
	if(data.radical) s << '.';
	return s;
}

//------------------------------------------------------------------------------
// BondType
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &s, BondType bt) {
	if(bt == BondType::Invalid) throw LogicError("Can not print bond type BondType::Invalid.");
	return s << lib::Chem::bondToChar(bt);
}

} // namespace mod