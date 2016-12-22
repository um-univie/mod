#include "Depiction.h"

#include <mod/Config.h>
#include <mod/Error.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/Chem/OBabel.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/Graph/Properties/Molecule.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

namespace mod {
namespace lib {
namespace Graph {

DepictionData::DepictionData(const GraphType &g, const PropString &pString, const PropMolecule &pMol)
: g(g), moleculeState(pMol), hasMoleculeEncoding(true) {
	{ // vertexData
		std::vector<bool> atomUsed(AtomIds::Max + 1, false);
		Chem::markSpecialAtomsUsed(atomUsed);
		std::vector<Vertex> verticesToProcess;
		for(Vertex v : asRange(vertices(g))) {
			unsigned char atomId = pMol[v].getAtomId();
			if(atomId != AtomIds::Invalid) atomUsed[atomId] = true;
			else verticesToProcess.push_back(v);
		}
		// map non-atom labels to atoms
		std::map<std::string, AtomId> labelNoChargeToAtomId;
		for(Vertex v : verticesToProcess) {
			std::string label = std::get<0>(Chem::extractChargeRadical(pString[v]));
			auto iter = labelNoChargeToAtomId.find(label);
			if(iter == end(labelNoChargeToAtomId)) {
				unsigned char atomId = 1;
				for(; atomId <= AtomIds::Max; atomId++) {
					if(atomUsed[atomId]) continue;
					atomUsed[atomId] = true;
					iter = labelNoChargeToAtomId.insert(std::make_pair(label, AtomId(atomId))).first;
					phonyAtomToStringNoCharge[AtomId(atomId)] = label;
					break;
				}
				if(atomId > AtomIds::Max) {
					hasMoleculeEncoding = false;
					break;
				}
			}
			auto atomId = iter->second;
			nonAtomToPhonyAtom[v] = AtomData(atomId, pMol[v].getCharge(), pMol[v].getRadical());
		}
	}
	{ // edgeData
		for(Edge e : asRange(edges(g))) {
			if(pMol[e] == BondType::Invalid) nonBondEdges[e] = pString[e];
		}
	}

	if(hasMoleculeEncoding) {
#ifdef MOD_HAVE_OPENBABEL
		obMolAll = Chem::makeOBMol(g, std::cref(*this), std::cref(*this), true);
		obMolNoHydrogen = Chem::makeOBMol(g, std::cref(*this), std::cref(*this), false);
#endif
	}
}

AtomId DepictionData::getAtomId(Vertex v) const {
	return moleculeState[v].getAtomId();
}

Charge DepictionData::getCharge(Vertex v) const {
	return moleculeState[v].getCharge();
}

bool DepictionData::getRadical(Vertex v) const {
	return moleculeState[v].getRadical();
}

std::string DepictionData::getVertexLabelNoChargeRadical(Vertex v) const {
	if(!hasMoleculeEncoding) MOD_ABORT;
	auto atomId = getAtomId(v);
	if(atomId != AtomIds::Invalid) return Chem::symbolFromAtomId(atomId);
	else {
		auto nonAtomIter = nonAtomToPhonyAtom.find(v);
		assert(nonAtomIter != end(nonAtomToPhonyAtom));
		auto labelIter = phonyAtomToStringNoCharge.find(nonAtomIter->second.getAtomId());
		assert(labelIter != end(phonyAtomToStringNoCharge));
		return labelIter->second;
	}
}

BondType DepictionData::getBondData(Edge e) const {
	return moleculeState[e];
}

std::string DepictionData::getEdgeLabel(Edge e) const {
	if(!hasMoleculeEncoding) MOD_ABORT;
	auto bt = getBondData(e);
	if(bt != BondType::Invalid) return std::string(1, Chem::bondToChar(bt));
	auto iter = nonBondEdges.find(e);
	assert(iter != end(nonBondEdges));
	return iter->second;
}

const AtomData &DepictionData::operator()(Vertex v) const {
	if(moleculeState[v].getAtomId() != AtomIds::Invalid) return moleculeState[v];
	else {
		auto iter = nonAtomToPhonyAtom.find(v);
		assert(iter != end(nonAtomToPhonyAtom));
		return iter->second;
	}
}

BondType DepictionData::operator()(Edge e) const {
	auto eType = moleculeState[e];
	return eType != BondType::Invalid ? eType : BondType::Single;
}

bool DepictionData::getHasCoordinates() const {
#ifdef MOD_HAVE_OPENBABEL
	if(getConfig().io.useOpenBabelCoords.get())
		return hasMoleculeEncoding;
	else return false;
#else
	return false;
#endif  
}

double DepictionData::getX(Vertex v, bool withHydrogen) const {
	if(!getHasCoordinates()) MOD_ABORT;
#ifdef MOD_HAVE_OPENBABEL
	unsigned int vId = get(boost::vertex_index_t(), g, v);
	return withHydrogen ? lib::Chem::getOBAtomX(*obMolAll, vId) : lib::Chem::getOBAtomX(*obMolNoHydrogen, vId);
#else
	MOD_ABORT;
#endif
}

double DepictionData::getY(Vertex v, bool withHydrogen) const {
	if(!getHasCoordinates()) MOD_ABORT;
#ifdef MOD_HAVE_OPENBABEL
	unsigned int vId = get(boost::vertex_index_t(), g, v);
	return withHydrogen ? lib::Chem::getOBAtomY(*obMolAll, vId) : lib::Chem::getOBAtomY(*obMolNoHydrogen, vId);
#else
	MOD_ABORT;
#endif
}

void DepictionData::setImage(std::shared_ptr<mod::Function<std::string()> > image) {
	this->image = image;
}

std::shared_ptr<mod::Function<std::string()> > DepictionData::getImage() const {
	return image;
}

void DepictionData::setImageCommand(std::string cmd) {
	imageCmd = cmd;
}

std::string DepictionData::getImageCommand() const {
	return imageCmd;
}

} // namespace Graph
} // namespace lib
} // namespace mod
