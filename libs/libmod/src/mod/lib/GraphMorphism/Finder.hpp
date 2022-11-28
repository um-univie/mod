#ifndef MOD_LIB_GRAPH_MORPHISM_FINDER_HPP
#define MOD_LIB_GRAPH_MORPHISM_FINDER_HPP

#include <jla_boost/graph/morphism/VertexOrderByMult.hpp>

namespace mod::lib::GraphMorphism {

struct DefaultFinderArgsProvider {
	template<typename Graph>
	friend std::vector<typename boost::graph_traits<Graph>::vertex_descriptor>
	get_vertex_order(const DefaultFinderArgsProvider &, const Graph &g) {
		return jla_boost::GraphMorphism::vertex_order_by_mult(g);
	}
};

} // namespace mod::lib::GraphMorphism

#endif // MOD_LIB_GRAPH_MORPHISM_FINDER_HPP