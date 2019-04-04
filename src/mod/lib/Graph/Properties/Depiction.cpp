#include "Depiction.h"

#include <mod/Config.h>
#include <mod/Error.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/Chem/OBabel.h>
#include <mod/lib/Graph/Properties/Molecule.h>
#include <mod/lib/Graph/Properties/Stereo.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/IO/Graph.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <boost/lexical_cast.hpp>

namespace mod {
namespace lib {
namespace Graph {

DepictionData::DepictionData(const LabelledGraph &lg) : lg(lg), hasMoleculeEncoding(true) {
	const auto &g = get_graph(lg);
	const auto &pString = get_string(lg);
	const auto &pMol = get_molecule(lg);
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
		std::map<std::string, AtomId> labelNoStuff;
		for(Vertex v : verticesToProcess) {
			std::string label = std::get<0>(Chem::extractIsotopeChargeRadical(pString[v]));
			auto iter = labelNoStuff.find(label);
			if(iter == end(labelNoStuff)) {
				unsigned char atomId = 1;
				for(; atomId <= AtomIds::Max; atomId++) {
					if(atomUsed[atomId]) continue;
					atomUsed[atomId] = true;
					iter = labelNoStuff.insert(std::make_pair(label, AtomId(atomId))).first;
					phonyAtomToStringNoStuff[AtomId(atomId)] = label;
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
}

AtomId DepictionData::getAtomId(Vertex v) const {
	return get_molecule(lg)[v].getAtomId();
}

Isotope DepictionData::getIsotope(Vertex v) const {
	return get_molecule(lg)[v].getIsotope();
}

Charge DepictionData::getCharge(Vertex v) const {
	return get_molecule(lg)[v].getCharge();
}

bool DepictionData::getRadical(Vertex v) const {
	return get_molecule(lg)[v].getRadical();
}

std::string DepictionData::getVertexLabelNoIsotopeChargeRadical(Vertex v) const {
	if(!hasMoleculeEncoding) MOD_ABORT;
	auto atomId = getAtomId(v);
	if(atomId != AtomIds::Invalid) return Chem::symbolFromAtomId(atomId);
	else {
		auto nonAtomIter = nonAtomToPhonyAtom.find(v);
		assert(nonAtomIter != end(nonAtomToPhonyAtom));
		auto labelIter = phonyAtomToStringNoStuff.find(nonAtomIter->second.getAtomId());
		assert(labelIter != end(phonyAtomToStringNoStuff));
		return labelIter->second;
	}
}

BondType DepictionData::getBondData(Edge e) const {
	return get_molecule(lg)[e];
}

bool DepictionData::hasImportantStereo(Vertex v) const {
	if(!has_stereo(lg)) return false;
	return !get_stereo(lg)[v]->morphismDynamicOk();
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
	if(get_molecule(lg)[v].getAtomId() != AtomIds::Invalid) return get_molecule(lg)[v];
	else {
		auto iter = nonAtomToPhonyAtom.find(v);
		assert(iter != end(nonAtomToPhonyAtom));
		return iter->second;
	}
}

BondType DepictionData::operator()(Edge e) const {
	auto eType = get_molecule(lg)[e];
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
	unsigned int vId = get(boost::vertex_index_t(), get_graph(lg), v);
	const auto &mol = getOB(withHydrogen);
	return mol.getAtomX(vId);
#else
	MOD_ABORT;
#endif
}

double DepictionData::getY(Vertex v, bool withHydrogen) const {
	if(!getHasCoordinates()) MOD_ABORT;
#ifdef MOD_HAVE_OPENBABEL
	unsigned int vId = get(boost::vertex_index_t(), get_graph(lg), v);
	const auto &mol = getOB(withHydrogen);
	return mol.getAtomY(vId);
#else
	MOD_ABORT;
#endif
}

lib::IO::Graph::Write::EdgeFake3DType DepictionData::getEdgeFake3DType(Edge e, bool withHydrogen) const {
	if(!has_stereo(lg)) 
		return lib::IO::Graph::Write::EdgeFake3DType::None;
#ifdef MOD_HAVE_OPENBABEL
	auto idSrc = get(boost::vertex_index_t(), get_graph(lg), source(e, get_graph(lg)));
	auto idTar = get(boost::vertex_index_t(), get_graph(lg), target(e, get_graph(lg)));
	const auto &mol = getOB(withHydrogen);
	return mol.getBondFake3D(idSrc, idTar);
#else
	MOD_ABORT;
#endif
}

std::string DepictionData::getRawStereoString(Vertex v) const {
	const auto &conf = *get_stereo(lg)[v];
	const auto getNeighbourId = [&](const lib::Stereo::EmbeddingEdge & emb) {
		const auto &g = get_graph(lg);
		return get(boost::vertex_index_t(), g, target(emb.getEdge(v, g), g));
	};
	return conf.asRawString(getNeighbourId);
}

std::string DepictionData::getPrettyStereoString(Vertex v) const {
	const auto &conf = *get_stereo(lg)[v];
	const auto getNeighbourId = [&](const lib::Stereo::EmbeddingEdge & emb) {
		const auto &g = get_graph(lg);
		return get(boost::vertex_index_t(), g, target(emb.getEdge(v, g), g));
	};
	return conf.asPrettyString(getNeighbourId);
}

std::string DepictionData::getStereoString(Edge e) const {
	const auto cat = get_stereo(lg)[e];
	return boost::lexical_cast<std::string>(cat);
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

const lib::Chem::OBMolHandle &DepictionData::getOB(bool withHydrogen) const {
	if(!hasMoleculeEncoding) MOD_ABORT;
#ifdef MOD_HAVE_OPENBABEL
	if(!obMolAll) {
		const auto &g = get_graph(lg);
		const auto *pStereo = has_stereo(lg) ? &get_stereo(lg) : nullptr;
		const auto hasImportantStereo = [pStereo](const auto v) {
			if(!pStereo) return false;
			return !(*pStereo)[v]->morphismDynamicOk();
		};
		obMolAll = Chem::makeOBMol(g, std::cref(*this), std::cref(*this), hasImportantStereo, true, pStereo);
		obMolNoHydrogen = Chem::makeOBMol(g, std::cref(*this), std::cref(*this), hasImportantStereo, false, pStereo);
	}
	return withHydrogen ? obMolAll : obMolNoHydrogen;
#endif
}

} // namespace Graph
} // namespace lib
} // namespace mod
