#ifndef JLA_BOOST_GRAPH_DPO_FILTERED_GRAPH_PROJECTION_HPP
#define JLA_BOOST_GRAPH_DPO_FILTERED_GRAPH_PROJECTION_HPP

#include <jla_boost/graph/FilteredGraph.hpp>
#include <jla_boost/graph/dpo/Rule.hpp>
#include <jla_boost/graph/morphism/VertexMap.hpp>

#include <boost/graph/graph_traits.hpp>

namespace jla_boost {
namespace GraphDPO {

template<typename DPORule>
struct FilteredGraphProjection {
	using GraphType = typename PushoutRuleTraits<DPORule>::GraphType;

	struct Filter {

		Filter() : projection(nullptr) { }

		Filter(const FilteredGraphProjection<DPORule> *projection) : projection(projection) { }

		bool operator()(typename boost::graph_traits<GraphType>::vertex_descriptor v) const {
			assert(projection);
			auto membership = projection->g[v].membership;
			return membership == projection->membership || membership == Membership::Context;
		}

		bool operator()(typename boost::graph_traits<GraphType>::edge_descriptor e) const {
			assert(projection);
			auto membership = projection->g[e].membership;
			return membership == projection->membership || membership == Membership::Context;
		}
	private:
		const FilteredGraphProjection<DPORule> *projection;
	};

	using GraphTypeFiltered = boost::filtered_graph<GraphType, Filter, Filter>;
public:

	FilteredGraphProjection(const GraphType &g, Membership membership) : g(g), membership(membership), graph(g, Filter(this), Filter(this)) { }
public:
	const GraphType &g;
	const Membership membership;
	const GraphTypeFiltered graph;
};

} // namespace GraphDPO
namespace GraphMorphism {

template<typename DPORule>
struct ReinterpreterTraits<GraphDPO::FilteredGraphProjection<DPORule> > {
	using type = typename GraphDPO::PushoutRuleTraits<DPORule>::GraphType;

	static const type &unwrap(const GraphDPO::FilteredGraphProjection<DPORule> &g) {
		return g.g;
	}
};

} // namespace GraphMorphism
namespace GraphDPO {

template<typename DPORule>
struct DPOProjectionVertexIndexMap {
	using GraphType = typename PushoutRuleTraits<DPORule>::GraphType;

	DPOProjectionVertexIndexMap() : projection(nullptr) { }

	DPOProjectionVertexIndexMap(const DPOProjectionVertexIndexMap &other) : projection(other.projection) { }

	explicit DPOProjectionVertexIndexMap(const FilteredGraphProjection<DPORule> *projection) : projection(projection) { }

	typename boost::graph_traits<GraphType>::vertices_size_type
	operator[](typename boost::graph_traits<typename FilteredGraphProjection<DPORule>::GraphTypeFiltered>::vertex_descriptor v) const {
		return get(boost::vertex_index_t(), projection->graph, v);
	}
public:
	const FilteredGraphProjection<DPORule> *projection;
};

} // namespace GraphDPO
} // namespace jla_boost
namespace boost {

template<typename DPORule>
struct graph_traits<jla_boost::GraphDPO::FilteredGraphProjection<DPORule> >
: graph_traits<typename jla_boost::GraphDPO::FilteredGraphProjection<DPORule>::GraphTypeFiltered> {
};

} // namespace boost
namespace jla_boost {
namespace GraphDPO {

// IncidenceGraph
//------------------------------------------------------------------------------

template<typename DPORule>
std::pair<typename boost::graph_traits<FilteredGraphProjection<DPORule> >::out_edge_iterator,
typename boost::graph_traits<FilteredGraphProjection<DPORule> >::out_edge_iterator>
out_edges(typename boost::graph_traits<FilteredGraphProjection<DPORule> >::vertex_descriptor v, const FilteredGraphProjection<DPORule> &g) {
	return out_edges(v, g.graph);
}

template<typename DPORule>
typename boost::graph_traits<FilteredGraphProjection<DPORule> >::vertex_descriptor
source(typename boost::graph_traits<FilteredGraphProjection<DPORule> >::edge_descriptor e, const FilteredGraphProjection<DPORule> &g) {
	return source(e, g.graph);
}

template<typename DPORule>
typename boost::graph_traits<FilteredGraphProjection<DPORule> >::vertex_descriptor
target(typename boost::graph_traits<FilteredGraphProjection<DPORule> >::edge_descriptor e, const FilteredGraphProjection<DPORule> &g) {
	return target(e, g.graph);
}

template<typename DPORule>
typename boost::graph_traits<FilteredGraphProjection<DPORule> >::vertices_size_type
out_degree(typename boost::graph_traits<FilteredGraphProjection<DPORule> >::vertex_descriptor v, const FilteredGraphProjection<DPORule> &g) {
	return out_degree(v, g.graph);
}

// BidirectionalGraph
//------------------------------------------------------------------------------

template<typename DPORule>
std::pair<typename boost::graph_traits<FilteredGraphProjection<DPORule> >::in_edge_iterator,
typename boost::graph_traits<FilteredGraphProjection<DPORule> >::in_edge_iterator>
in_edges(typename boost::graph_traits<FilteredGraphProjection<DPORule> >::vertex_descriptor v, const FilteredGraphProjection<DPORule> &g) {
	return in_edges(v, g.graph);
}

template<typename DPORule>
typename boost::graph_traits<FilteredGraphProjection<DPORule> >::vertices_size_type
in_degree(typename boost::graph_traits<FilteredGraphProjection<DPORule> >::vertex_descriptor v, const FilteredGraphProjection<DPORule> &g) {
	return in_degree(v, g.graph);
}

//template<typename DPORule>
//boost::graph_traits<DPOProjection>::vertices_size_type
//degree(boost::graph_traits<DPOProjection>::vertex_descriptor v, const DPOProjection &g) {
//	return degree(v, g.graph);
//}

// AdjacencyGraph
//------------------------------------------------------------------------------

template<typename DPORule>
std::pair<typename boost::graph_traits<FilteredGraphProjection<DPORule> >::adjacency_iterator,
typename boost::graph_traits<FilteredGraphProjection<DPORule> >::adjacency_iterator>
adjacent_vertices(typename boost::graph_traits<FilteredGraphProjection<DPORule> >::vertex_descriptor v, const FilteredGraphProjection<DPORule> &g) {
	return adjacent_vertices(v, g.graph);
}

// VertexListGraph
//------------------------------------------------------------------------------

template<typename DPORule>
std::pair<typename boost::graph_traits<FilteredGraphProjection<DPORule> >::vertex_iterator,
typename boost::graph_traits<FilteredGraphProjection<DPORule> >::vertex_iterator>
vertices(const FilteredGraphProjection<DPORule> &g) {
	return vertices(g.graph);
}

template<typename DPORule>
typename boost::graph_traits<FilteredGraphProjection<DPORule> >::vertices_size_type
num_vertices(const FilteredGraphProjection<DPORule> &g) {
	return num_vertices(g.graph);
}

// EdgeListGraph
//------------------------------------------------------------------------------

template<typename DPORule>
std::pair<typename boost::graph_traits<FilteredGraphProjection<DPORule> >::edge_iterator,
typename boost::graph_traits<FilteredGraphProjection<DPORule> >::edge_iterator>
edges(const FilteredGraphProjection<DPORule> &g) {
	return edges(g.graph);
}

template<typename DPORule>
typename boost::graph_traits<FilteredGraphProjection<DPORule> >::edges_size_type
num_edges(const FilteredGraphProjection<DPORule> &g) {
	return num_edges(g.graph);
}

// AdjacencyMatrix

template<typename DPORule>
std::pair<typename boost::graph_traits<FilteredGraphProjection<DPORule> >::edge_descriptor, bool>
edge(typename boost::graph_traits<FilteredGraphProjection<DPORule> >::vertex_descriptor u,
		typename boost::graph_traits<FilteredGraphProjection<DPORule> >::vertex_descriptor v,
		const FilteredGraphProjection<DPORule> &g) {
	return edge(u, v, g.graph);
}

} // namespace GraphDPO
} // namespace jla_boost
namespace boost {

// PropertyGraph

template<typename DPORule>
struct property_traits<jla_boost::GraphDPO::DPOProjectionVertexIndexMap<DPORule> > {
	typedef typename graph_traits<jla_boost::GraphDPO::FilteredGraphProjection<DPORule> >::vertices_size_type value_type;
	typedef typename graph_traits<jla_boost::GraphDPO::FilteredGraphProjection<DPORule> >::vertices_size_type reference;
	typedef typename graph_traits<jla_boost::GraphDPO::FilteredGraphProjection<DPORule> >::vertex_descriptor key_type;
	typedef readable_property_map_tag category;
};

template<typename DPORule>
struct vertex_property_type<jla_boost::GraphDPO::FilteredGraphProjection<DPORule> > : boost::no_property {
};

template<typename DPORule>
struct edge_property_type<jla_boost::GraphDPO::FilteredGraphProjection<DPORule> > : boost::no_property {
};

template<typename DPORule>
struct graph_property_type<jla_boost::GraphDPO::FilteredGraphProjection<DPORule> > : boost::no_property {
};

//template<typename Property>
//struct property_map<mod::lib::Rule::DPOProjection, Property> {
//	typedef typename property_map<mod::lib::Rule::DPOProjection::GraphType, Property>::type type;
//	typedef typename property_map<mod::lib::Rule::DPOProjection::GraphType, Property>::const_type const_type;
//};

template<typename DPORule>
struct property_map<jla_boost::GraphDPO::FilteredGraphProjection<DPORule>, boost::vertex_index_t> {
	typedef jla_boost::GraphDPO::DPOProjectionVertexIndexMap<DPORule> type;
	typedef jla_boost::GraphDPO::DPOProjectionVertexIndexMap<DPORule> const_type;
};

//template<>
//struct property_map<mod::lib::Rule::DPOProjection, boost::vertex_name_t> {
//	typedef mod::lib::Rule::DPOProjectionVertexNameMap type;
//	typedef mod::lib::Rule::DPOProjectionVertexNameMap const_type;
//};

//template<>
//struct property_map<mod::lib::Rule::DPOProjection, boost::edge_name_t> {
//	typedef mod::lib::Rule::DPOProjectionEdgeNameMap type;
//	typedef mod::lib::Rule::DPOProjectionEdgeNameMap const_type;
//};

} // namespace boost
namespace jla_boost {
namespace GraphDPO {

template<typename DPORule>
typename boost::property_traits<DPOProjectionVertexIndexMap<DPORule> >::reference
get(const DPOProjectionVertexIndexMap<DPORule> &map, typename boost::property_traits<DPOProjectionVertexIndexMap<DPORule> >::key_type k) {
	return map[k];
}

//template<typename DPORule>
//boost::property_traits<DPOProjectionVertexNameMap>::reference
//get(const DPOProjectionVertexNameMap &map, boost::property_traits<DPOProjectionVertexNameMap>::key_type k) {
//	return map[k];
//}

//template<typename DPORule>
//boost::property_traits<DPOProjectionEdgeNameMap>::reference
//get(const DPOProjectionEdgeNameMap &map, boost::property_traits<DPOProjectionEdgeNameMap>::key_type k) {
//	return map[k];
//}

//template<typename PropertyTag>
//typename boost::property_map<DPOProjection, PropertyTag>::const_type
//get(PropertyTag t, const DPOProjection &g) {
//	return get(t, g.graph);
//}

template<typename DPORule>
DPOProjectionVertexIndexMap<DPORule> get(boost::vertex_index_t t, const FilteredGraphProjection<DPORule> &g) {
	return DPOProjectionVertexIndexMap<DPORule>(&g);
}

//template<typename DPORule>DPOProjectionVertexNameMap get(boost::vertex_name_t t, const DPOProjection &g) {
//	return DPOProjectionVertexNameMap(&g);
//}

//template<typename DPORule>DPOProjectionEdgeNameMap get(boost::edge_name_t t, const DPOProjection &g) {
//	return DPOProjectionEdgeNameMap(&g);
//}

template<typename DPORule, typename PropertyTag, typename VertexOrEdge>
typename boost::property_traits<typename boost::property_map<FilteredGraphProjection<DPORule>, PropertyTag>::const_type>::reference
get(PropertyTag t, const FilteredGraphProjection<DPORule> &g, VertexOrEdge ve) {
	return get(get(t, g), ve);
}

// Other

//template<typename DPORule>boost::graph_traits<DPOProjection>::vertex_descriptor
//vertex(boost::graph_traits<DPOProjection>::vertices_size_type n, const DPOProjection &g) {
//	for(boost::graph_traits<DPOProjection>::vertex_descriptor v : asRange(vertices(g))) {
//		if(get(boost::vertex_index_t(), g, v) == n) return v;
//	}
//	MOD_ABORT;
//}

} // namespace GraphDPO
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_DPO_FILTERED_GRAPH_PROJECTION_HPP */