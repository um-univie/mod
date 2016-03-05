#ifndef JLA_BOOST_GRAPH_MORPHISM_VF2_HPP
#define JLA_BOOST_GRAPH_MORPHISM_VF2_HPP

#include <jla_boost/graph/morphism/Finder.hpp>
#include <jla_boost/graph/morphism/MatchAsVertexMap.hpp>

namespace jla_boost {
namespace GraphMorphism {
namespace detail {

template<typename Graph>
void assertSizes(const Graph &g) {
	assert(num_vertices(g) == std::distance(vertices(g).first, vertices(g).second));
	assert(num_edges(g) == std::distance(edges(g).first, edges(g).second));
}

} // namespace detail

struct VF2Isomorphism {

	template<typename GraphDomain, typename GraphCodomain, typename MR, typename EdgePredicate, typename VertexPredicate,
	typename ArgsProviderDomain, typename ArgsProviderCodomain>
	bool operator()(const GraphDomain &gDomain, const GraphCodomain &gCodomain, MR mr, EdgePredicate edgePred, VertexPredicate vertexPred,
			ArgsProviderDomain argsDomain, ArgsProviderCodomain argsCodomain) {
		auto &&vOrderDomain = get_vertex_order(argsDomain, gDomain);
		return vf2_graph_iso(gDomain, gCodomain,
				makeMatchAsVertexMapWrapper(gDomain, gCodomain, mr),
				get(boost::vertex_index_t(), gDomain), get(boost::vertex_index_t(), gCodomain),
				vOrderDomain, edgePred, vertexPred);
	}

	template<typename GraphDomain, typename GraphCodomain, typename MR, typename EdgePredicate, typename VertexPredicate>
	bool operator()(const GraphDomain &gDomain, const GraphCodomain &gCodomain, MR mr, EdgePredicate edgePred, VertexPredicate vertexPred) {
		return (*this)(gDomain, gCodomain, mr, edgePred, vertexPred, DefaultFinderArgsProvider(), DefaultFinderArgsProvider());
	}
};

struct VF2Monomorphism {

	template<typename GraphDomain, typename GraphCodomain, typename MR, typename EdgePredicate, typename VertexPredicate,
	typename ArgsProviderDomain, typename ArgsProviderCodomain>
	bool operator()(const GraphDomain &gDomain, const GraphCodomain &gCodomain, MR mr, EdgePredicate edgePred, VertexPredicate vertexPred,
			ArgsProviderDomain argsDomain, ArgsProviderCodomain argsCodomain) {
		auto &&vOrderDomain = get_vertex_order(argsDomain, gDomain);
		return vf2_subgraph_mono(gDomain, gCodomain,
				makeMatchAsVertexMapWrapper(gDomain, gCodomain, mr),
				get(boost::vertex_index_t(), gDomain), get(boost::vertex_index_t(), gCodomain),
				vOrderDomain, edgePred, vertexPred);
	}

	template<typename GraphDomain, typename GraphCodomain, typename MR, typename EdgePredicate, typename VertexPredicate>
	bool operator()(const GraphDomain &gDomain, const GraphCodomain &gCodomain, MR mr, EdgePredicate edgePred, VertexPredicate vertexPred) {
		return (*this)(gDomain, gCodomain, mr, edgePred, vertexPred, DefaultFinderArgsProvider(), DefaultFinderArgsProvider());
	}
};

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_VF2_HPP */