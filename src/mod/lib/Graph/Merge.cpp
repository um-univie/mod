#include "Merge.h"

#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/Graph/Properties/Molecule.h>
#include <mod/lib/IO/IO.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <algorithm>
#include <set>
#include <ostream>

namespace mod {
namespace lib {
namespace Graph {

Merge::Merge() : locked(false) { }

Merge::~Merge() { }

Merge *Merge::clone() const {
	Merge *res = new Merge();
	for(const Single *g : getSingles()) res->mergeWith(*g);
	return res;
}

const GraphType &Merge::getGraph() const {
	calcCache();
	assert(graphBoost);
	return *graphBoost;
}

const PropString &Merge::getStringState() const {
	calcCache();
	assert(pString);
	return *pString;
}

const PropMolecule &Merge::getMoleculeState() const {
	calcCache();
	assert(moleculeState);
	return *moleculeState;
}

void Merge::printName(std::ostream& s) const {
	s << "{ ";
	for(const Single *g : getSingles()) {
		s << "'";
		g->printName(s);
		s << "' ";
	}
	s << "}";
}

void Merge::mergeWith(const Single &g) {
	assert(!locked);
	singles.insert(&g);
	clearCache();
}

void Merge::lock() {
	assert(!locked);
	locked = true;
}

bool Merge::isLocked() const {
	return locked;
}

void Merge::clearCache() const {
	graphBoost.release();
	pString.release();
}

void Merge::calcCache() const {
	assert(getSingles().size() > 0);
	if(graphBoost) return;
	graphBoost.reset(new GraphType());
	pString.reset(new PropString(*graphBoost));
	for(const Graph::Single *g : getSingles()) doMerge(*g);
	moleculeState.reset(new PropMolecule(*graphBoost, *pString));
}

void Merge::doMerge(const Single& src) const {
	const GraphType &sourceBoost = src.getGraph();
	const PropString &sourceLabelState = src.getStringState();
	Vertex map[num_vertices(sourceBoost)];

	for(Vertex v : asRange(vertices(sourceBoost))) {
		unsigned int index = get(boost::vertex_index_t(), sourceBoost, v);
		map[index] = add_vertex(*graphBoost);
		pString->addVertex(map[index], sourceLabelState[v]);
	}

	for(Edge e : asRange(edges(sourceBoost))) {
		unsigned int from = get(boost::vertex_index_t(), sourceBoost, source(e, sourceBoost));
		unsigned int to = get(boost::vertex_index_t(), sourceBoost, target(e, sourceBoost));
		Edge eAdded = add_edge(map[from], map[to], *graphBoost).first;
		pString->addEdge(eAdded, sourceLabelState[e]);
	}
}

//------------------------------------------------------------------------------
// Static stuff
//------------------------------------------------------------------------------

bool Merge::equal(const Merge &g1, const Merge &g2) {
	if(g1.getSingles().size() != g2.getSingles().size()) return false;
	const std::multiset<const Single*, Less> &v1 = g1.getSingles();
	const std::multiset<const Single*, Less> &v2 = g2.getSingles();
	return std::equal(v1.begin(), v1.end(), v2.begin());
}

bool equal(const Merge *g1, const Merge *g2) {
	return Merge::equal(*g1, *g2);
}

//------------------------------------------------------------------------------
// Compare Functor
//------------------------------------------------------------------------------

bool MergeLess::operator ()(const Merge* g1, const Merge* g2) const {
	assert(g1);
	assert(g2);
	unsigned int size1 = g1->getSingles().size();
	unsigned int size2 = g2->getSingles().size();
	if(size1 != size2) return size1 < size2;
	std::multiset<const Single*, Less>::const_iterator i1, i1end, i2, i2end;
	i1 = g1->getSingles().begin();
	i1end = g1->getSingles().end();
	i2 = g2->getSingles().begin();
	i2end = g2->getSingles().end();
	for(; i1 != i1end; i1++, i2++) {
		const Single *s1 = *i1;
		const Single *s2 = *i2;
		if(s1->getId() != s2->getId()) return s1->getId() < s2->getId();
	}
	return false;
}

} // namespace Graph
} // namespace lib
} // namespace mod
