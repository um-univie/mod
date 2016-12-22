#include "MoleculeUtil.h"

#include <mod/Config.h>
#include <mod/Error.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/bimap/bimap.hpp>
#include <boost/graph/copy.hpp>

#include <map>

namespace mod {
namespace lib {
namespace Chem {

//------------------------------------------------------------------------------
// Other
//------------------------------------------------------------------------------

std::tuple<std::string, Charge, bool> extractChargeRadical(std::string label) {
	auto lenRes = extractChargeRadicalLen(label);
	label.erase(std::get<0>(lenRes));
	return std::make_tuple(std::move(label), std::get<1>(lenRes), std::get<2>(lenRes));
}

std::tuple<std::size_t, Charge, bool> extractChargeRadicalLen(const std::string &label) {
	assert(!label.empty());
	long int lastPos = label.size() - 1;
	signed char charge = 0;
	bool radical = false;
	if(lastPos >= 0 && label[lastPos] == '.') {
		radical = true;
		--lastPos;
	}
	if(lastPos >= 1 // "+" and "-" as labels do not have charge
			&& (label[lastPos] == '+' || label.back() == '-')) {
		charge = label[lastPos] == '+' ? 1 : -1;
		--lastPos;
		if(lastPos >= 1) { // "4+" have charge +1, and label "4"
			char maybeNum = label[lastPos];
			if(maybeNum >= '0' && maybeNum <= '9') {
				charge *= maybeNum - '0';
				--lastPos;
			}
		}
	}
	return std::make_tuple(lastPos + 1, Charge(charge), radical);
}

AtomId atomIdFromSymbol(const std::string &label) {
	return atomIdFromSymbol(label, label.size());
}

AtomId atomIdFromSymbol(const std::string &label, std::size_t len) {
	using namespace AtomIds;
	if(len == 1) {
		switch(label[0]) {
		case 'B': return B;
		case 'C': return C;
		case 'F': return F;
		case 'H': return H;
		case 'I': return I;
		case 'K': return K;
		case 'N': return N;
		case 'O': return O;
		case 'P': return P;
		case 'S': return S;
		case 'U': return U;
		case 'V': return V;
		case 'W': return W;
		case 'Y': return Y;
		default: return Invalid;
		}
	} else if(len == 2) {
		char c1 = label[0], c2 = label[1];
		switch(c1) {
		case 'Z':
			switch(c2) {
			case 'r': return Zr;
			case 'n': return Zn;
			default: return Invalid;
			}
		case 'Y':
			switch(c2) {
			case 'b': return Yb;
			default: return Invalid;
			}
		case 'X':
			switch(c2) {
			case 'e': return Xe;
			default: return Invalid;
			}
		case 'S':
			switch(c2) {
			case 'r': return Sr;
			case 'i': return Si;
			case 'n': return Sn;
			case 'm': return Sm;
			case 'c': return Sc;
			case 'b': return Sb;
			case 'g': return Sg;
			case 'e': return Se;
			default: return Invalid;
			}
		case 'R':
			switch(c2) {
			case 'u': return Ru;
			case 'h': return Rh;
			case 'n': return Rn;
			case 'b': return Rb;
			case 'a': return Ra;
			case 'g': return Rg;
			case 'f': return Rf;
			case 'e': return Re;
			default: return Invalid;
			}
		case 'P':
			switch(c2) {
			case 'r': return Pr;
			case 'u': return Pu;
			case 't': return Pt;
			case 'o': return Po;
			case 'm': return Pm;
			case 'b': return Pb;
			case 'a': return Pa;
			case 'd': return Pd;
			default: return Invalid;
			}
		case 'T':
			switch(c2) {
			case 'i': return Ti;
			case 'h': return Th;
			case 'm': return Tm;
			case 'l': return Tl;
			case 'c': return Tc;
			case 'b': return Tb;
			case 'a': return Ta;
			case 'e': return Te;
			default: return Invalid;
			}
		case 'K':
			switch(c2) {
			case 'r': return Kr;
			default: return Invalid;
			}
		case 'I':
			switch(c2) {
			case 'r': return Ir;
			case 'n': return In;
			default: return Invalid;
			}
		case 'H':
			switch(c2) {
			case 's': return Hs;
			case 'o': return Ho;
			case 'g': return Hg;
			case 'f': return Hf;
			case 'e': return He;
			default: return Invalid;
			}
		case 'O':
			switch(c2) {
			case 's': return Os;
			default: return Invalid;
			}
		case 'N':
			switch(c2) {
			case 'p': return Np;
			case 'i': return Ni;
			case 'o': return No;
			case 'b': return Nb;
			case 'a': return Na;
			case 'e': return Ne;
			case 'd': return Nd;
			default: return Invalid;
			}
		case 'M':
			switch(c2) {
			case 'o': return Mo;
			case 't': return Mt;
			case 'g': return Mg;
			case 'n': return Mn;
			case 'd': return Md;
			default: return Invalid;
			}
		case 'L':
			switch(c2) {
			case 'r': return Lr;
			case 'a': return La;
			case 'i': return Li;
			case 'u': return Lu;
			case 'v': return Lv;
			default: return Invalid;
			}
		case 'C':
			switch(c2) {
			case 's': return Cs;
			case 'r': return Cr;
			case 'u': return Cu;
			case 'o': return Co;
			case 'n': return Cn;
			case 'm': return Cm;
			case 'l': return Cl;
			case 'a': return Ca;
			case 'f': return Cf;
			case 'e': return Ce;
			case 'd': return Cd;
			default: return Invalid;
			}
		case 'B':
			switch(c2) {
			case 'r': return Br;
			case 'k': return Bk;
			case 'i': return Bi;
			case 'h': return Bh;
			case 'a': return Ba;
			case 'e': return Be;
			default: return Invalid;
			}
		case 'A':
			switch(c2) {
			case 's': return As;
			case 'r': return Ar;
			case 'u': return Au;
			case 't': return At;
			case 'm': return Am;
			case 'l': return Al;
			case 'c': return Ac;
			case 'g': return Ag;
			default: return Invalid;
			}
		case 'G':
			switch(c2) {
			case 'a': return Ga;
			case 'e': return Ge;
			case 'd': return Gd;
			default: return Invalid;
			}
		case 'F':
			switch(c2) {
			case 'r': return Fr;
			case 'm': return Fm;
			case 'e': return Fe;
			case 'l': return Fl;
			default: return Invalid;
			}
		case 'E':
			switch(c2) {
			case 's': return Es;
			case 'r': return Er;
			case 'u': return Eu;
			default: return Invalid;
			}
		case 'D':
			switch(c2) {
			case 's': return Ds;
			case 'b': return Db;
			case 'y': return Dy;
			default: return Invalid;
			}
		default: return Invalid;
		}
	} else if(len == 3) {
		char c1 = label[0], c2 = label[1], c3 = label[2];
		switch(c1) {
		case 'U':
			switch(c2) {
			case 'u':
				switch(c3) {
				case 't': return Uut;
				case 'o': return Uuo;
				case 'p': return Uup;
				case 's': return Uus;
				default: return Invalid;
				}
			default: return Invalid;
			}
		default: return Invalid;
		}
	} else return Invalid;
}

std::tuple<AtomId, Charge, bool> decodeVertexLabel(const std::string &label) {
	auto lenChargeRadical = extractChargeRadicalLen(label);
	auto atomId = atomIdFromSymbol(label, std::get<0>(lenChargeRadical));
	return std::make_tuple(atomId, std::get<1>(lenChargeRadical), std::get<2>(lenChargeRadical));
}

BondType decodeEdgeLabel(const std::string &label) {
	if(label.size() == 1) {
		switch(label[0]) {
		case '-': return BondType::Single;
		case ':': return BondType::Aromatic;
		case '=': return BondType::Double;
		case '#': return BondType::Triple;
		default: return BondType::Invalid;
		}
	} else return BondType::Invalid;
}

void markSpecialAtomsUsed(std::vector<bool> &used) {
	used[AtomIds::Hydrogen] = true;
	used[AtomIds::Carbon] = true;
	used[AtomIds::Nitrogen] = true;
	used[AtomIds::Oxygen] = true;
}

std::string symbolFromAtomId(AtomId atomId) {
	switch(atomId) {
#define MOD_toString(s) MOD_toString1(s)
#define MOD_toString1(s) #s
#define MDO_CHEM_atomIdIter(r, data, t)                                         \
	case BOOST_PP_TUPLE_ELEM(MOD_CHEM_ATOM_DATA_ELEM_SIZE(), 0, t): return MOD_toString(BOOST_PP_TUPLE_ELEM(3, 1, t));
		BOOST_PP_SEQ_FOR_EACH(MDO_CHEM_atomIdIter, ~, MOD_CHEM_ATOM_DATA())
	default: MOD_ABORT;
	}
#undef MDO_CHEM_atomIdIter
#undef MOD_toString1
#undef MOD_toString
}

void appendSymbolFromAtomId(std::string &s, AtomId atomId) {
	switch(atomId) {
#define MOD_toString(s) MOD_toString1(s)
#define MOD_toString1(s) #s
#define MDO_CHEM_atomIdIter(r, data, t)                                                      \
	case BOOST_PP_TUPLE_ELEM(MOD_CHEM_ATOM_DATA_ELEM_SIZE(), 0, t):                            \
		for(unsigned int i = 0; i < sizeof(MOD_toString(BOOST_PP_TUPLE_ELEM(3, 1, t))) - 1; i++) \
			s += MOD_toString(BOOST_PP_TUPLE_ELEM(3, 1, t))[i];                                    \
		break;
		BOOST_PP_SEQ_FOR_EACH(MDO_CHEM_atomIdIter, ~, MOD_CHEM_ATOM_DATA())
	default: MOD_ABORT;
	}
#undef MDO_CHEM_atomIdIter
#undef MOD_toString1
#undef MOD_toString
}

char bondToChar(BondType bt) {
	switch(bt) {
	case BondType::Invalid:
		MOD_ABORT;
		break;
	case BondType::Single:
		return '-';
	case BondType::Aromatic:
		return ':';
	case BondType::Double:
		return '=';
	case BondType::Triple:
		return '#';
	default:
		MOD_ABORT;
	}
}

} // namespace Chem
} // namespace lib
} // namespace mod