#ifndef JLA_BOOST_GRAPH_MORPHISM_VERTEXORDERBYMULT_HPP
#define JLA_BOOST_GRAPH_MORPHISM_VERTEXORDERBYMULT_HPP

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map_iterator.hpp>
#include <boost/range/algorithm/sort.hpp>

#include <algorithm>
#include <vector>

namespace jla_boost {
namespace GraphMorphism {
using namespace boost; // TODO: remvoe
namespace detail {

// Used to sort nodes by in/out degrees

template<typename Graph>
struct vertex_in_out_degree_cmp {
	typedef typename graph_traits<Graph>::vertex_descriptor vertex_type;

	vertex_in_out_degree_cmp(const Graph &g) : g(g) { }

	bool operator()(const vertex_type &v, const vertex_type &w) const {
		// lexicographical comparison
		return std::make_pair(in_degree(v, g), out_degree(v, g)) <
				std::make_pair(in_degree(w, g), out_degree(w, g));
	}
public:
	const Graph &g;
};


// Used to sort nodes by multiplicity of in/out degrees

template<typename Graph, typename FrequencyMap>
struct vertex_frequency_degree_cmp {
	typedef typename graph_traits<Graph>::vertex_descriptor vertex_type;

	vertex_frequency_degree_cmp(const Graph &g, FrequencyMap freq) : g(g), freq_(freq) { }

	bool operator()(const vertex_type &v, const vertex_type &w) const {
		// lexicographical comparison
		return std::make_pair(freq_[v], in_degree(v, g) + out_degree(v, g)) <
				std::make_pair(freq_[w], in_degree(w, g) + out_degree(w, g));
	}
public:
	const Graph &g;
	FrequencyMap freq_;
};

// Sorts vertices of a graph by multiplicity of in/out degrees 

template<typename Graph, typename IndexMap, typename VertexOrder>
void sort_vertices(const Graph &g, IndexMap idx, VertexOrder &order) {
	typedef typename graph_traits<Graph>::vertices_size_type size_type;

	boost::range::sort(order, vertex_in_out_degree_cmp<Graph>(g));

	std::vector<size_type> freq_vec(num_vertices(g), 0);
	typedef iterator_property_map<typename std::vector<size_type>::iterator,
			IndexMap, size_type, size_type&> frequency_map_type;

	frequency_map_type freq = make_iterator_property_map(freq_vec.begin(), idx);

	typedef typename VertexOrder::iterator order_iterator;

	for(order_iterator order_iter = order.begin(); order_iter != order.end();) {
		size_type count = 0;
		for(order_iterator count_iter = order_iter;
				(count_iter != order.end()) &&
				(in_degree(*order_iter, g) == in_degree(*count_iter, g)) &&
				(out_degree(*order_iter, g) == out_degree(*count_iter, g));
				++count_iter)
			++count;

		for(size_type i = 0; i < count; ++i) {
			freq[*order_iter] = count;
			++order_iter;
		}
	}

	boost::range::sort(order, vertex_frequency_degree_cmp<Graph, frequency_map_type>(g, freq));
}

} // namespace detail

// Returns vertex order (vertices sorted by multiplicity of in/out degrees)

template<typename Graph>
std::vector<typename graph_traits<Graph>::vertex_descriptor>
vertex_order_by_mult(const Graph &g) {
	std::vector<typename graph_traits<Graph>::vertex_descriptor> vertex_order;
	std::copy(vertices(g).first, vertices(g).second, std::back_inserter(vertex_order));

	detail::sort_vertices(g, get(vertex_index, g), vertex_order);
	return vertex_order;
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_VERTEXORDERBYMULT_HPP */