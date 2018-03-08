#include "Molecule.h"

#include <mod/Error.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/Chem/OBabel.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/IO/IO.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

namespace mod {
namespace lib {
namespace Graph {

PropMolecule::PropMolecule(const GraphType &g, const PropString &pString) : Base(g), isMolecule(true) {
	// atomData
	this->vertexState.resize(num_vertices(g));
	for(Vertex v : asRange(vertices(g))) {
		AtomId atomId;
		Charge charge;
		bool radical;
		std::tie(atomId, charge, radical) = Chem::decodeVertexLabel(pString[v]);
		this->vertexState[get(boost::vertex_index_t(), g, v)] = AtomData(atomId, charge, radical);
		if(atomId == AtomIds::Invalid) isMolecule = false;
	}

	// edgeData
	this->edgeState.reserve(num_edges(g));
	for(Edge e : asRange(edges(g))) {
		BondType bt = Chem::decodeEdgeLabel(pString[e]);
		assert(get(boost::edge_index_t(), g, e) == this->edgeState.size());
		this->edgeState.push_back(bt);
		if(bt == BondType::Invalid) isMolecule = false;
	}

	verify(&g);
}

bool PropMolecule::getIsMolecule() const {
	return isMolecule;
}

#ifdef MOD_HAVE_OPENBABEL

const lib::Chem::OBMolHandle &PropMolecule::getOBMol() const {
	if(!isMolecule) {
		IO::log() << "MoleculeState: Trying to create OpenBabel::OBMol from non-molecule." << std::endl
				<< "Should DepictionData be used instead?" << std::endl;
		MOD_ABORT;
	}
	if(!obMol) {
		obMol = Chem::makeOBMol(this->g, [this](Vertex v) -> const AtomData& {
			return (*this)[v];
		}, [this](Edge e) {
			return (*this)[e];
		}, jla_boost::AlwaysFalse(), true, nullptr);
	}
	return obMol;
}

#endif

double PropMolecule::getMolarMass() const {
	if(!getIsMolecule()) {
		IO::log() << "MoleculeState: Trying to get molar mass of non-molecule." << std::endl;
		MOD_ABORT;
	} else {
#ifndef MOD_HAVE_OPENBABEL
		MOD_NO_OPENBABEL_ERROR
		std::exit(1);
#else
		return getOBMol().getMolarMass();
#endif
	}
}

double PropMolecule::getEnergy() const {
	if(!energy) {
#ifndef MOD_HAVE_OPENBABEL
		MOD_NO_OPENBABEL_ERROR
		IO::log() << "Energy calculation is not possible without Open Babel." << std::endl;
		IO::log() << "Energy values can be manually cached on graphs if calculation is not desired." << std::endl;
		std::exit(1);
#else
		energy = getOBMol().getEnergy();
#endif
	}
	return *energy;
}

void PropMolecule::cacheEnergy(double value) const {
	if(energy) {
		IO::log() << "Can not cache energy on graph already with energy." << std::endl;
		MOD_ABORT;
	}
	energy = value;
}

} // namespace Graph
} // namespace lib
} // namespace mod
