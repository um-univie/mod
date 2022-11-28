#include "Tetrahedral.hpp"

#include <mod/Error.hpp>

namespace mod::lib::Stereo {

Tetrahedral::Tetrahedral(const GeometryGraph &g, const std::array<EmbeddingEdge, 4> &edges_, bool fixed)
: StaticDegree<4>(g.tetrahedral, edges_), fixed(fixed) {
	// We don't want to visualise wedge and hash bonds thar are Double, Triple, or Aromatic, so we permuted it a bit.
	// If there are too many, we give up.
	auto getIsMulti = [](const EmbeddingEdge & emb) {
		switch(emb.cat) {
		case EdgeCategory::Any:
		case EdgeCategory::Undefined:
		case EdgeCategory::Single: return false;
		case EdgeCategory::Double:
		case EdgeCategory::Triple:
		case EdgeCategory::Aromatic: return true;
		}
		MOD_ABORT;
	};
	std::bitset<4> isMulti;
	auto rotateLeft = [this, &isMulti]() {
		auto e1 = edges[1];
		edges[1] = edges[2];
		edges[2] = edges[3];
		edges[3] = e1;
		bool b1 = isMulti[1];
		isMulti[1] = isMulti[2];
		isMulti[2] = isMulti[3];
		isMulti[3] = b1;
	};
	auto rotateRight = [this, &isMulti]() {
		auto e3 = edges[3];
		edges[3] = edges[2];
		edges[2] = edges[1];
		edges[1] = e3;
		bool b3 = isMulti[3];
		isMulti[3] = isMulti[2];
		isMulti[2] = isMulti[1];
		isMulti[1] = b3;
	};
	auto doSwap = [this, &isMulti](std::size_t a, std::size_t b) {
		std::swap(edges[a], edges[b]);
		bool v = isMulti[a];
		isMulti[a] = isMulti[b];
		isMulti[b] = v;
	};
	for(std::size_t i = 0; i < 4; ++i)
		isMulti.set(i, getIsMulti(edges[i]));
	if(isMulti.count() == 0); // yay
	else if(isMulti.count() == 1) {
		if(isMulti[2]) rotateLeft();
		else if(isMulti[3]) rotateRight();
	} else if(isMulti.count() == 2) {
		if(!isMulti[0]) {
			// rotate one down to [1] and do the swap
			if(isMulti[1]); // yay, only the swap
			else {
				assert(isMulti[2]);
				assert(isMulti[3]);
				rotateLeft();
			}
			// apply (0 1)(2 3)
			doSwap(0, 1);
			doSwap(2, 3);
		}
		assert(isMulti[0]);
		if(isMulti[1]); // yay
		else if(isMulti[2]) rotateLeft();
		else if(isMulti[3]) rotateRight();
		else MOD_ABORT;
	} else {
		MOD_ABORT;
	}
}

std::unique_ptr<Configuration> Tetrahedral::cloneFree(const GeometryGraph &g) const {
	return std::make_unique<Tetrahedral>(g, edges, fixed);
}

std::unique_ptr<Configuration> Tetrahedral::clone(const GeometryGraph &g, const std::vector<std::size_t> &offsetMap) const {
	auto c = std::make_unique<Tetrahedral>(g, edges, fixed);
	for(auto &emb : c->edges) {
		if(emb.type == EmbeddingEdge::Type::Edge) {
			emb.offset = offsetMap[emb.offset];
		}
	}
	return c;
}

Fixation Tetrahedral::getFixation() const {
	return Fixation(fixed);
}

bool Tetrahedral::localPredIso(const Configuration &other) const {
	assert(dynamic_cast<const Tetrahedral*> (&other));
	const Tetrahedral &o = static_cast<const Tetrahedral&> (other);
	return this->fixed == o.fixed;
}

bool Tetrahedral::localPredSpec(const Configuration &other) const {
	assert(dynamic_cast<const Tetrahedral*> (&other));
	const Tetrahedral &o = static_cast<const Tetrahedral&> (other);
	// if we are free, it's fine, otherwise the other must be fixed
	return !this->fixed || o.fixed;
}

bool Tetrahedral::morphismStaticOk() const {
	return false;
}

bool Tetrahedral::morphismDynamicOk() const {
	return !this->fixed;
}

bool Tetrahedral::morphismIso(const Configuration &cCodomBase, std::vector<std::size_t> &perm) const {
	static constexpr std::array<std::array<std::size_t, 4>, 12> Good = {
		{
			//     0
			//     |
			//     |   
			//   _-*---3 hash
			//  -   \_
			// 1     2 wedge
			{0, 1, 2, 3},
			{0, 2, 3, 1},
			{0, 3, 1, 2},
			{1, 0, 3, 2},
			{1, 2, 0, 3},
			{1, 3, 2, 0},
			{2, 0, 1, 3},
			{2, 1, 3, 0},
			{2, 3, 0, 1},
			{3, 0, 2, 1},
			{3, 1, 0, 2},
			{3, 2, 1, 0}
		}
	};
	static constexpr std::array<std::array<std::size_t, 4>, 12> Bad = {
		{
			{0, 1, 3, 2},
			{0, 2, 1, 3},
			{0, 3, 2, 1},
			{1, 0, 2, 3},
			{1, 2, 3, 0},
			{1, 3, 0, 2},
			{2, 0, 3, 1},
			{2, 1, 0, 3},
			{2, 3, 1, 0},
			{3, 0, 1, 2},
			{3, 1, 2, 0},
			{3, 2, 0, 1}
		}
	};
	assert(dynamic_cast<const Tetrahedral*> (&cCodomBase));
	//	const Tetrahedral &cDom = *this;
	//	const Tetrahedral &cCodom = static_cast<const Tetrahedral&> (cCodomBase);
	assert(perm.size() == 4);
	std::array<std::size_t, 4> p;
	std::copy(perm.begin(), perm.end(), p.begin());
	const bool isIncomplete = std::find(perm.begin(), perm.end(), -1) != perm.end();
	if(isIncomplete) {
		MOD_ABORT;
	} else {
		const bool good = std::binary_search(Good.begin(), Good.end(), p);
		const bool bad = std::binary_search(Bad.begin(), Bad.end(), p);
		assert(good != bad);
		(void) bad;
		return good;
	}
}

bool Tetrahedral::morphismSpec(const Configuration &cCodomBase, std::vector<std::size_t> &perm) const {
	assert(cCodomBase.getGeometryVertex() == this->getGeometryVertex());
	// TODO: anything we should do different?
	return morphismIso(cCodomBase, perm);
}

std::string Tetrahedral::asRawStringImpl(std::function<std::size_t(const EmbeddingEdge&)> getNeighbourId) const {
	if(fixed) return "!";
	else return "";
}

std::pair<std::string, bool> Tetrahedral::asPrettyStringImpl(std::function<std::size_t(const EmbeddingEdge&)> getNeighbourId) const {
	std::string res = "\\modStereoShape{Tetrahedral";
	if(!fixed) return std::make_pair(res + "Sym}", false);
	res += "Fixed}";
	return std::make_pair(res, true);
}

} // namespace mod::lib::Stereo