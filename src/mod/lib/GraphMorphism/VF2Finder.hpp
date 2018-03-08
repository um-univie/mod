#ifndef MOD_LIB_GRAPH_MORPHISM_VF2_HPP
#define MOD_LIB_GRAPH_MORPHISM_VF2_HPP

#include <mod/lib/GraphMorphism/Finder.hpp>

#include <jla_boost/graph/morphism/finders/vf2.hpp>

namespace mod {
namespace lib {
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
		return jla_boost::GraphMorphism::vf2_graph_iso(gDomain, gCodomain, mr,
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
		return jla_boost::GraphMorphism::vf2_subgraph_mono(gDomain, gCodomain, mr,
				get(boost::vertex_index_t(), gDomain), get(boost::vertex_index_t(), gCodomain),
				vOrderDomain, edgePred, vertexPred);
	}

	template<typename GraphDomain, typename GraphCodomain, typename MR, typename EdgePredicate, typename VertexPredicate>
	bool operator()(const GraphDomain &gDomain, const GraphCodomain &gCodomain, MR mr, EdgePredicate edgePred, VertexPredicate vertexPred) {
		return (*this)(gDomain, gCodomain, mr, edgePred, vertexPred, DefaultFinderArgsProvider(), DefaultFinderArgsProvider());
	}
};

} // namespace GraphMorphism
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPH_MORPHISM_VF2_HPP */