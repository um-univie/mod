#ifndef MOD_LIB_RULES_APPLICATION_PARTIALMATCH_HPP
#define MOD_LIB_RULES_APPLICATION_PARTIALMATCH_HPP

#include <mod/lib/Rules/Real.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/Rules/Application/ComponentMatch.hpp>
#include <mod/lib/LabelledUnionGraph.hpp>
#include <mod/lib/Graph/LabelledGraph.hpp>
#include <jla_boost/graph/morphism/models/Vector.hpp>
#include <mod/lib/IO/IO.hpp>

namespace mod::lib::Rules::Application {

class PartialMatch {
public:
	using MatchGraph = Rules::LabelledRule::LeftGraphType;
	using HostGraph = LabelledUnionGraph<Graph::LabelledGraph>::GraphType;
	using Morphism = jla_boost::GraphMorphism::InvertibleVectorVertexMap<MatchGraph, HostGraph>;

	PartialMatch(const Rules::Real& rule);

	std::pair<bool, bool> tryPush(const ComponentMatch& cm);
	void pop();
	bool isFull() const;
	bool lastPushIsNewInstance() const;

	const std::vector<ComponentMatch>& getCompMatches() const;
	const std::vector<const Graph::Single*>& getLhs() const;

	std::unique_ptr<Rules::Real> apply() const;

	friend std::ostream& operator << (std::ostream& os, const PartialMatch& pm);


private:

	size_t updateHostGraph(const ComponentMatch& cm);

	const Rules::Real& rule;
	std::vector<const Graph::Single *> lhs;
	LabelledUnionGraph<Graph::LabelledGraph> hosts;
	std::map<std::pair<const Graph::Single*, size_t>, size_t> hostIndexMap;
	std::vector<ComponentMatch> compMatches;
	std::vector<bool> addedGraph;
	Morphism morphism;
	IO::Logger logger;
};

}

#endif
