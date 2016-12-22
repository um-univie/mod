#include "LabelledGraph.h"

#include <mod/lib/Graph/Properties/Molecule.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/GraphMorphism/Finder.hpp>

#include <cassert>

namespace mod {
namespace lib {
namespace Graph {

LabelledGraph::LabelledGraph(std::unique_ptr<GraphType> g, std::unique_ptr<PropStringType> pString)
: g(std::move(g)), pString(std::move(pString)) {
	assert(this->g);
	assert(this->pString);
	this->pString->verify(this->g.get());
}

LabelledGraph::LabelledGraph(const LabelledGraph &other) {
	g = std::make_unique<GraphType>(get_graph(other));
	pString = std::make_unique<PropStringType>(get_string(other), *g);
}

LabelledGraph::~LabelledGraph() { }

GraphType &get_graph(LabelledGraph &g) {
	return *g.g;
}

const GraphType &get_graph(const LabelledGraph &g) {
	return *g.g;
}

LabelledGraph::PropStringType &get_string(LabelledGraph &g) {
	assert(g.pString);
	return *g.pString;
}

const LabelledGraph::PropStringType &get_string(const LabelledGraph &g) {
	assert(g.pString);
	return *g.pString;
}

const LabelledGraph::PropMoleculeType &get_molecule(const LabelledGraph &g) {
	if(!g.pMolecule) g.pMolecule.reset(new LabelledGraph::PropMoleculeType(get_graph(g), get_string(g)));
	return *g.pMolecule;
}

const std::vector<typename boost::graph_traits<GraphType>::vertex_descriptor>&
get_vertex_order(const LabelledGraph &g) {
	if(g.vertex_order.empty()) {
		g.vertex_order = get_vertex_order(lib::GraphMorphism::DefaultFinderArgsProvider(), get_graph(g));
	}
	return g.vertex_order;
}

} // namespace Graph
} // namespace lib
} // namespace mod
