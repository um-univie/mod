#ifndef MOD_LIB_RULES_CONNECTEDCOMPONENT_H
#define MOD_LIB_RULES_CONNECTEDCOMPONENT_H

#include <boost/graph/graph_traits.hpp>

namespace mod {
namespace lib {
namespace Rules {

template<typename Graph, typename ComponentMap>
struct ConnectedComponentFilter {
	using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;
	using Edge = typename boost::graph_traits<Graph>::edge_descriptor;
	// because filters in boost::filetered_graph must be default constructible

	ConnectedComponentFilter() : g(nullptr), componentMap(nullptr) { }

	ConnectedComponentFilter(const Graph *g, const ComponentMap *componentMap, std::size_t component)
	: g(g), componentMap(componentMap), component(component) { }

	bool operator()(Vertex v) const {
		auto vComponent = (*componentMap)[get(boost::vertex_index_t(), *g, v)];
		return vComponent == component;
	}

	bool operator()(Edge e) const {
		auto vSrcComponent = (*componentMap)[get(boost::vertex_index_t(), *g, source(e, *g))];
		return vSrcComponent == component;
	}
private:
	const Graph *g;
	const ComponentMap *componentMap;
	std::size_t component;
};

} // namespace Rules
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RULES_CONNECTEDCOMPONENT_H */