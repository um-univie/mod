#include "Depiction.h"

#include <mod/Config.h>
#include <mod/Error.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/Chem/OBabel.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/Rules/Properties/String.h>
#include <mod/lib/Rules/Properties/Molecule.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <map>

namespace mod {
namespace lib {
namespace Rules {

//------------------------------------------------------------------------------
// template<Membership> DepictionData
//------------------------------------------------------------------------------

template<Membership membership>
DepictionDataCore::DepictionData<membership>::DepictionData(const DepictionDataCore &depict) : depict(depict) { }

template<Membership membership>
AtomId DepictionDataCore::DepictionData<membership>::getAtomId(Vertex v) const {
	const auto &molState = depict.moleculeState;
	switch(membership) {
	case Membership::Left:
		return molState.getLeft()[v].getAtomId();
	case Membership::Right:
		return molState.getRight()[v].getAtomId();
	case Membership::Context:
		auto leftId = molState.getLeft()[v].getAtomId();
		auto rightId = molState.getRight()[v].getAtomId();
		if(leftId == rightId) return leftId;
		else return AtomIds::Invalid;
	}
}

template<Membership membership>
Charge DepictionDataCore::DepictionData<membership>::getCharge(Vertex v) const {
	const auto &molState = depict.moleculeState;
	switch(membership) {
	case Membership::Left:
		return molState.getLeft()[v].getCharge();
	case Membership::Right:
		return molState.getRight()[v].getCharge();
	case Membership::Context:
		auto leftCharge = molState.getLeft()[v].getCharge();
		auto rightCharge = molState.getRight()[v].getCharge();
		if(leftCharge == rightCharge) return leftCharge;
		else return Charge(0);
	}
}

template<Membership membership>
bool DepictionDataCore::DepictionData<membership>::getRadical(Vertex v) const {
	const auto &molState = depict.moleculeState;
	switch(membership) {
	case Membership::Left:
		return molState.getLeft()[v].getRadical();
	case Membership::Right:
		return molState.getRight()[v].getRadical();
	case Membership::Context:
		auto leftRadical = molState.getLeft()[v].getRadical();
		auto rightRadical = molState.getRight()[v].getRadical();
		if(leftRadical == rightRadical) return leftRadical;
		else return false;
	}
}

template<Membership membership>
BondType DepictionDataCore::DepictionData<membership>::getBondData(Edge e) const {
	const auto &molState = depict.moleculeState;
	switch(membership) {
	case Membership::Left:
		return molState.getLeft()[e];
	case Membership::Right:
		return molState.getRight()[e];
	case Membership::Context:
		auto btLeft = molState.getLeft()[e];
		auto btRight = molState.getRight()[e];
		if(btLeft == btRight) return btLeft;
		else return BondType::Invalid;
	}
}

template<Membership membership>
std::string DepictionDataCore::DepictionData<membership>::getVertexLabelNoChargeRadical(Vertex v) const {
	auto atomId = getAtomId(v);
	if(atomId != AtomIds::Invalid)
		return Chem::symbolFromAtomId(atomId);
	switch(membership) {
	case Membership::Left:
	case Membership::Right:
	{
		const auto &nonAtomToPhonyAtom = membership == Membership::Left ? depict.nonAtomToPhonyAtomLeft : depict.nonAtomToPhonyAtomRight;
		auto nonAtomIter = nonAtomToPhonyAtom.find(v);
		assert(nonAtomIter != end(nonAtomToPhonyAtom));
		auto labelIter = depict.phonyAtomToString.find(nonAtomIter->second.getAtomId());
		assert(labelIter != end(depict.phonyAtomToString));
		return labelIter->second;
	}
	case Membership::Context:
	{
		auto nonAtomIterLeft = depict.nonAtomToPhonyAtomLeft.find(v);
		auto nonAtomIterRight = depict.nonAtomToPhonyAtomRight.find(v);
		std::string left, right;
		if(nonAtomIterLeft == end(depict.nonAtomToPhonyAtomLeft)) {
			auto atomId = depict.moleculeState.getLeft()[v].getAtomId();
			left = Chem::symbolFromAtomId(atomId);
		} else {
			auto labelIterLeft = depict.phonyAtomToString.find(nonAtomIterLeft->second.getAtomId());
			assert(labelIterLeft != end(depict.phonyAtomToString));
			left = labelIterLeft->second;
		}
		if(nonAtomIterRight == end(depict.nonAtomToPhonyAtomRight)) {
			auto atomId = depict.moleculeState.getRight()[v].getAtomId();
			right = Chem::symbolFromAtomId(atomId);
		} else {
			auto labelIterRight = depict.phonyAtomToString.find(nonAtomIterRight->second.getAtomId());
			assert(labelIterRight != end(depict.phonyAtomToString));
			right = labelIterRight->second;
		}
		if(left == right) return left;
		else return R"X($\langle$)X" + left + ", " + right + R"X($\rangle$)X";
	}
	}
}

template<Membership membership>
std::string DepictionDataCore::DepictionData<membership>::getEdgeLabel(Edge e) const {
	auto bt = getBondData(e);
	if(bt != BondType::Invalid)
		return std::string(1, Chem::bondToChar(bt));
	switch(membership) {
	case Membership::Left:
	case Membership::Right:
	{
		const auto &nonBondEdges = membership == Membership::Left ? depict.nonBondEdgesLeft : depict.nonBondEdgesRight;
		auto iter = nonBondEdges.find(e);
		assert(iter != end(nonBondEdges));
		return iter->second;
	}
	case Membership::Context:
	{
		std::string left, right;
		auto iterLeft = depict.nonBondEdgesLeft.find(e);
		auto iterRight = depict.nonBondEdgesRight.find(e);
		if(iterLeft != end(depict.nonBondEdgesLeft)) left = iterLeft->second;
		else left = std::string(1, Chem::bondToChar(depict.moleculeState.getLeft()[e]));
		if(iterRight != end(depict.nonBondEdgesRight)) right = iterRight->second;
		else right = std::string(1, Chem::bondToChar(depict.moleculeState.getRight()[e]));
		if(left == right) return left;
		else return "$\\langle$" + left + ", " + right + "$\\rangle$";
	}
	}
}

template<Membership membership>
const AtomData &DepictionDataCore::DepictionData<membership>::operator()(Vertex v) const {
	switch(membership) {
	case Membership::Left:
		if(getAtomId(v) != AtomIds::Invalid) return depict.moleculeState.getLeft()[v];
		else {
			auto iter = depict.nonAtomToPhonyAtomLeft.find(v);
			assert(iter != end(depict.nonAtomToPhonyAtomLeft));
			return iter->second;
		}
	case Membership::Right:
		if(getAtomId(v) != AtomIds::Invalid) return depict.moleculeState.getRight()[v];
		else {
			auto iter = depict.nonAtomToPhonyAtomRight.find(v);
			assert(iter != end(depict.nonAtomToPhonyAtomLeft));
			return iter->second;
		}
	case Membership::Context:
		// for now we just return whatever is in left, it's fake data anyway
		return depict.getLeft()(v);
	}
}

template<Membership membership>
BondType DepictionDataCore::DepictionData<membership>::operator()(Edge e) const {
	auto bt = getBondData(e);
	return bt == BondType::Invalid ? BondType::Single : bt;
}

template<Membership membership>
bool DepictionDataCore::DepictionData<membership>::getHasCoordinates() const {
	return depict.getHasCoordinates();
}

template<Membership membership>
double DepictionDataCore::DepictionData<membership>::getX(Vertex v, bool withHydrogen) const {
	return depict.getX(v);
}

template<Membership membership>
double DepictionDataCore::DepictionData<membership>::getY(Vertex v, bool withHydrogen) const {
	return depict.getY(v);
}

//template<Membership membership>
//bool DepictionDataCore::DepictionData<membership>::isAtomIdInvalidContext(Vertex v) const {
//	switch(membership) {
//	case Membership::Left:
//	case Membership::Right
//		return true;
//	}
//	const auto &molState = depict.moleculeState;
//	if(membership == Membership::Left) return true;
//	else if(membership == Membership::Right) return true;
//	else {
//		auto m = depict.g[v].membership;
//		//		if(m != Membership::StateChange) return true;
//		auto leftId = molState.getNormal(v).getAtomId();
//		auto rightId = molState.getChange(v).getAtomId();
//		if(leftId == rightId) return true;
//		else return false;
//	}
//}

//------------------------------------------------------------------------------
// DepictionDataCore
//------------------------------------------------------------------------------

DepictionDataCore::DepictionDataCore(const GraphType &g, const PropStringCore &labelState, const PropMoleculeCore &pMol)
: g(g), moleculeState(pMol), hasMoleculeEncoding(true), hasCoordinates(false) {
	{ // vertexData
		std::vector<bool> atomUsed(AtomIds::Max + 1, false);
		Chem::markSpecialAtomsUsed(atomUsed);
		std::vector < std::pair<Vertex, Membership> > verticesToProcess; // vertex x {Left, Right}
		for(Vertex v : asRange(vertices(g))) {
			auto m = g[v].membership;
			if(m != Membership::Right) {
				unsigned char atomId = pMol.getLeft()[v].getAtomId();
				if(atomId != AtomIds::Invalid) atomUsed[atomId] = true;
				else verticesToProcess.emplace_back(v, Membership::Left);
			}
			if(m != Membership::Left) {
				unsigned char atomId = pMol.getRight()[v].getAtomId();
				if(atomId != AtomIds::Invalid) atomUsed[atomId] = true;
				else verticesToProcess.emplace_back(v, Membership::Right);
			}
		}
		// map non-atom labels to atoms
		std::map<std::string, AtomId> labelToAtomId;
		for(auto p : verticesToProcess) {
			Vertex v = p.first;
			Membership m = p.second;
			assert(m != Membership::Context);
			auto label = std::get<0>(Chem::extractChargeRadical(m == Membership::Left ? labelState.getLeft()[v] : labelState.getRight()[v]));
			auto iter = labelToAtomId.find(label);
			if(iter == end(labelToAtomId)) {
				unsigned char atomId = 1;
				for(; atomId <= AtomIds::Max; atomId++) {
					if(atomUsed[atomId]) continue;
					atomUsed[atomId] = true;
					iter = labelToAtomId.emplace(label, AtomId(atomId)).first;
					phonyAtomToString[AtomId(atomId)] = label;
					break;
				}
				if(atomId > AtomIds::Max) {
					hasMoleculeEncoding = false;
					break;
				}
			}
			auto atomId = iter->second;
			if(m == Membership::Left) {
				auto charge = pMol.getLeft()[v].getCharge();
				bool radical = pMol.getLeft()[v].getRadical();
				nonAtomToPhonyAtomLeft[v] = AtomData(atomId, charge, radical);
			} else {
				auto charge = pMol.getRight()[v].getCharge();
				bool radical = pMol.getRight()[v].getRadical();
				nonAtomToPhonyAtomRight[v] = AtomData(atomId, charge, radical);
			}
		}
	}
	{ // edgeData
		for(Edge e : asRange(edges(g))) {
			auto m = g[e].membership;
			if(m != Membership::Right) {
				auto bt = pMol.getLeft()[e];
				if(bt == BondType::Invalid) nonBondEdgesLeft[e] = labelState.getLeft()[e];
			}
			if(m != Membership::Left) {
				auto bt = pMol.getRight()[e];
				if(bt == BondType::Invalid) nonBondEdgesRight[e] = labelState.getRight()[e];
			}
		}
	}

	if(hasMoleculeEncoding) {
#ifdef MOD_HAVE_OPENBABEL
		obMol = Chem::makeOBMol(g, std::cref(*this), std::cref(*this));
		x.resize(num_vertices(g));
		y.resize(num_vertices(g));
		for(Vertex v : asRange(vertices(g))) {
			unsigned int vId = get(boost::vertex_index_t(), g, v);
			x[vId] = lib::Chem::getOBAtomX(*obMol, vId);
			y[vId] = lib::Chem::getOBAtomY(*obMol, vId);
		}
		hasCoordinates = true;
#endif
	}
}

const AtomData &DepictionDataCore::operator()(Vertex v) const {
	// return whatever we find
	auto m = g[v].membership;
	if(m != Membership::Right) {
		auto atomId = moleculeState.getLeft()[v].getAtomId();
		if(atomId != AtomIds::Invalid) return moleculeState.getLeft()[v];
		else {
			auto iter = nonAtomToPhonyAtomLeft.find(v);
			assert(iter != end(nonAtomToPhonyAtomLeft));
			return iter->second;
		}
	} else {
		auto atomId = moleculeState.getRight()[v].getAtomId();
		if(atomId != AtomIds::Invalid) return moleculeState.getRight()[v];
		else {
			auto iter = nonAtomToPhonyAtomRight.find(v);
			assert(iter != end(nonAtomToPhonyAtomRight));
			return iter->second;
		}
	}
}

BondType DepictionDataCore::operator()(Edge e) const {
	// return whatever we find
	auto m = g[e].membership;
	if(m != Membership::Right) {
		auto bt = moleculeState.getLeft()[e];
		return bt == BondType::Invalid ? BondType::Single : bt;
	} else {
		auto bt = moleculeState.getRight()[e];
		return bt == BondType::Invalid ? BondType::Single : bt;
	}
}

bool DepictionDataCore::getHasCoordinates() const {
#ifdef MOD_HAVE_OPENBABEL
	if(getConfig().io.useOpenBabelCoords.get())
		return hasMoleculeEncoding;
	else return false;
#else
	return false;
#endif 
}

double DepictionDataCore::getX(Vertex v) const {
	if(!getHasCoordinates()) MOD_ABORT;
	unsigned int vId = get(boost::vertex_index_t(), g, v);
	assert(vId < x.size());
	return x[vId];
}

double DepictionDataCore::getY(Vertex v) const {
	if(!getHasCoordinates()) MOD_ABORT;
	unsigned int vId = get(boost::vertex_index_t(), g, v);
	assert(vId < y.size());
	return y[vId];
}

void DepictionDataCore::copyCoords(const DepictionDataCore &other, const std::map<Vertex, Vertex> &vMap) {
	if(!other.getHasCoordinates()) {
		IO::log() << "Can not copy coordinates from depiction without coordinates." << std::endl;
		MOD_ABORT;
	}
	x.resize(num_vertices(g));
	y.resize(num_vertices(g));
	for(Vertex v : asRange(vertices(g))) {
		auto iter = vMap.find(v);
		if(iter == end(vMap)) {
			IO::log() << "Vertex " << v << " (id=" << get(boost::vertex_index_t(), g, v) << ") not mapped." << std::endl;
			IO::log() << "Map:" << std::endl;
			for(auto p : vMap) IO::log() << "\t" << p.first << " => " << p.second << std::endl;
			IO::log() << "num_vertices: " << num_vertices(g) << std::endl;
			IO::log() << "other.num_vertices: " << num_vertices(other.g) << std::endl;
			MOD_ABORT;
		}
		Vertex vOther = iter->second;
		unsigned int vId = get(boost::vertex_index_t(), g, v);
		x[vId] = other.getX(vOther);
		y[vId] = other.getY(vOther);
	}
	hasCoordinates = true;
}

DepictionDataCore::DepictionData<Membership::Left> DepictionDataCore::getLeft() const {
	if(!hasMoleculeEncoding) MOD_ABORT;
	return DepictionData<Membership::Left>(*this);
}

DepictionDataCore::DepictionData<Membership::Context> DepictionDataCore::getContext() const {
	if(!hasMoleculeEncoding) MOD_ABORT;
	return DepictionData<Membership::Context>(*this);
}

DepictionDataCore::DepictionData<Membership::Right> DepictionDataCore::getRight() const {
	if(!hasMoleculeEncoding) MOD_ABORT;
	return DepictionData<Membership::Right>(*this);
}

template struct DepictionDataCore::DepictionData<Membership::Left>;
template struct DepictionDataCore::DepictionData<Membership::Context>;
template struct DepictionDataCore::DepictionData<Membership::Right>;

} // namespace Rules
} // namespace lib
} // namespace mod