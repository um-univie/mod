#ifndef JLA_BOOST_UNIONGRAPH_H
#define JLA_BOOST_UNIONGRAPH_H

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <iosfwd>
#include <tuple>
#include <vector>

namespace jla_boost {

template<typename Graph>
struct union_graph {
	using GraphStore = std::vector<const Graph*>;
	using const_iterator = typename GraphStore::const_iterator;

	using base_traits = boost::graph_traits<Graph>;

	// Graph
	//--------------------------------------------------------------------------
	struct vertex_descriptor;
	struct edge_descriptor;
	using directed_category = typename base_traits::directed_category;
	using edge_parallel_category = typename base_traits::edge_parallel_category;
	using traversal_category = typename base_traits::traversal_category;
	static vertex_descriptor null_vertex();
	// IncidenceGraph
	//--------------------------------------------------------------------------
	struct out_edge_iterator;
	using degree_size_type = typename base_traits::degree_size_type;
	// BidirectionalGraph
	//--------------------------------------------------------------------------
	struct in_edge_iterator;

	// VertexListGraph
	//--------------------------------------------------------------------------
	struct vertex_iterator;
	using vertices_size_type = typename base_traits::vertices_size_type;
	// EdgeListGraph
	//--------------------------------------------------------------------------
	struct edge_iterator;
	using edges_size_type = typename base_traits::edges_size_type;
	// AdjacencyMatrix
	//--------------------------------------------------------------------------
	// (nothing)
	// PropertyGraph
	//--------------------------------------------------------------------------
	// (nothing)
public:
	union_graph();
	void push_back(const Graph *g);
	const Graph &operator[](std::size_t idx) const;
	std::size_t size() const;
	const_iterator begin() const;
	const_iterator end() const;
	vertices_size_type get_num_vertices() const;
	vertices_size_type get_vertex_idx_offset(std::size_t gIdx) const;
	edges_size_type get_num_edges() const;
	edges_size_type get_edge_idx_offset(std::size_t gIdx) const;
private:
	// idxOffset.back() is the number of vertices in the graph
	// idxOffset.size() == graphs.size() + 1
	std::vector<const Graph*> graphs;
	std::vector<typename base_traits::vertices_size_type> vertexIdxOffset;
	std::vector<typename base_traits::edges_size_type> edgeIdxOffset;
};

// Implementation details
//------------------------------------------------------------------------------

template<typename Graph>
union_graph<Graph>::union_graph() : vertexIdxOffset(1, 0), edgeIdxOffset(1, 0) { }

template<typename Graph>
void union_graph<Graph>::push_back(const Graph *g) {
	const auto vOffset = vertexIdxOffset.back() + num_vertices(*g);
	vertexIdxOffset.push_back(vOffset);
	const auto eOffset = edgeIdxOffset.back() + num_edges(*g);
	edgeIdxOffset.push_back(eOffset);
	graphs.push_back(g);
}

template<typename Graph>
const Graph &union_graph<Graph>::operator[](std::size_t gIdx) const {
	return *graphs[gIdx];
}

template<typename Graph>
std::size_t union_graph<Graph>::size() const {
	return graphs.size();
}

template<typename Graph>
typename union_graph<Graph>::const_iterator union_graph<Graph>::begin() const {
	return graphs.begin();
}

template<typename Graph>
typename union_graph<Graph>::const_iterator union_graph<Graph>::end() const {
	return graphs.end();
}

template<typename Graph>
typename union_graph<Graph>::vertices_size_type union_graph<Graph>::get_num_vertices() const {
	return vertexIdxOffset.back();
}

template<typename Graph>
typename union_graph<Graph>::vertices_size_type union_graph<Graph>::get_vertex_idx_offset(std::size_t gIdx) const {
	return vertexIdxOffset[gIdx];
}

template<typename Graph>
typename union_graph<Graph>::edges_size_type union_graph<Graph>::get_num_edges() const {
	return edgeIdxOffset.back();
}

template<typename Graph>
typename union_graph<Graph>::edges_size_type union_graph<Graph>::get_edge_idx_offset(std::size_t gIdx) const {
	return edgeIdxOffset[gIdx];
}

// Graph
//------------------------------------------------------------------------------

template<typename Graph>
struct union_graph<Graph>::vertex_descriptor {

	friend bool operator==(const vertex_descriptor &l, const vertex_descriptor &r) {
		return std::tie(l.gIdx, l.v) == std::tie(r.gIdx, r.v);
	}

	friend bool operator!=(const vertex_descriptor &l, const vertex_descriptor &r) {
		return std::tie(l.gIdx, l.v) != std::tie(r.gIdx, r.v);
	}

	friend bool operator<(const vertex_descriptor &l, const vertex_descriptor &r) {
		return std::tie(l.gIdx, l.v) < std::tie(r.gIdx, r.v);
	}

	friend std::ostream &operator<<(std::ostream &s, const vertex_descriptor &v) {
		return s << '(' << v.gIdx << ", " << v.v << ')';
	}
public:
	std::size_t gIdx;
	typename base_traits::vertex_descriptor v;
};

template<typename Graph>
struct union_graph<Graph>::edge_descriptor {

	friend bool operator==(const edge_descriptor &l, const edge_descriptor &r) {
		return std::tie(l.gIdx, l.e) == std::tie(r.gIdx, r.e);
	}

	friend bool operator!=(const edge_descriptor &l, const edge_descriptor &r) {
		return std::tie(l.gIdx, l.e) != std::tie(r.gIdx, r.e);
	}

	friend bool operator<(const edge_descriptor &l, const edge_descriptor &r) {
		return std::tie(l.gIdx, l.e) < std::tie(r.gIdx, r.e);
	}

	friend std::ostream &operator<<(std::ostream &s, const edge_descriptor &e) {
		return s << '(' << e.gIdx << ", " << e.e << ')';
	}
public:
	std::size_t gIdx;
	typename base_traits::edge_descriptor e;
};

template<typename Graph>
typename union_graph<Graph>::vertex_descriptor union_graph<Graph>::null_vertex() {
	return vertex_descriptor{std::numeric_limits<std::size_t>::max(), Graph::null_vertex()};
}

// IncidenceGraph
//------------------------------------------------------------------------------

template<typename Graph>
struct union_graph<Graph>::out_edge_iterator
: boost::iterator_facade<typename union_graph<Graph>::out_edge_iterator, typename union_graph<Graph>::edge_descriptor,
typename std::iterator_traits<typename base_traits::out_edge_iterator>::iterator_category, typename union_graph<Graph>::edge_descriptor> {
	out_edge_iterator() = default;

	out_edge_iterator(std::size_t gIdx, typename base_traits::out_edge_iterator eIter) : gIdx(gIdx), eIter(eIter) { }
private:
	friend class boost::iterator_core_access;

	edge_descriptor dereference() const { // Readable Iterator, Writable Iterator
		return edge_descriptor{gIdx, *eIter};
	}

	bool equal(const out_edge_iterator &other) const { // Single Pass Iterator
		return std::tie(gIdx, eIter) == std::tie(other.gIdx, other.eIter);
	}

	void increment() { // Incrementable Iterator
		++eIter;
	}
	//a.decrement()	unused	 	Bidirectional Traversal Iterator
	//a.advance(n)	unused	 	Random Access Traversal Iterator
	//c.distance_to(z)	convertible to F::difference_type	equivalent to distance(c, X(z)).	Random Access Traversal Iterator
private:
	std::size_t gIdx;
	typename base_traits::out_edge_iterator eIter;
};

template<typename Graph>
std::pair<typename union_graph<Graph>::out_edge_iterator, typename union_graph<Graph>::out_edge_iterator>
out_edges(typename union_graph<Graph>::vertex_descriptor v, const union_graph<Graph> &g) {
	auto base = out_edges(v.v, g[v.gIdx]);
	return std::make_pair(
			typename union_graph<Graph>::out_edge_iterator(v.gIdx, base.first),
			typename union_graph<Graph>::out_edge_iterator(v.gIdx, base.second));
}

template<typename Graph>
typename union_graph<Graph>::vertex_descriptor source(typename union_graph<Graph>::edge_descriptor e, const union_graph<Graph> &g) {
	return typename union_graph<Graph>::vertex_descriptor{e.gIdx, source(e.e, g[e.gIdx])};
}

template<typename Graph>
typename union_graph<Graph>::vertex_descriptor target(typename union_graph<Graph>::edge_descriptor e, const union_graph<Graph> &g) {
	return typename union_graph<Graph>::vertex_descriptor{e.gIdx, target(e.e, g[e.gIdx])};
}

template<typename Graph>
typename union_graph<Graph>::degree_size_type out_degree(typename union_graph<Graph>::vertex_descriptor v, const union_graph<Graph> &g) {
	return out_degree(v.v, g[v.gIdx]);
}

// BidirectionalGraph
//------------------------------------------------------------------------------

template<typename Graph>
struct union_graph<Graph>::in_edge_iterator
: boost::iterator_facade<typename union_graph<Graph>::in_edge_iterator, typename union_graph<Graph>::edge_descriptor,
typename std::iterator_traits<typename base_traits::in_edge_iterator>::iterator_category, typename union_graph<Graph>::edge_descriptor> {
	in_edge_iterator() = default;

	in_edge_iterator(std::size_t gIdx, typename base_traits::in_edge_iterator eIter) : gIdx(gIdx), eIter(eIter) { }
private:
	friend class boost::iterator_core_access;

	edge_descriptor dereference() const { // Readable Iterator, Writable Iterator
		return edge_descriptor{gIdx, *eIter};
	}

	bool equal(const in_edge_iterator &other) const { // Single Pass Iterator
		return std::tie(gIdx, eIter) == std::tie(other.gIdx, other.eIter);
	}

	void increment() { // Incrementable Iterator
		++eIter;
	}
	//a.decrement()	unused	 	Bidirectional Traversal Iterator
	//a.advance(n)	unused	 	Random Access Traversal Iterator
	//c.distance_to(z)	convertible to F::difference_type	equivalent to distance(c, X(z)).	Random Access Traversal Iterator
private:
	std::size_t gIdx;
	typename base_traits::in_edge_iterator eIter;
};

template<typename Graph>
std::pair<typename union_graph<Graph>::in_edge_iterator, typename union_graph<Graph>::in_edge_iterator>
in_edges(typename union_graph<Graph>::vertex_descriptor v, const union_graph<Graph> &g) {
	auto base = in_edges(v.v, g[v.gIdx]);
	return std::make_pair(
			typename union_graph<Graph>::in_edge_iterator(v.gIdx, base.first),
			typename union_graph<Graph>::in_edge_iterator(v.gIdx, base.second));
}

template<typename Graph>
typename union_graph<Graph>::degree_size_type in_degree(typename union_graph<Graph>::vertex_descriptor v, const union_graph<Graph> &g) {
	return in_degree(v.v, g[v.gIdx]);
}

template<typename Graph>
typename union_graph<Graph>::degree_size_type degree(typename union_graph<Graph>::vertex_descriptor v, const union_graph<Graph> &g) {
	return degree(v.v, g[v.gIdx]);
}


// VertexListGraph
//------------------------------------------------------------------------------

template<typename Graph>
struct union_graph<Graph>::vertex_iterator
: boost::iterator_facade<typename union_graph<Graph>::vertex_iterator, typename union_graph<Graph>::vertex_descriptor,
typename std::iterator_traits<typename base_traits::vertex_iterator>::iterator_category, typename union_graph<Graph>::vertex_descriptor> {
	vertex_iterator() = default;

	explicit vertex_iterator(const union_graph<Graph> *g)
	: g(g), gIdx(g->size()) { }

	vertex_iterator(const union_graph<Graph> *g, std::size_t gIdx, typename base_traits::vertex_iterator vIter)
	: g(g), gIdx(gIdx), vIter(vIter) { }
private:
	friend class boost::iterator_core_access;

	vertex_descriptor dereference() const { // Readable Iterator, Writable Iterator
		return vertex_descriptor{gIdx, *vIter};
	}

	bool equal(const vertex_iterator &other) const { // Single Pass Iterator
		if(gIdx == g->size()) return std::tie(g, gIdx) == std::tie(other.g, other.gIdx);
		else return std::tie(g, gIdx, vIter) == std::tie(other.g, other.gIdx, other.vIter);
	}

	void increment() { // Incrementable Iterator
		++vIter;
		while(vIter == vertices((*g)[gIdx]).second) {
			gIdx++;
			if(gIdx == g->size()) break;
			vIter = vertices((*g)[gIdx]).first;
		}
	}
	//a.decrement()	unused	 	Bidirectional Traversal Iterator
	//a.advance(n)	unused	 	Random Access Traversal Iterator

	std::ptrdiff_t distance_to(const vertex_iterator &other) const { // Random Access Traversal Iterator
		std::size_t idxThis = gIdx == g->size() ? g->get_num_vertices() : get(boost::vertex_index_t(), *g, * * this);
		std::size_t idxOther = other.gIdx == g->size() ? g->get_num_vertices() : get(boost::vertex_index_t(), *g, *other);
		return idxOther - idxThis;
	}
private:
	const union_graph<Graph> *g;
	std::size_t gIdx;
	typename base_traits::vertex_iterator vIter;
};

template<typename Graph>
std::pair<typename union_graph<Graph>::vertex_iterator, typename union_graph<Graph>::vertex_iterator>
vertices(const union_graph<Graph> &g) {
	auto endIter = typename union_graph<Graph>::vertex_iterator(&g);
	for(std::size_t gIdx = 0; gIdx < g.size(); gIdx++) {
		if(num_vertices(g[gIdx]) > 0) {
			auto iter = typename union_graph<Graph>::vertex_iterator(&g, gIdx, vertices(g[gIdx]).first);
			return std::make_pair(iter, endIter);
		}
	}
	return std::make_pair(endIter, endIter);
}

template<typename Graph>
typename union_graph<Graph>::vertices_size_type num_vertices(const union_graph<Graph> &g) {
	return g.get_num_vertices();
}

// EdgeListGraph
//----------------------------------------------------------------------------

template<typename Graph>
struct union_graph<Graph>::edge_iterator
: boost::iterator_facade<typename union_graph<Graph>::edge_iterator, typename union_graph<Graph>::edge_descriptor,
typename std::iterator_traits<typename base_traits::edge_iterator>::iterator_category, typename union_graph<Graph>::edge_descriptor> {
	edge_iterator() = default;

	explicit edge_iterator(const union_graph<Graph> *g)
	: g(g), gIdx(g->size()) { }

	edge_iterator(const union_graph<Graph> *g, std::size_t gIdx, typename base_traits::edge_iterator eIter)
	: g(g), gIdx(gIdx), eIter(eIter) { }
private:
	friend class boost::iterator_core_access;

	edge_descriptor dereference() const { // Readable Iterator, Writable Iterator
		return edge_descriptor{gIdx, *eIter};
	}

	bool equal(const edge_iterator &other) const { // Single Pass Iterator
		if(gIdx == g->size()) return std::tie(g, gIdx) == std::tie(other.g, other.gIdx);
		else return std::tie(g, gIdx, eIter) == std::tie(other.g, other.gIdx, other.eIter);
	}

	void increment() { // Incrementable Iterator
		++eIter;
		while(eIter == edges((*g)[gIdx]).second) {
			gIdx++;
			if(gIdx == g->size()) break;
			eIter = edges((*g)[gIdx]).first;
		}
	}
	//a.decrement()	unused	 	Bidirectional Traversal Iterator
	//a.advance(n)	unused	 	Random Access Traversal Iterator
	//c.distance_to(z)	convertible to F::difference_type	equivalent to distance(c, X(z)).	Random Access Traversal Iterator
private:
	const union_graph<Graph> *g;
	std::size_t gIdx;
	typename base_traits::edge_iterator eIter;
};

template<typename Graph>
std::pair<typename union_graph<Graph>::edge_iterator, typename union_graph<Graph>::edge_iterator>
edges(const union_graph<Graph> &g) {
	auto endIter = typename union_graph<Graph>::edge_iterator(&g);
	for(std::size_t gIdx = 0; gIdx < g.size(); gIdx++) {
		if(num_edges(g[gIdx]) > 0) {
			auto iter = typename union_graph<Graph>::edge_iterator(&g, gIdx, edges(g[gIdx]).first);
			return std::make_pair(iter, endIter);
		}
	}
	return std::make_pair(endIter, endIter);
}

template<typename Graph>
typename union_graph<Graph>::edges_size_type num_edges(const union_graph<Graph> &g) {
	return g.get_num_edges();
}

// AdjacencyMatrix
//------------------------------------------------------------------------------

template<typename Graph>
std::pair<typename union_graph<Graph>::edge_descriptor, bool>
edge(typename union_graph<Graph>::vertex_descriptor u, typename union_graph<Graph>::vertex_descriptor v, const union_graph<Graph> &g) {
	if(u.gIdx != v.gIdx) return std::make_pair(typename union_graph<Graph>::edge_descriptor(), false);
	auto base = edge(u.v, v.v, g[u.gIdx]);
	return std::make_pair(typename union_graph<Graph>::edge_descriptor{u.gIdx, base.first}, base.second);
}

// PropertyGraph
//------------------------------------------------------------------------------
namespace detail {

template<typename G>
struct const_wrapper {
};

template<typename T>
struct get_graph_type {
	using type = T;
};

template<typename G>
struct get_graph_type<const_wrapper<G> > {
	using type = G;
};

template<typename G, typename Property>
struct union_graph_property_map {
	using Graph = typename get_graph_type<G>::type;
	using UnionGraph = typename std::conditional<std::is_same<Graph, G>::value, union_graph<Graph>, const union_graph<Graph> >::type;
	using base_traits = boost::property_traits<typename boost::property_map<Graph, Property>::type>;
	using value_type = typename base_traits::value_type;
	using reference = typename base_traits::reference;
	using key_type = typename union_graph<typename get_graph_type<G>::type>::vertex_descriptor;
	using category = typename base_traits::category;
public:

	union_graph_property_map(UnionGraph &g) : g(g) { }
	//private:
	UnionGraph &g;
};

template<typename G, typename Property>
struct property_getter {

	static typename union_graph_property_map<G, Property>::value_type
	apply(const union_graph_property_map<G, Property> &map, typename union_graph<typename get_graph_type<G>::type>::vertex_descriptor v) {
		return get(Property{}, map.g[v.gIdx], v.v);
	}

	static typename union_graph_property_map<G, Property>::value_type
	apply(const union_graph_property_map<G, Property> &map, typename union_graph<typename get_graph_type<G>::type>::edge_descriptor e) {
		return get(Property{}, map.g[e.gIdx], e.e);
	}
};

template<typename G>
struct property_getter<G, boost::vertex_index_t> {
	using Property = boost::vertex_index_t;

	static typename union_graph_property_map<G, Property>::value_type
	apply(const union_graph_property_map<G, Property> &map, typename union_graph<typename get_graph_type<G>::type>::vertex_descriptor v) {
		return get(boost::vertex_index_t(), map.g[v.gIdx], v.v) + map.g.get_vertex_idx_offset(v.gIdx);
	}
};

template<typename G>
struct property_getter<G, boost::edge_index_t> {
	using Property = boost::edge_index_t;

	static typename union_graph_property_map<G, Property>::value_type
	apply(const union_graph_property_map<G, Property> &map, typename union_graph<typename get_graph_type<G>::type>::edge_descriptor e) {
		return get(boost::edge_index_t(), map.g[e.gIdx], e.e) + map.g.get_edge_idx_offset(e.gIdx);
	}
};

template<typename G, typename Property>
typename union_graph_property_map<G, Property>::value_type
get(const union_graph_property_map<G, Property> &map, typename union_graph<typename get_graph_type<G>::type>::vertex_descriptor v) {
	return property_getter<G, Property>::apply(map, v);
}

template<typename G, typename Property>
typename union_graph_property_map<G, Property>::value_type
get(const union_graph_property_map<G, Property> &map, typename union_graph<typename get_graph_type<G>::type>::edge_descriptor e) {
	return property_getter<G, Property>::apply(map, e);
}


} // namespace detail
} // namespace jla_boost
namespace boost {

template<typename G, typename Property>
struct property_map<jla_boost::union_graph<G>, Property> {
	using type = jla_boost::detail::union_graph_property_map<G, Property>;
	using const_type = jla_boost::detail::union_graph_property_map<jla_boost::detail::const_wrapper<G>, Property>;
};

template<typename G, typename Property>
struct property_map<const jla_boost::union_graph<G>, Property> {
	using type = jla_boost::detail::union_graph_property_map<jla_boost::detail::const_wrapper<G>, Property>;
	using const_type = type;
};

} // namespace boost
namespace jla_boost {

template<typename G, typename Property>
typename boost::property_map<union_graph<G>, Property>::type get(Property, union_graph<G> &g) {
	return detail::union_graph_property_map<G, Property>(g);
}

template<typename G, typename Property>
typename boost::property_map<union_graph<G>, Property>::const_type get(Property, const union_graph<G> &g) {
	return detail::union_graph_property_map<detail::const_wrapper<G>, Property>(g);
}

template<typename G, typename Property, typename Key>
typename boost::property_map<union_graph<G>, Property>::type::value_type get(const Property &p, const union_graph<G> &g, const Key &k) {
	return get(get(p, g), k);
}

//put(property, g, x, v)	Set property value for vertex or edge x to v.

// Other
//------------------------------------------------------------------------------

template<typename G>
typename union_graph<G>::vertex_descriptor vertex(typename union_graph<G>::vertices_size_type vId, const union_graph<G> &g) {
	assert(g.size() > 0);
	assert(g.get_vertex_idx_offset(0) == 0);
	const auto gIdx = [&]() -> std::size_t {
		for(std::size_t gIdx = 1; gIdx < g.size(); ++gIdx) {
			if(g.get_vertex_idx_offset(gIdx) > vId) return gIdx - 1;
		}
		return g.size() - 1;
	}();
	const auto baseOffset = g.get_vertex_idx_offset(gIdx);
	assert(baseOffset <= vId);
	return typename union_graph<G>::vertex_descriptor{gIdx, vertex(vId - baseOffset, g[gIdx])};
}

} // namespace jla_boost

#endif /* JLA_BOOST_UNIONGRAPH_H */
