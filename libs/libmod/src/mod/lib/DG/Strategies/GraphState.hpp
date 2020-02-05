#ifndef MOD_LIB_DG_STRATEGIES_GRAPHSTATE_H
#define MOD_LIB_DG_STRATEGIES_GRAPHSTATE_H

#include <boost/iterator/transform_iterator.hpp>

#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <vector>

namespace mod {
namespace lib {
namespace Graph {
class Single;
} // namespace Graph
namespace DG {
namespace Strategies {

struct GraphState {
	using GraphList = std::vector<const lib::Graph::Single *>;

	struct Subset {
		struct Transformer {
			Transformer(const GraphList &graphs) : graphs(graphs) {}
			GraphList::value_type operator()(unsigned int i) const {
				assert(i < graphs.size());
				return graphs[i];
			}
		private:
			const GraphList &graphs;
		};
		using const_iterator = boost::transform_iterator<Transformer, std::vector<unsigned int>::const_iterator, const lib::Graph::Single *>;
		using size_type = std::vector<unsigned int>::size_type;
	public:
		explicit Subset(const GraphState &rs) : rs(rs) {}
		explicit Subset(const GraphState &rs, const Subset &other) : rs(rs), indices(other.indices) {}

		const_iterator begin() const {
			return const_iterator(indices.begin(), Transformer(rs.getUniverse()));
		}

		const_iterator end() const {
			return const_iterator(indices.end(), Transformer(rs.getUniverse()));
		}

		size_type size() const {
			return indices.size();
		}

		bool empty() const {
			return indices.empty();
		}
	private:
		friend class GraphState;
		const GraphState &rs;
		std::vector<unsigned int> indices;
	};
	using SubsetStore = std::unordered_map<unsigned int, Subset>;
private:
	void commonInit();
private:
	GraphState &operator=(const GraphState &other); // disable
public:
	explicit GraphState();
	explicit GraphState(const GraphState &other);
	explicit GraphState(const std::vector<const Graph::Single *> &universe);
	explicit GraphState(const std::vector<const GraphState *> &resultSets);
	~GraphState();
	//	void promoteToSubset(unsigned int subsetIndex, const lib::Graph::Single *g);
	void addToSubset(unsigned int subsetIndex, const lib::Graph::Single *g);
	void addToUniverse(const lib::Graph::Single *g);
	template<typename T>
	struct Compare;
	template<typename T>
	void sortUniverse(const T compare);
	template<typename T>
	void sortSubset(unsigned int subsetIndex, const T compare);
	bool hasSubset(unsigned int subsetIndex) const;
	const Subset &getSubset(unsigned int i) const;
	const SubsetStore &getSubsets() const;
	const GraphList &getUniverse() const;
	bool isInUniverse(const lib::Graph::Single *g) const;
	friend bool operator==(const GraphState &a, const GraphState &b);
private:
	unsigned int addUniverseGetIndex(const lib::Graph::Single *g);
private:
	GraphList universe;
	SubsetStore subsets;
};

template<typename T>
class GraphState::Compare {
	Compare();
public:

	explicit Compare(const GraphState::GraphList &graphs, T compare)
			: graphs(graphs), compare(compare) {}

	bool operator()(unsigned int a, unsigned int b) {
		assert(a < graphs.size());
		assert(b < graphs.size());
		assert(graphs[a]);
		assert(graphs[b]);
		return compare(graphs[a], graphs[b]);
	}
	const GraphState::GraphList &graphs;
	T compare;
};

template<typename T>
void GraphState::sortUniverse(T compare) {
	std::vector<unsigned int> newToOld(universe.size());
	for(unsigned int i = 0; i < newToOld.size(); i++) newToOld[i] = i;
	Compare<T> comp(universe, compare);
	std::stable_sort(newToOld.begin(), newToOld.end(), comp);
	std::vector<unsigned int> oldToNew(universe.size());
	for(unsigned int i = 0; i < universe.size(); i++) oldToNew[newToOld[i]] = i;
	{
		GraphList newUniverse(universe.size());
		for(unsigned int i = 0; i < universe.size(); i++) newUniverse[i] = universe[newToOld[i]];
		std::swap(newUniverse, universe);
	}
	{ // TODO: remove, stupid sanity check
		for(unsigned int i = 1; i < universe.size(); i++) assert(!compare(universe[i], universe[i - 1]));
	}
	// substitute subset ids
	for(SubsetStore::value_type &p : subsets)
		for(unsigned int i = 0; i < p.second.indices.size(); i++)
			p.second.indices[i] = oldToNew[p.second.indices[i]];
}

template<typename T>
void GraphState::sortSubset(unsigned int subsetIndex, T compare) {
	assert(hasSubset(subsetIndex));
	SubsetStore::iterator iter = subsets.find(subsetIndex);
	assert(iter != subsets.end());
	Subset &subset = iter->second;
	Compare<T> comp(universe, compare);
	std::stable_sort(subset.indices.begin(), subset.indices.end(), comp);
}

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_DG_STRATEGIES_GRAPHSTATE_H */