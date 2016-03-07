#ifndef JLA_BOOST_GRAPH_MORPHISM_FINDER_HPP
#define JLA_BOOST_GRAPH_MORPHISM_FINDER_HPP

#include <jla_boost/graph/vf2_sub_graph_iso.hpp>

namespace jla_boost {
namespace GraphMorphism {

struct DefaultFinderArgsProvider {

	template<typename Graph>
	friend std::vector<typename boost::graph_traits<Graph>::vertex_descriptor>
	get_vertex_order(const DefaultFinderArgsProvider&, const Graph &g) {
		return vertex_order_by_mult(g);
	}
};

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_FINDER_HPP */