#ifndef MOD_LIB_GRAPH_MULTISET_H
#define MOD_LIB_GRAPH_MULTISET_H

#include <algorithm>
#include <vector>

namespace mod {
namespace lib {
namespace Graph {

template<typename GraphType>
struct Multiset {
	Multiset() = default;

	explicit Multiset(GraphType *g) : graphs(1, g) { }

	explicit Multiset(std::vector<GraphType*> graphs) : graphs(std::move(graphs)) {
		this->graphs.shrink_to_fit();
		std::sort(this->graphs.begin(), this->graphs.end(), typename std::remove_cv<GraphType>::type::IdLess());
	}

	auto begin() const {
		return graphs.begin();
	}

	auto end() const {
		return graphs.end();
	}

	bool empty() const {
		return graphs.empty();
	}

	std::size_t size() const {
		return graphs.size();
	}

	friend bool operator==(const Multiset &l, const Multiset &r) {
		return l.graphs == r.graphs;
	}

	friend bool operator<(const Multiset &l, const Multiset &r) {
		return l.graphs < r.graphs;
	}
private:
	std::vector<GraphType*> graphs;
};

} // namespace Graph
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPH_MULTISET_H */

