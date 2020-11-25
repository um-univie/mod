#ifndef MOD_LIB_STATE_SPACE_MATCH_NETWORK_HPP
#define MOD_LIB_STATE_SPACE_MATCH_NETWORK_HPP

#include <mod/lib/Rules/Real.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/Graph/Collection.hpp>

namespace mod::lib::statespace {

class MatchNetwork {
	struct RulePatternIdx {
		size_t ruleIdx, patternIdx;
	};


public:
	struct VProp {
		Graph::Single *graph;
		std::unordered_map<const Graph::Single *, bool> isEmbeddable;
		std::vector<RulePatternIdx> patternIndices;
	};

	struct EProp { };

	using GraphType = jla_boost::EdgeIndexedAdjacencyList<boost::bidirectionalS, VProp, EProp>;
	//using GraphType = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VProp, EProp>;
	using Vertex = boost::graph_traits<GraphType>::vertex_descriptor;
	using Edge = boost::graph_traits<GraphType>::edge_descriptor;

	struct StoredRule {
		const Rules::Real *rule;
		std::vector<Vertex> pattern2vertex;
	};

	MatchNetwork(const std::vector<const Rules::Real *>& rules,
	             LabelSettings labelSettings);
	std::vector<size_t> getValidRules(const std::vector<const Graph::Single *>& hosts);
private:
	bool containsPattern(Vertex v, const Graph::Single *host);

	const std::vector<const Rules::Real *> rules;
	Graph::Collection graphDB;
	LabelSettings labelSettings;
	GraphType graph;
	std::vector<Vertex> roots;
};

}

#endif
