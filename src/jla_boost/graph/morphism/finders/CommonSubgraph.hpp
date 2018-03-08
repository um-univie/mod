#ifndef JLA_BOOST_GRAPH_MORPHOSM_FINDERS_COMMON_SUBGRAPH_HPP
#define JLA_BOOST_GRAPH_MORPHOSM_FINDERS_COMMON_SUBGRAPH_HPP

// Adapted from Boost.Graph mcgregor_common_subgraph

#include <jla_boost/graph/morphism/finders/InjectiveEnumeration.hpp>

#include <jla_boost/Functional.hpp>
#include <jla_boost/graph/PairToRangeAdaptor.hpp>
#include <jla_boost/graph/morphism/PropertyTags.hpp>
#include <jla_boost/graph/morphism/models/InvertibleAdaptor.hpp>
#include <jla_boost/graph/morphism/models/PropertyVertexMap.hpp>

#include <boost/make_shared.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/shared_array_property_map.hpp>

#include <algorithm>
#include <vector>
#include <stack>

namespace jla_boost {
namespace GraphMorphism {
using namespace boost; // TODO: remvoe

template<typename GraphLeft, typename GraphRight, typename EdgePred, typename VertexPred>
struct CommonSubgraphEnumerator : InjectiveEnumerationState<
CommonSubgraphEnumerator<GraphLeft, GraphRight, EdgePred, VertexPred>,
GraphLeft, GraphRight, EdgePred, VertexPred> {
	using Self = CommonSubgraphEnumerator;
	using Base = InjectiveEnumerationState<Self, GraphLeft, GraphRight, EdgePred, VertexPred>;
public:
	using VertexLeft = typename boost::graph_traits<GraphLeft>::vertex_descriptor;
	using VertexRight = typename boost::graph_traits<GraphRight>::vertex_descriptor;
	using EdgeLeft = typename boost::graph_traits<GraphLeft>::edge_descriptor;
	using EdgeRight = typename boost::graph_traits<GraphRight>::edge_descriptor;
private:

	static VertexLeft vNullLeft() {
		return boost::graph_traits<GraphLeft>::null_vertex();
	}

	static VertexRight vNullRight() {
		return boost::graph_traits<GraphRight>::null_vertex();
	}
public:

	CommonSubgraphEnumerator(const GraphLeft &gLeft, const GraphRight &gRight,
			EdgePred edgePred, VertexPred vertexPred, bool onlyConnected)
	: Base(gLeft, gRight, edgePred, vertexPred),
	onlyConnected(onlyConnected) { }
public:

	bool visit_tryPush(VertexLeft vLeft, VertexRight vRight) {
		if(this->getStackSize() == 0) return true;
		bool has_one_edge = false;

		// Verify edges with existing sub-graph
		for(auto vOtherLeft : asRange(vertices(this->gLeft))) {
			auto vOtherRight = this->rightFromLeft(vOtherLeft);
			// Skip unassociated vertices
			if(vOtherRight == vNullRight()) continue;

			// NOTE: This will not work with parallel edges, since the
			// first matching edge is always chosen.
			EdgeLeft edge_to_new1, edge_from_new1;
			bool edge_to_new_exists1 = false, edge_from_new_exists1 = false;

			EdgeRight edge_to_new2, edge_from_new2;
			bool edge_to_new_exists2 = false, edge_from_new_exists2 = false;

			// Search for edge from existing to new vertex (gLeft)
			for(auto eOutLeft : asRange(out_edges(vOtherLeft, this->gLeft))) {
				if(target(eOutLeft, this->gLeft) == vLeft) {
					edge_to_new1 = eOutLeft;
					edge_to_new_exists1 = true;
					break;
				}
			}

			// Search for edge from existing to new vertex (gRight)
			for(auto eOutRight : asRange(out_edges(vOtherRight, this->gRight))) {
				if(target(eOutRight, this->gRight) == vRight) {
					edge_to_new2 = eOutRight;
					edge_to_new_exists2 = true;
					break;
				}
			}

			bool is_undirected1 = is_undirected(this->gLeft),
					is_undirected2 = is_undirected(this->gRight);

			if(is_undirected1 && is_undirected2) {
				// Edge in both graphs exists and both graphs are undirected
				if(edge_to_new_exists1 && edge_to_new_exists2) {
					has_one_edge = true;
				}
				continue;
			} else {
				if(!is_undirected1) {
					// Search for edge from new to existing vertex (gLeft)
					for(auto eOutLeft : asRange(out_edges(vLeft, this->gLeft))) {
						if(target(eOutLeft, this->gLeft) == vOtherLeft) {
							edge_from_new1 = eOutLeft;
							edge_from_new_exists1 = true;
							break;
						}
					}
				}

				if(!is_undirected2) {
					// Search for edge from new to existing vertex (gRight)
					for(auto eOutRight : asRange(out_edges(vRight, this->gRight))) {
						if(target(eOutRight, this->gRight) == vOtherRight) {
							edge_from_new2 = eOutRight;
							edge_from_new_exists2 = true;
							break;
						}
					}
				}

				// Make sure edges from new to existing vertices are equivalent
				//			if(edge_from_new_exists1 != edge_from_new_exists2) return false; // here is the induced part
				if(edge_from_new_exists1 && edge_from_new_exists2) {
					if(!this->edgePred(edge_from_new1, edge_from_new2)) {
						//						std::cout << "\tfailed: edgePred\n";
						return false;
					}
				}

				if((edge_from_new_exists1 && edge_from_new_exists2) ||
						(edge_to_new_exists1 && edge_to_new_exists2)) {
					has_one_edge = true;
				}
			} // else
		} // BGL_FORALL_VERTICES_T

		// Make sure new vertices are connected to the existing subgraph
		if(onlyConnected && !has_one_edge) {
			//			std::cout << "\tfailed: onlyConnected\n";
			return false;
		}
		return true;
	}
public:

	// Recursive method that does a depth-first search in the space of
	// potential subgraphs.  At each level, every new vertex pair from
	// both graphs is tested to see if it can extend the current
	// subgraph.  If so, the subgraph is output to subgraph_callback
	// in the form of two correspondence maps (one for each graph).
	// Returning false from subgraph_callback will terminate the
	// search.

	template<typename Callback>
	bool operator()(Callback &&callback) {
		auto vsLeft = asRange(vertices(this->gLeft));
		auto vsRight = asRange(vertices(this->gRight));

		// Iterate until all vertices have been visited
		for(auto vLeft : vsLeft) {
			// Skip already matched vertices in first graph
			if(this->rightFromLeft(vLeft) != vNullRight()) continue;

			for(auto vRight : vsRight) {
				// Skip already matched vertices in second graph
				if(this->leftFromRight(vRight) != vNullLeft()) continue;

				// Check if current sub-graph can be extended with the matched vertex pair
				bool wasPushed = this->tryPush(vLeft, vRight);
				if(!wasPushed) continue;

				// Returning false from the callback will cancel iteration
				auto mSized = addProp(this->getVertexMap(), PreImageSizeT(), this->getStackSize());
				if(!callback(std::move(mSized), this->gLeft, this->gRight)) {
					return false;
				}

				// Depth-first search into the state space of possible sub-graphs
				bool continueSearch = (*this)(callback);
				if(!continueSearch) return false;

				VertexLeft vStackLeft;
				VertexRight vStackRight;
				std::tie(vStackLeft, vStackRight) = this->pop();
				assert(vStackLeft == vLeft);
				assert(vStackRight == vRight);
			} // for all vertices(gRight)
		} // for all vertices(gLeft)
		return true;
	}
private: // args
	const bool onlyConnected;
};

template<typename GraphLeft, typename GraphRight, typename EdgePred, typename VertexPred>
auto makeCommonSubgraphEnumerator(const GraphLeft &gLeft, const GraphRight &gRight,
		EdgePred edgePred, VertexPred vertexPred, bool onlyConnected) {
	return CommonSubgraphEnumerator<GraphLeft, GraphRight, EdgePred, VertexPred>(gLeft, gRight, edgePred, vertexPred, onlyConnected);
}

// ==========================================================================

// Enumerates all common subgraphs present in gLeft and gRight.
// Continues until the search space has been fully explored or false
// is returned from callback.

template<typename GraphLeft, typename GraphRight, typename EdgePred, typename VertexPred, typename Callback>
void commonSubgraphs(const GraphLeft &gLeft, const GraphRight &gRight,
		EdgePred edgePred, VertexPred vertexPred, bool onlyConnected, Callback callback) {
	CommonSubgraphEnumerator<GraphLeft, GraphRight, EdgePred, VertexPred>
			(gLeft, gRight, edgePred, vertexPred, onlyConnected)(callback);
}

// Variant of commonSubgraphs with all default parameters

template<typename GraphLeft, typename GraphRight, typename Callback>
void commonSubgraphs(const GraphLeft &gLeft, const GraphRight &gRight, bool onlyConnected, Callback callback) {
	commonSubgraphs(gLeft, gRight, get(boost::vertex_index, gLeft), get(boost::vertex_index, gRight),
			jla_boost::AlwaysTrue(), jla_boost::AlwaysTrue(), onlyConnected, callback);
}

// Named parameter variant of commonSubgraphs

template<typename GraphLeft, typename GraphRight, typename Callback, typename Param, typename Tag, typename Rest>
void commonSubgraphs(const GraphLeft &gLeft, const GraphRight &gRight, bool onlyConnected, Callback callback,
		const boost::bgl_named_params<Param, Tag, Rest> &params) {
	commonSubgraphs(gLeft, gRight,
			choose_param(get_param(params, boost::edges_equivalent_t()), jla_boost::AlwaysTrue()),
			choose_param(get_param(params, boost::vertices_equivalent_t()), jla_boost::AlwaysTrue()),
			onlyConnected, callback);
}

// ==========================================================================

namespace detail {

// Helper class for caching maximum and/or unique subgraphs.

template<typename GraphLeft, typename GraphRight, typename Next, bool Unique, bool Maximum>
struct CommonSubgraphCallbackHelper {
	static_assert(Unique || Maximum, "Not intended for just caching.");
	using CachedVertexMap = InvertibleVectorVertexMap<GraphLeft, GraphRight>;

	struct CachedSubgraph {

		template<typename VertexMap>
		CachedSubgraph(VertexMap &&m, const GraphLeft &gLeft, const GraphRight &gRight, std::size_t size)
		: m(std::forward<VertexMap>(m), gLeft, gRight), size(size) { }
	public:
		CachedVertexMap m;
		std::size_t size;
	};
private:
	CommonSubgraphCallbackHelper(const CommonSubgraphCallbackHelper&) = delete;
	CommonSubgraphCallbackHelper &operator=(const CommonSubgraphCallbackHelper&) = delete;
public:

	CommonSubgraphCallbackHelper(const GraphLeft &gLeft, const GraphRight &gRight, Next next)
	: gLeft(gLeft), gRight(gRight), next(next) { }

	template<typename VertexMap>
	bool operator()(VertexMap &&m, const GraphLeft &gLeft, const GraphRight &gRight) {
		auto subgraphSize = get_prop(PreImageSizeT(), m);
		if(Maximum) {
			if(!cache.empty()) {
				if(subgraphSize > cache.front().size) cache.clear();
				else if(subgraphSize < cache.front().size) return true;
			}
		}
		if(Unique) {
			auto mAsProperty = makeAsPropertyMap(std::ref(m), gLeft, gRight);
			for(const auto &c : cache) {
				if(subgraphSize != c.size) continue;
				auto cAsProperty = makeAsPropertyMap(std::ref(c.m), gLeft, gRight);
				if(!are_property_maps_different(mAsProperty, cAsProperty, gLeft)) {
					return true;
				}
			}
		}
		cache.emplace_back(m, gLeft, gRight, subgraphSize);
		if(Maximum) return true;
		else return next(m, gLeft, gRight);
	}

	void outputSubgraphs() {
		static_assert(Maximum, "Otherwise they are reported online.");
		for(auto &subgraph : cache)
			next(addProp(std::move(subgraph.m), PreImageSizeT(), subgraph.size), gLeft, gRight);
	}
private:
	const GraphLeft &gLeft;
	const GraphLeft &gRight;
	Next next;
	std::vector<CachedSubgraph> cache;
};

template<typename GraphLeft, typename GraphRight, typename Next>
using UniqueVertexMapCallback = CommonSubgraphCallbackHelper<GraphLeft, GraphRight, Next, true, false>;
template<typename GraphLeft, typename GraphRight, typename Next>
using MaximumSubgraphCallback = CommonSubgraphCallbackHelper<GraphLeft, GraphRight, Next, false, true>;
template<typename GraphLeft, typename GraphRight, typename Next>
using UniqueMaximumSubgraphCallback = CommonSubgraphCallbackHelper<GraphLeft, GraphRight, Next, true, true>;

} // namespace detail

// Enumerates all unique common subgraphs between gLeft and gRight.
// The user callback is invoked for each unique subgraph as they are
// discovered.

template<typename GraphLeft, typename GraphRight, typename EdgePred, typename VertexPred, typename Callback>
void commonSubgraphs_unique(const GraphLeft &gLeft, const GraphRight &gRight,
		EdgePred edgePred, VertexPred vertexPred, bool onlyConnected, Callback callback) {
	detail::UniqueVertexMapCallback<GraphLeft, GraphRight, Callback> unique_callback(gLeft, gRight, callback);
	commonSubgraphs(gLeft, gRight, edgePred, vertexPred, onlyConnected, std::ref(unique_callback));
}

// Variant of commonSubgraphs_unique with all default parameters.

template<typename GraphLeft, typename GraphRight, typename Callback>
void commonSubgraphs_unique(const GraphLeft &gLeft, const GraphRight &gRight, bool onlyConnected, Callback callback) {
	commonSubgraphs_unique(gLeft, gRight, jla_boost::AlwaysTrue(), jla_boost::AlwaysTrue(), onlyConnected, callback);
}

// Named parameter variant of commonSubgraphs_unique

template <typename GraphLeft, typename GraphRight, typename Callback, typename Param, typename Tag, typename Rest>
void commonSubgraphs_unique(const GraphLeft &gLeft, const GraphRight &gRight, bool onlyConnected, Callback callback,
		const boost::bgl_named_params<Param, Tag, Rest> &params) {
	commonSubgraphs_unique(gLeft, gRight,
			choose_param(get_param(params, boost::edges_equivalent_t()), jla_boost::AlwaysTrue()),
			choose_param(get_param(params, boost::vertices_equivalent_t()), jla_boost::AlwaysTrue()),
			onlyConnected, callback);
}

// ==========================================================================

// Enumerates the largest common subgraphs found between gLeft and gRight
// Note that the ENTIRE search space is explored before callback is actually invoked.

template<typename GraphLeft, typename GraphRight, typename EdgePred, typename VertexPred, typename Callback>
void commonSubgraphs_maximum(const GraphLeft &gLeft, const GraphRight &gRight,
		EdgePred edgePred, VertexPred vertexPred, bool onlyConnected, Callback callback) {
	detail::MaximumSubgraphCallback<GraphLeft, GraphRight, Callback> max_interceptor(gLeft, gRight, callback);
	commonSubgraphs(gLeft, gRight, edgePred, vertexPred, onlyConnected, std::ref(max_interceptor));
	// Only output the largest subgraphs
	max_interceptor.outputSubgraphs();
}

// Variant of commonSubgraphs_maximum with all default
// parameters.

template <typename GraphLeft, typename GraphRight, typename Callback>
void commonSubgraphs_maximum(const GraphLeft &gLeft, const GraphRight &gRight, bool onlyConnected, Callback callback) {
	commonSubgraphs_maximum(gLeft, gRight, jla_boost::AlwaysTrue(), jla_boost::AlwaysTrue(), onlyConnected, callback);
}

// Named parameter variant of commonSubgraphs_maximum

template <typename GraphLeft, typename GraphRight, typename Callback, typename Param, typename Tag, typename Rest>
void commonSubgraphs_maximum(const GraphLeft &gLeft, const GraphRight &gRight, bool onlyConnected, Callback callback,
		const boost::bgl_named_params<Param, Tag, Rest> &params) {
	commonSubgraphs_maximum(gLeft, gRight,
			choose_param(get_param(params, boost::edges_equivalent_t()), jla_boost::AlwaysTrue()),
			choose_param(get_param(params, boost::vertices_equivalent_t()), jla_boost::AlwaysTrue()),
			onlyConnected, callback);
}

// ==========================================================================

// Enumerates the largest, unique common subgraphs found between
// gLeft and gRight.  Note that the ENTIRE search space is explored
// before callback is actually invoked.

template<typename GraphLeft, typename GraphRight, typename EdgePred, typename VertexPred, typename Callback>
void commonSubgraphs_maximum_unique(const GraphLeft &gLeft, const GraphRight &gRight,
		EdgePred edgePred, VertexPred vertexPred, bool onlyConnected, Callback callback) {
	detail::UniqueMaximumSubgraphCallback<GraphLeft, GraphRight, Callback> unique_max_interceptor(gLeft, gRight, callback);
	commonSubgraphs(gLeft, gRight, edgePred, vertexPred, onlyConnected, std::ref(unique_max_interceptor));
	// Only output the largest, unique subgraphs
	unique_max_interceptor.outputSubgraphs();
}

// Variant of commonSubgraphs_maximum_unique with all default parameters

template<typename GraphLeft, typename GraphRight, typename Callback>
void commonSubgraphs_maximum_unique(const GraphLeft &gLeft, const GraphRight &gRight, bool onlyConnected, Callback callback) {
	commonSubgraphs_maximum_unique(gLeft, gRight, jla_boost::AlwaysTrue(), jla_boost::AlwaysTrue(), onlyConnected, callback);
}

// Named parameter variant of commonSubgraphs_maximum_unique

template<typename GraphLeft, typename GraphRight, typename Callback, typename Param, typename Tag, typename Rest>
void commonSubgraphs_maximum_unique(const GraphLeft &gLeft, const GraphRight &gRight, bool onlyConnected, Callback callback,
		const boost::bgl_named_params<Param, Tag, Rest> &params) {
	commonSubgraphs_maximum_unique(gLeft, gRight,
			choose_param(get_param(params, boost::edges_equivalent_t()), jla_boost::AlwaysTrue()),
			choose_param(get_param(params, boost::vertices_equivalent_t()), jla_boost::AlwaysTrue()),
			onlyConnected, callback);
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHOSM_FINDERS_COMMON_SUBGRAPH_HPP */