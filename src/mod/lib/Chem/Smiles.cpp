#include "Smiles.h"

#include <mod/Error.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/Graph/Properties/String.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

namespace mod {
namespace lib {
namespace Chem {

const std::vector<AtomId> &getSmilesOrganicSubset() {
	using namespace AtomIds;
	static std::vector<AtomId> atomIds{
		Boron,
		Carbon,
		Nitrogen,
		Oxygen,
		Phosphorus,
		Sulfur,
		Fluorine,
		Chlorine,
		Bromine,
		Iodine
	};
	return atomIds;
}

bool isInSmilesOrganicSubset(AtomId atomId) {
	// B, C, N, O, P, S, F, Cl, Br, I
	using namespace AtomIds;
	switch(atomId) {
	case Boron:
	case Carbon:
	case Nitrogen:
	case Oxygen:
	case Phosphorus:
	case Sulfur:
	case Fluorine:
	case Chlorine:
	case Bromine:
	case Iodine: return true;
	default: return false;
	}
}

void addImplicitHydrogens(lib::Graph::GraphType &g, lib::Graph::PropStringType &pString, lib::Graph::Vertex v, AtomId atomId,
		std::function<void(lib::Graph::GraphType&, lib::Graph::PropStringType&, lib::Graph::Vertex) > hydrogenAdder) {
	//==========================================================================
	// WARNING: keep in sync with the smiles writer
	//==========================================================================
	assert(isInSmilesOrganicSubset(atomId));
	// B, C, N, O, P, S, F, Cl, Br, I
	// B			3
	// C			4
	// N			3, 5, {{-, :, :}}, {{-, -, =}}, {{:, :, :}}
	// O			2
	// P			3, 5
	// S			2, 4, 6, {{:, :}}
	// halogens		1
	unsigned char numSingle = 0, numDouble = 0, numTriple = 0, numAromatic = 0, valenceNoAromatic = 0;
	for(lib::Graph::Edge eOut : asRange(out_edges(v, g))) {
		BondType bt = decodeEdgeLabel(pString[eOut]);
		switch(bt) {
		case BondType::Invalid: MOD_ABORT;
		case BondType::Single:
			numSingle++;
			break;
		case BondType::Double:
			numDouble++;
			break;
		case BondType::Triple:
			numTriple++;
			break;
		case BondType::Aromatic:
			numAromatic++;
			break;
		}
	}
	valenceNoAromatic = numSingle + numDouble * 2 + numTriple * 3;
	using BondVector = std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>;
	BondVector numBonds{numSingle, numDouble, numTriple, numAromatic};
	using namespace AtomIds;
	unsigned char valence = valenceNoAromatic + numAromatic;
	if(numAromatic > 0) valence++;
	auto fMissingH = [ = ]() -> unsigned char {
		switch(atomId) {
		case Boron:
			if(valence <= 3) return 3 - valence;
			else return 0;
		case Carbon:
			if(valence <= 4) return 4 - valence;
			else return 0;
		case Nitrogen:
			if(numBonds == BondVector(1, 0, 0, 2)) return 0;
			if(numBonds == BondVector(2, 1, 0, 0)) return 0;
			if(numBonds == BondVector(0, 0, 0, 3)) return 0;
		case Phosphorus:
			if(valence <= 3) return 3 - valence;
			else if(valence <= 5) return 5 - valence;
			else return 0;
		case Oxygen:
			if(valence <= 2) return 2 - valence;
			else return 0;
		case Sulfur:
			if(numBonds == BondVector(0, 0, 0, 2)) return 0;
			if(valence <= 2) return 2 - valence;
			else if(valence <= 4) return 4 - valence;
			else if(valence <= 6) return 6 - valence;
			else return 0;
		case Fluorine:
		case Chlorine:
		case Bromine:
		case Iodine:
			if(valence <= 1) return 1 - valence;
			else return 0;
		default: MOD_ABORT;
		}
		return 0;
	};
	unsigned char hToAdd = fMissingH();
	for(unsigned char i = 0; i < hToAdd; i++) hydrogenAdder(g, pString, v);
}

} // namespace Chem
} // namespace lib
} // namespace mod
