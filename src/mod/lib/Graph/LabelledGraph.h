#ifndef MOD_LIB_GRAPH_LABELLED_GRAPH_H
#define MOD_LIB_GRAPH_LABELLED_GRAPH_H

#include <mod/lib/Graph/Properties/String.h>

#include <jla_boost/graph/morphism/Finder.hpp>

#include <cassert>

namespace mod {
namespace lib {
namespace Graph {

struct LabelledGraph { // models a mod::lib::LabelledGraphConcept
	using GraphType = lib::Graph::GraphType;
	using PropStringType = PropString;
public:

	LabelledGraph(std::unique_ptr<GraphType> g, std::unique_ptr<PropStringType> pString)
	: g(std::move(g)), pString(std::move(pString)) {
		assert(this->g);
		assert(this->pString);
	}

	friend const GraphType &get_graph(const LabelledGraph &g) {
		return *g.g;
	}

	friend const PropStringType &get_string(const LabelledGraph &g) {
		assert(g.pString);
		return *g.pString;
	}
public:

	friend const std::vector<typename boost::graph_traits<GraphType>::vertex_descriptor>&
	get_vertex_order(const LabelledGraph &g) {
		if(g.vertex_order.empty()) {
			g.vertex_order = get_vertex_order(jla_boost::GraphMorphism::DefaultFinderArgsProvider(), get_graph(g));
		}
		return g.vertex_order;
	}
private:
	std::unique_ptr<GraphType> g;
	std::unique_ptr<PropStringType> pString;
private:
	mutable std::vector<typename boost::graph_traits<GraphType>::vertex_descriptor> vertex_order;
};

using PropStringType = LabelledGraph::PropStringType;

} // namespace Graph
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPH_LABELLED_GRAPH_H */
