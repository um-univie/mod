#ifndef MOD_LIB_UTIL_H
#define	MOD_LIB_UTIL_H

#include <mod/Error.h>

#include <boost/functional/hash.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>

#include <unordered_map>

namespace mod {
namespace lib {

//------------------------------------------------------------------------------
// Concepts
//------------------------------------------------------------------------------

template<typename T, typename ValueType>
struct GraphPropertyConcept {

	BOOST_CONCEPT_USAGE(GraphPropertyConcept) { }
private:

};

//------------------------------------------------------------------------------
// EdgeMap
//------------------------------------------------------------------------------

template<typename Graph, typename ValueType>
struct EdgeMap {
	using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;
	using Edge = typename boost::graph_traits<Graph>::edge_descriptor;
	using VSizeType = typename boost::graph_traits<Graph>::vertices_size_type;

	struct EdgeRepr {
		Edge e; // we only have this for the property pointer, the source and target may be invalid
		VSizeType lower;
		VSizeType higher;

		EdgeRepr(Edge e, const Graph &g) : e(e), lower(get(boost::vertex_index_t(), g, source(e, g))), higher(get(boost::vertex_index_t(), g, target(e, g))) {
			if(lower > higher) std::swap(lower, higher);
		}

		friend bool operator==(EdgeRepr e1, EdgeRepr e2) {
			return e1.e == e2.e;
		}

		friend std::ostream &operator<<(std::ostream &s, EdgeRepr e) {
			return s << e.e;
		}
	};

	struct Hash {

		std::size_t operator()(EdgeRepr e) const {
			return boost::hash_value(std::make_pair(e.lower, e.higher));
		}
	};

	using Data = std::unordered_map<EdgeRepr, ValueType, Hash>;

	explicit EdgeMap(const Graph &g) : g(g) { }
private:
	const Graph &g;
	Data data;
public:

	void reserve(std::size_t n) {
		data.reserve(n);
	}

	void add(Edge e, const ValueType &v) {
		EdgeRepr eRepr(e, g);
		auto iter = data.find(eRepr);
		if(iter != data.end()) MOD_ABORT;
		data.emplace(eRepr, std::move(v));
	}

	void add(Edge e, ValueType &&v) {
		EdgeRepr eRepr(e, g);
		auto iter = data.find(eRepr);
		if(iter != data.end()) MOD_ABORT;
		data.emplace(eRepr, std::move(v));
	}

	typename Data::iterator find(Edge e) {
		return data.find(EdgeRepr(e, g));
	}

	typename Data::const_iterator find(Edge e) const {
		return data.find(EdgeRepr(e, g));
	}

	auto size() const -> decltype(data.size()) {
		return data.size();
	}

	auto begin() const -> decltype(data.begin()) {
		return data.begin();
	}

	auto end() const -> decltype(data.end()) {
		return data.end();
	}
};

template<typename Graph, typename ValueType>
auto begin(const EdgeMap<Graph, ValueType> &em) -> decltype(em.begin()) {
	return em.begin();
}

template<typename Graph, typename ValueType>
auto end(const EdgeMap<Graph, ValueType> &em) -> decltype(em.end()) {
	return em.end();
}

} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_UTIL_H */