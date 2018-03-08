#include "Depiction.h"

#include <mod/Config.h>
#include <mod/Error.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/Chem/OBabel.h>
#include <mod/lib/IO/Graph.h>
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
	const auto &pMol = get_molecule(depict.lr);
	switch(membership) {
	case Membership::Left:
		return pMol.getLeft()[v].getAtomId();
	case Membership::Right:
		return pMol.getRight()[v].getAtomId();
	case Membership::Context:
		auto leftId = pMol.getLeft()[v].getAtomId();
		auto rightId = pMol.getRight()[v].getAtomId();
		if(leftId == rightId) return leftId;
		else return AtomIds::Invalid;
	}
}

template<Membership membership>
Charge DepictionDataCore::DepictionData<membership>::getCharge(Vertex v) const {
	const auto &pMol = get_molecule(depict.lr);
	switch(membership) {
	case Membership::Left:
		return pMol.getLeft()[v].getCharge();
	case Membership::Right:
		return pMol.getRight()[v].getCharge();
	case Membership::Context:
		auto leftCharge = pMol.getLeft()[v].getCharge();
		auto rightCharge = pMol.getRight()[v].getCharge();
		if(leftCharge == rightCharge) return leftCharge;
		else return Charge(0);
	}
}

template<Membership membership>
bool DepictionDataCore::DepictionData<membership>::getRadical(Vertex v) const {
	const auto &pMol = get_molecule(depict.lr);
	switch(membership) {
	case Membership::Left:
		return pMol.getLeft()[v].getRadical();
	case Membership::Right:
		return pMol.getRight()[v].getRadical();
	case Membership::Context:
		auto leftRadical = pMol.getLeft()[v].getRadical();
		auto rightRadical = pMol.getRight()[v].getRadical();
		if(leftRadical == rightRadical) return leftRadical;
		else return false;
	}
}

template<Membership membership>
BondType DepictionDataCore::DepictionData<membership>::getBondData(Edge e) const {
	const auto &pMol = get_molecule(depict.lr);
	switch(membership) {
	case Membership::Left:
		return pMol.getLeft()[e];
	case Membership::Right:
		return pMol.getRight()[e];
	case Membership::Context:
		auto btLeft = pMol.getLeft()[e];
		auto btRight = pMol.getRight()[e];
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
		const auto &pMol = get_molecule(depict.lr);
		auto nonAtomIterLeft = depict.nonAtomToPhonyAtomLeft.find(v);
		auto nonAtomIterRight = depict.nonAtomToPhonyAtomRight.find(v);
		std::string left, right;
		if(nonAtomIterLeft == end(depict.nonAtomToPhonyAtomLeft)) {
			auto atomId = pMol.getLeft()[v].getAtomId();
			left = Chem::symbolFromAtomId(atomId);
		} else {
			auto labelIterLeft = depict.phonyAtomToString.find(nonAtomIterLeft->second.getAtomId());
			assert(labelIterLeft != end(depict.phonyAtomToString));
			left = labelIterLeft->second;
		}
		if(nonAtomIterRight == end(depict.nonAtomToPhonyAtomRight)) {
			auto atomId = pMol.getRight()[v].getAtomId();
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
		if(iter == end(nonBondEdges)) IO::log() << "WTF: " << e << std::endl;
		assert(iter != end(nonBondEdges));
		return iter->second;
	}
	case Membership::Context:
	{
		std::string left, right;
		auto iterLeft = depict.nonBondEdgesLeft.find(e);
		auto iterRight = depict.nonBondEdgesRight.find(e);
		const auto &pMol = get_molecule(depict.lr);
		if(iterLeft != end(depict.nonBondEdgesLeft)) left = iterLeft->second;
		else left = std::string(1, Chem::bondToChar(pMol.getLeft()[e]));
		if(iterRight != end(depict.nonBondEdgesRight)) right = iterRight->second;
		else right = std::string(1, Chem::bondToChar(pMol.getRight()[e]));
		if(left == right) return left;
		else return "$\\langle$" + left + ", " + right + "$\\rangle$";
	}
	}
}

template<Membership membership>
const AtomData &DepictionDataCore::DepictionData<membership>::operator()(Vertex v) const {
	switch(membership) {
	case Membership::Left:
		if(getAtomId(v) != AtomIds::Invalid) return get_molecule(depict.lr).getLeft()[v];
		else {
			auto iter = depict.nonAtomToPhonyAtomLeft.find(v);
			assert(iter != end(depict.nonAtomToPhonyAtomLeft));
			return iter->second;
		}
	case Membership::Right:
		if(getAtomId(v) != AtomIds::Invalid) return get_molecule(depict.lr).getRight()[v];
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
bool DepictionDataCore::DepictionData<membership>::hasImportantStereo(Vertex v) const {
	const auto &lr = depict.lr;
	switch(membership) {
	case Membership::Left:
	{
		const auto &lg = get_labelled_left(lr);
		if(!has_stereo(lg)) return false;
		return !get_stereo(lg)[v]->morphismDynamicOk();
	}
	case Membership::Right:
	{
		const auto &lg = get_labelled_right(lr);
		if(!has_stereo(lg)) return false;
		return !get_stereo(lg)[v]->morphismDynamicOk();
	}
	case Membership::Context:
		return get_stereo(lr).inContext(v) && depict.hasImportantStereo(v);
	}
	MOD_ABORT;
}

template<Membership membership>
bool DepictionDataCore::DepictionData<membership>::getHasCoordinates() const {
	return depict.getHasCoordinates();
}

template<Membership membership>
double DepictionDataCore::DepictionData<membership>::getX(Vertex v, bool withHydrogen) const {
	return depict.getX(v, withHydrogen);
}

template<Membership membership>
double DepictionDataCore::DepictionData<membership>::getY(Vertex v, bool withHydrogen) const {
	return depict.getY(v, withHydrogen);
}

template<Membership membership>
lib::IO::Graph::Write::EdgeFake3DType DepictionDataCore::DepictionData<membership>::getEdgeFake3DType(Edge e, bool withHydrogen) const {
#ifndef MOD_HAVE_OPENBABEL
	MOD_NO_OPENBABEL_ERROR
#else
	assert(depict.hasMoleculeEncoding);
	const auto &g = get_graph(depict.lr);
	const auto vSrc = source(e, g);
	const auto vTar = target(e, g);
	const auto idSrc = get(boost::vertex_index_t(), g, vSrc);
	const auto idTar = get(boost::vertex_index_t(), g, vTar);
	const CoordData &cData = withHydrogen ? depict.cDataAll : depict.cDataNoHydrogen;
	switch(membership) {
	case Membership::Left:
		return cData.obMolLeft.getBondFake3D(idSrc, idTar);
	case Membership::Right:
		return cData.obMolRight.getBondFake3D(idSrc, idTar);
	case Membership::Context:
		if(get_stereo(depict.lr).inContext(vSrc) && get_stereo(depict.lr).inContext(vTar))
			return cData.obMolLeft.getBondFake3D(idSrc, idTar);
		else
			return lib::IO::Graph::Write::EdgeFake3DType::None;
	}
#endif
	MOD_ABORT;
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

DepictionDataCore::DepictionDataCore(const LabelledRule &lr) : lr(lr), hasMoleculeEncoding(true), hasCoordinates(false) {
	const auto &g = get_graph(lr);
	const auto &pString = get_string(lr);
	const auto &pMol = get_molecule(lr);
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
			auto label = std::get<0>(Chem::extractChargeRadical(m == Membership::Left ? pString.getLeft()[v] : pString.getRight()[v]));
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
				if(bt == BondType::Invalid) nonBondEdgesLeft[e] = pString.getLeft()[e];
			}
			if(m != Membership::Left) {
				auto bt = pMol.getRight()[e];
				if(bt == BondType::Invalid) nonBondEdgesRight[e] = pString.getRight()[e];
			}
		}
	}

	if(hasMoleculeEncoding) {
#ifdef MOD_HAVE_OPENBABEL
		const auto doIt = [&](CoordData & cData, const bool withHydrogen) {
			std::tie(cData.obMol, cData.obMolLeft, cData.obMolRight)
					= Chem::makeOBMol(lr, std::cref(*this), std::cref(*this), getLeft(), getLeft(), getRight(), getRight(), withHydrogen);
			cData.x.resize(num_vertices(g));
			cData.y.resize(num_vertices(g));
			for(const auto v : asRange(vertices(g))) {
				const auto vId = get(boost::vertex_index_t(), g, v);
				if(cData.obMol.hasAtom(vId)) {
					cData.x[vId] = cData.obMol.getAtomX(vId);
					cData.y[vId] = cData.obMol.getAtomY(vId);
				} else {
					assert(!withHydrogen);
					cData.x[vId] = std::numeric_limits<double>::quiet_NaN();
					cData.y[vId] = std::numeric_limits<double>::quiet_NaN();
				}
			}
		};
		doIt(cDataAll, true);
		doIt(cDataNoHydrogen, false);
		hasCoordinates = true;
#endif
	}
}

const AtomData &DepictionDataCore::operator()(Vertex v) const {
	const auto &g = get_graph(lr);
	const auto &pMol = get_molecule(lr);
	// return whatever we find
	auto m = g[v].membership;
	if(m != Membership::Right) {
		auto atomId = pMol.getLeft()[v].getAtomId();
		if(atomId != AtomIds::Invalid) return pMol.getLeft()[v];
		else {
			auto iter = nonAtomToPhonyAtomLeft.find(v);
			assert(iter != end(nonAtomToPhonyAtomLeft));
			return iter->second;
		}
	} else {
		auto atomId = pMol.getRight()[v].getAtomId();
		if(atomId != AtomIds::Invalid) return pMol.getRight()[v];
		else {
			auto iter = nonAtomToPhonyAtomRight.find(v);
			assert(iter != end(nonAtomToPhonyAtomRight));
			return iter->second;
		}
	}
}

BondType DepictionDataCore::operator()(Edge e) const {
	const auto &g = get_graph(lr);
	const auto &pMol = get_molecule(lr);
	// if there is agreement, return that, otherwise prefer single bonds
	// this should give a bit more freedom in bond angles
	const auto m = g[e].membership;
	BondType l = BondType::Single, r = BondType::Single;
	if(m != Membership::Right) {
		const auto bt = pMol.getLeft()[e];
		l = bt == BondType::Invalid ? BondType::Single : bt;
	} else {
		const auto bt = pMol.getRight()[e];
		r = bt == BondType::Invalid ? BondType::Single : bt;
	}
	if(l == r) return l;
	else return BondType::Single;
}

bool DepictionDataCore::hasImportantStereo(Vertex v) const {
	if(!has_stereo(lr)) return false;
	const auto &g = get_graph(lr);
	const auto m = g[v].membership;
	if(m != Membership::Right && !get_stereo(get_labelled_left(lr))[v]->morphismDynamicOk()) return true;
	if(m != Membership::Left && !get_stereo(get_labelled_right(lr))[v]->morphismDynamicOk()) return true;
	return false;
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

double DepictionDataCore::getX(Vertex v, bool withHydrogen) const {
	if(!getHasCoordinates()) MOD_ABORT;
	const auto &g = get_graph(lr);
	unsigned int vId = get(boost::vertex_index_t(), g, v);
	const CoordData &cData = withHydrogen ? cDataAll : cDataNoHydrogen;
	assert(vId < cData.x.size());
	return cData.x[vId];
}

double DepictionDataCore::getY(Vertex v, bool withHydrogen) const {
	if(!getHasCoordinates()) MOD_ABORT;
	const auto &g = get_graph(lr);
	unsigned int vId = get(boost::vertex_index_t(), g, v);
	const CoordData &cData = withHydrogen ? cDataAll : cDataNoHydrogen;
	assert(vId < cData.y.size());
	return cData.y[vId];
}

void DepictionDataCore::copyCoords(const DepictionDataCore &other, const std::map<Vertex, Vertex> &vMap) {
	if(!other.getHasCoordinates()) {
		IO::log() << "Can not copy coordinates from depiction without coordinates." << std::endl;
		MOD_ABORT;
	}
	const auto &g = get_graph(lr);
	const auto doIt = [&](CoordData &cData, const bool withHydrogen) {
		cData.x.resize(num_vertices(g));
		cData.y.resize(num_vertices(g));
		for(const Vertex v : asRange(vertices(g))) {
			const auto iter = vMap.find(v);
			if(iter == end(vMap)) {
				IO::log() << "Vertex " << v << " (id=" << get(boost::vertex_index_t(), g, v) << ") not mapped." << std::endl;
				IO::log() << "Map:" << std::endl;
				for(auto p : vMap) IO::log() << "\t" << p.first << " => " << p.second << std::endl;
				IO::log() << "num_vertices: " << num_vertices(g) << std::endl;
				IO::log() << "other.num_vertices: " << num_vertices(get_graph(other.lr)) << std::endl;
				MOD_ABORT;
			}
			const Vertex vOther = iter->second;
			const auto vId = get(boost::vertex_index_t(), g, v);
			cData.x[vId] = other.getX(vOther, withHydrogen);
			cData.y[vId] = other.getY(vOther, withHydrogen);
		}
		if(hasMoleculeEncoding) {
#ifdef MOD_HAVE_OPENBABEL
			cData.obMol.setCoordinates(cData.x, cData.y);
			cData.obMolLeft.setCoordinates(cData.x, cData.y);
			cData.obMolRight.setCoordinates(cData.x, cData.y);
#endif
		}
	};
	doIt(cDataAll, true);
	doIt(cDataNoHydrogen, false);
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