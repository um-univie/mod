#ifndef JLA_BOOST_GRAPH_DPO_FILTERED_GRAPH_PROJECTION_HPP
#define JLA_BOOST_GRAPH_DPO_FILTERED_GRAPH_PROJECTION_HPP

#include <jla_boost/graph/AdaptorTraits.hpp>
#include <jla_boost/graph/dpo/Rule.hpp>
#include <jla_boost/graph/morphism/VertexMap.hpp>

#include <boost/graph/graph_traits.hpp>

namespace jla_boost {
namespace GraphDPO {

template<typename GraphType>
struct FilteredGraphProjection {
	using Self = FilteredGraphProjection;

	struct Filter {

		Filter() : gInner(nullptr) { }

		Filter(const GraphType &gInner, Membership membership) : gInner(&gInner), membership(membership) { }

		bool operator()(typename boost::graph_traits<GraphType>::vertex_descriptor v) const {
			assert(gInner);
			auto m = (*gInner)[v].membership;
			return m == membership || m == Membership::Context;
		}

		bool operator()(typename boost::graph_traits<GraphType>::edge_descriptor e) const {
			assert(gInner);
			auto m = (*gInner)[e].membership;
			return m == membership || m == Membership::Context;
		}
	private:
		const GraphType *gInner;
		Membership membership;
	};

	using GraphTypeFiltered = boost::filtered_graph<GraphType, Filter, Filter>;
public:

	FilteredGraphProjection(const GraphType &gInner, Membership membership)
	: gInner(gInner), membership(membership), g(gInner, Filter(gInner, membership), Filter(gInner, membership)) { }
public: // Graph
	using vertex_descriptor = typename boost::graph_traits<GraphTypeFiltered>::vertex_descriptor;
	using edge_descriptor = typename boost::graph_traits<GraphTypeFiltered>::edge_descriptor;
	using directed_category = typename boost::graph_traits<GraphTypeFiltered>::directed_category;
	using edge_parallel_category = typename boost::graph_traits<GraphTypeFiltered>::edge_parallel_category;
	using traversal_category = typename boost::graph_traits<GraphTypeFiltered>::traversal_category;

	static vertex_descriptor null_vertex() {
		return GraphType::null_vertex();
	}
public: // IncidenceGraph
	using out_edge_iterator = typename boost::graph_traits<GraphTypeFiltered>::out_edge_iterator;
	using degree_size_type = typename boost::graph_traits<GraphTypeFiltered>::degree_size_type;

	friend std::pair<out_edge_iterator, out_edge_iterator> out_edges(vertex_descriptor v, const Self &g) {
		return out_edges(v, g.g);
	}

	friend vertex_descriptor source(edge_descriptor e, const Self &g) {
		return source(e, g.g);
	}

	friend vertex_descriptor target(edge_descriptor e, const Self &g) {
		return target(e, g.g);
	}

	friend degree_size_type out_degree(vertex_descriptor v, const Self &g) {
		return out_degree(v, g.g);
	}
public: // BidirectionalGraph
	using in_edge_iterator = typename boost::graph_traits<GraphTypeFiltered>::in_edge_iterator;

	friend std::pair<in_edge_iterator, in_edge_iterator> in_edges(vertex_descriptor v, const Self &g) {
		return in_edges(v, g.g);
	}

	friend degree_size_type in_degree(vertex_descriptor v, const Self &g) {
		return in_degree(v, g.g);
	}

	friend degree_size_type degree(vertex_descriptor v, const Self &g) {
		return degree(v, g.g);
	}
public:// AdjacencyGraph
	using adjacency_iterator = typename boost::graph_traits<GraphTypeFiltered>::adjacency_iterator;

	friend std::pair<adjacency_iterator, adjacency_iterator> adjacent_vertices(vertex_descriptor v, const Self &g) {
		return adjacent_vertices(v, g.g);
	}
public: // VertexListGraph
	using vertex_iterator = typename boost::graph_traits<GraphTypeFiltered>::vertex_iterator;
	using vertices_size_type = typename boost::graph_traits<GraphTypeFiltered>::vertices_size_type;

	friend std::pair<vertex_iterator, vertex_iterator> vertices(const Self &g) {
		return vertices(g.g);
	}

	friend vertices_size_type num_vertices(const Self &g) {
		return num_vertices(g.g);
	}
public: // EdgeListGraph
	using edge_iterator = typename boost::graph_traits<GraphTypeFiltered>::edge_iterator;
	using edges_size_type = typename boost::graph_traits<GraphTypeFiltered>::edges_size_type;

	friend std::pair<edge_iterator, edge_iterator> edges(const Self &g) {
		return edges(g.g);
	}

	friend edges_size_type num_edges(const Self &g) {
		return num_edges(g.g);
	}
public: // "AdjacencyMatrix" (it's not constant time)

	friend std::pair<edge_descriptor, bool> edge(vertex_descriptor u, vertex_descriptor v, const Self &g) {
		return edge(u, v, g.g);
	}
public: // PropertyGraph

	template<typename PropertyTag>
	friend decltype(auto) get(PropertyTag t, const Self &g) {
		return get(t, g.g);
	}

	template<typename PropertyTag, typename VertexOrEdge>
	friend decltype(auto) get(PropertyTag t, const Self &g, VertexOrEdge ve) {
		return get(t, g.g, ve);
	}

	template<typename PropertyTag, typename VertexOrEdge, typename Value>
	friend void put(PropertyTag t, Self &g, VertexOrEdge ve, Value &&v) {
		put(t, g.g, ve, std::forward<Value>(v));
	}

	template<typename VertexOrEdge>
	decltype(auto) operator[](VertexOrEdge ve) {
		return g[ve];
	}

	template<typename VertexOrEdge>
	decltype(auto) operator[](VertexOrEdge ve) const {
		return g[ve];
	}
public: // Other

	friend vertex_descriptor vertex(vertices_size_type n, const Self &g) {
		return vertex(n, g.g);
	}
public:
	const GraphType &gInner;
	const Membership membership;
	const GraphTypeFiltered g;
};

} // namespace GraphDPO

template<typename GraphType>
struct GraphAdaptorTraits<GraphDPO::FilteredGraphProjection<GraphType> > {
	using type = GraphType;

	static const type &unwrap(const GraphDPO::FilteredGraphProjection<GraphType> &g) {
		return g.gInner;
	}
};

} // namespace jla_boost
namespace boost {

// PropertyGraph
//------------------------------------------------------------------------------

template<typename GraphType, typename Property>
struct property_map<jla_boost::GraphDPO::FilteredGraphProjection<GraphType>, Property>
: property_map<GraphType, Property> {
};

template<typename GraphType, typename Property>
struct property_map<const jla_boost::GraphDPO::FilteredGraphProjection<GraphType>, Property>
: property_map<GraphType, Property> {
};

} // namespace boost

#endif /* JLA_BOOST_GRAPH_DPO_FILTERED_GRAPH_PROJECTION_HPP */