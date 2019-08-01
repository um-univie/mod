#ifndef MOD_LIB_GRAPH_COLLECTION_HPP
#define MOD_LIB_GRAPH_COLLECTION_HPP

#include <mod/Config.hpp>
#include <mod/graph/Graph.hpp>

#include <unordered_map>
#include <unordered_set>

#include <boost/functional/hash.hpp>

namespace mod {
namespace lib {
namespace Graph {

struct CollectionStats {
	std::size_t numVertices;
	std::size_t numEdges;
public:
	friend bool operator==(CollectionStats a, CollectionStats b) {
		return std::tie(a.numVertices, a.numEdges) == std::tie(b.numVertices, b.numEdges);
	}
};
} // namespace Graph
} // namespace lib
} // namespace mod

template<>
struct std::hash<mod::lib::Graph::CollectionStats> {
	std::size_t operator()(mod::lib::Graph::CollectionStats stats) const {
		std::size_t res = stats.numVertices;
		boost::hash_combine(res, stats.numEdges);
		return res;
	}
};

namespace mod {
namespace lib {
namespace Graph {

struct Collection {
	explicit Collection(LabelSettings ls, Config::IsomorphismAlg alg);
	~Collection();
	const std::vector<std::shared_ptr<graph::Graph>> &asList() const;
	// by pointer
	bool contains(std::shared_ptr<graph::Graph> g) const;
	// By isomorphism.
	// Returns nullptr if non found.
	std::shared_ptr<graph::Graph> findIsomorphic(std::shared_ptr<graph::Graph> g) const;
	// By isomorphism, but g may not necessarily be wrapped yet.
	// Returns nullptr if non found.
	std::shared_ptr<graph::Graph> findIsomorphic(lib::Graph::Single *g) const;
public:
	// Insert without checking for isomorphism.
	// Still checks for pointer equality.
	// Requires: findIsomorphic(g) != g;
	// Returns whether insertion was done or not.
	bool trustInsert(std::shared_ptr<graph::Graph> g);
	// Try to insert with isomorphism check first.
	// Returns a graph and a boolean indicating whether insertion was successful.
	// If inserted, return g, otherwise return an isomorphic graph.
	// Note: if the same graph object is already present, the return value is <g, false>.
	std::pair<std::shared_ptr<graph::Graph>, bool> tryInsert(std::shared_ptr<graph::Graph> g);
private:
	struct Store;
	const LabelSettings ls;
	std::unordered_map<CollectionStats, std::unique_ptr<Store>> graphStore;
	// owning part
	std::vector<std::shared_ptr<graph::Graph>> graphs;
};

} // namespace Graph
} // namespace lib
} // namespace mod

#endif //MOD_LIB_GRAPH_COLLECTION_HPP