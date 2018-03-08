#include "GraphState.h"

#include <mod/lib/Graph/Single.h>
#include <mod/lib/IO/IO.h>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

void GraphState::commonInit() {
	// subset 0 is special
	subsets.insert(SubsetStore::value_type(0, Subset(*this)));
	assert(subsets.size() >= 1);
	assert(subsets.begin()->first == 0);
}

GraphState::GraphState() {
	commonInit();
}

GraphState::GraphState(const GraphState &other) : universe(other.universe) {
	assert(other.subsets.size() >= 1);
	assert(other.subsets.begin()->first == 0);
	// commonInit is not needed, as other should have subset 0
	// we need to give the subsets new owners
	for(const SubsetStore::value_type &p : other.subsets) {
		assert(p.first == 0); // TODO: remove
		subsets.insert(SubsetStore::value_type(p.first, Subset(*this, p.second)));
	}
}

GraphState::GraphState(const std::vector<const Graph::Single*> &universe) : universe(universe) {
	commonInit();
}

GraphState::GraphState(const std::vector<const GraphState*> &resultSets) {
#ifndef NDEBUG
	for(const GraphState *rs : resultSets) {
		assert(rs->subsets.size() >= 1);
		assert(rs->subsets.begin()->first == 0);
	}
#endif
	// collect universe

	for(const GraphState *rs : resultSets) {
		GraphState::GraphList other(rs->getUniverse());

		std::sort(other.begin(), other.end(), [](const Graph::Single *g1, const Graph::Single * g2) {
			return g1->getId() < g2->getId();
		});
		GraphState::GraphList result;

		std::set_union(universe.begin(), universe.end(), other.begin(), other.end(), std::back_inserter(result), lib::Graph::Single::IdLess());
		std::swap(result, universe);
	}
	// collect all subsets
	using NewSubsetStore = std::map<unsigned int, std::set<const Graph::Single*, lib::Graph::Single::IdLess> >;
	NewSubsetStore newSubsets;
	// make sure the special 0th subset is there
	newSubsets.insert(std::make_pair(0, std::set<const Graph::Single*, lib::Graph::Single::IdLess>()));
	for(const GraphState *rs : resultSets) {
		for(const SubsetStore::value_type &p : rs->subsets) {
			assert(p.first == 0); // TODO: remove
			for(const Graph::Single *g : p.second) newSubsets[p.first].insert(g);
		}
	}

	using GraphToIndex = std::map<const Graph::Single*, unsigned int>;
	GraphToIndex graphToIndex;
	for(unsigned int i = 0; i < universe.size(); i++) graphToIndex[universe[i]] = i;

	for(const NewSubsetStore::value_type &newSubset : newSubsets) {
		assert(newSubset.first == 0); // TODO: remove
		std::pair < SubsetStore::iterator, bool> p = subsets.insert(std::make_pair(newSubset.first, Subset(*this)));
		assert(p.second);
		Subset &subset = p.first->second;
		for(const Graph::Single *g : newSubset.second) subset.indices.push_back(graphToIndex[g]);
	}

	assert(subsets.size() >= 1);
	assert(subsets.begin()->first == 0);
}

GraphState::~GraphState() { }

//void GraphState::promoteToSubset(unsigned int subsetIndex, const lib::Graph::Single *g) {
//	unsigned int oldUniverseSize = universe.size();
//	unsigned int gIndex = addUniverseGetIndex(g);
//	if(gIndex != oldUniverseSize) return;
//
//	SubsetStore::iterator iter = subsets.find(subsetIndex);
//	if(iter == subsets.end()) iter = subsets.insert(SubsetStore::value_type(subsetIndex, Subset(*this))).first;
//	Subset &subset = iter->second;
//
//	for(unsigned int index : subset.indices) {
//		if(index == gIndex) return;
//	}
//	subset.indices.push_back(gIndex);
//}

void GraphState::addToSubset(unsigned int subsetIndex, const lib::Graph::Single *g) {
	unsigned int gIndex = addUniverseGetIndex(g);

	SubsetStore::iterator iter = subsets.find(subsetIndex);
	if(iter == subsets.end()) iter = subsets.insert(SubsetStore::value_type(subsetIndex, Subset(*this))).first;
	Subset &subset = iter->second;

	for(unsigned int index : subset.indices) {
		if(index == gIndex) return;
	}
	subset.indices.push_back(gIndex);
}

void GraphState::addToUniverse(const Graph::Single *g) {
	addUniverseGetIndex(g);
}

bool GraphState::hasSubset(unsigned int i) const {
	return subsets.find(i) != subsets.end();
}

const GraphState::Subset &GraphState::getSubset(unsigned int i) const {
	if(!hasSubset(i)) {
		IO::log() << "WTF: hasSubset(" << i << ") == false" << std::endl;
		IO::log() << "WTF: numSubsets: " << subsets.size() << std::endl;
	}
	assert(hasSubset(i));
	return subsets.find(i)->second;
}

const GraphState::SubsetStore &GraphState::getSubsets() const {
	return subsets;
}

const GraphState::GraphList &GraphState::getUniverse() const {
	return universe;
}

bool GraphState::isInUniverse(const lib::Graph::Single *g) const {
	for(unsigned int i = 0; i < universe.size(); i++) {
		if(universe[i] == g) return true;
	}
	return false;
}

bool operator==(const GraphState &a, const GraphState &b) {
	typedef GraphState::SubsetStore::const_iterator Iter;
	{// check sizes
		if(a.universe.size() != b.universe.size()) return false;
		if(a.subsets.size() != b.subsets.size()) return false;
		Iter ai, ae, bi, be;
		for(ai = a.subsets.begin(), ae = a.subsets.end(),
				bi = b.subsets.begin(), be = b.subsets.end();
				ai != ae && bi != be; ai++, bi++) {
			if(ai->first != bi->first) return false;
			if(ai->second.size() != bi->second.size()) return false;
		}
	}
	GraphState aCopy(a), bCopy(b);
	std::less<const Graph::Single*> comp;
	aCopy.sortUniverse(comp);
	bCopy.sortUniverse(comp);
	for(unsigned int i = 0; i < a.universe.size(); i++) {
		if(aCopy.universe[i] != bCopy.universe[i]) return false;
	}
	Iter ai, ae, bi, be;
	for(ai = aCopy.subsets.begin(), ae = aCopy.subsets.end(),
			bi = bCopy.subsets.begin(), be = bCopy.subsets.end();
			ai != ae && bi != be; ai++, bi++) {
		aCopy.sortSubset(ai->first, comp);
		bCopy.sortSubset(bi->first, comp);
		for(unsigned int i = 0; i < ai->second.size(); i++) {
			if(*(ai->second.begin() + i) != *(bi->second.begin() + i)) return false;
		}
	}
	return true;
}

unsigned int GraphState::addUniverseGetIndex(const lib::Graph::Single *g) {
	for(unsigned int i = 0; i < universe.size(); i++) {
		if(universe[i] == g) return i;
	}
	universe.push_back(g);
	return universe.size() - 1;
}

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod