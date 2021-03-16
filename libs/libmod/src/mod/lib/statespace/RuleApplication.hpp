#ifndef MOD_LIB_STATESPACE_RULE_APPLICATION_HPP
#define MOD_LIB_STATESPACE_RULE_APPLICATION_HPP

#include <mod/lib/Rules/LabelledRule.hpp>
#include <mod/lib/Rules/Real.hpp>
#include <mod/lib/Rules/GraphAsRuleCache.hpp>
#include <jla_boost/graph/morphism/models/Vector.hpp>
#include <mod/lib/statespace/ComponentMap.hpp>
#include <mod/lib/DG/NonHyper.hpp>
#include <mod/lib/statespace/CanonMatch.hpp>

namespace mod::lib::statespace {
class PartialMatch {
public:
	using PatternGraph = Rules::LabelledRule::LeftGraphType;
	using HostGraph = Rules::LabelledRule::RightGraphType;
	using LabelledPatternGraph = Rules::LabelledLeftGraph;
	using LabelledHostGraph = Rules::LabelledRightGraph;
	using InvertibleVertexMap = jla_boost::GraphMorphism::InvertibleVectorVertexMap<PatternGraph, HostGraph>;
	PartialMatch(const Rules::Real& rHosts,
	                   const Rules::Real& rPatterns,
	             const std::vector<const Graph::Single *>& hosts,
	             LabelSettings ls,
	             IO::Logger& logger);


	bool push(const ComponentMatch& cm, lib::Rules::GraphAsRuleCache &graphAsRule);
	bool empty() const;
	void pop();
	std::unique_ptr<Rules::Real> apply() const;
	std::vector<const Graph::Single *> lhs(const std::vector<const Graph::Single *>& graphs) const;

	size_t getHostsNullVertex() const { return boost::graph_traits<HostGraph>::null_vertex(); }
	size_t getPatternsNullVertex() const { return boost::graph_traits<PatternGraph>::null_vertex(); }

	const PatternGraph& getPatternsGraph() const {
		return get_graph(get_labelled_left(rPatterns.getDPORule()));
	}

	const HostGraph& getHostsGraph() const {
		return get_graph(get_labelled_right(rHosts.getDPORule()));
	}

	friend std::ostream& operator<<(std::ostream& os, const PartialMatch& cm) {
		const auto &lgPatterns = get_labelled_left(cm.rPatterns.getDPORule());
		const auto &gPatterns = get_graph(lgPatterns);
		const auto &lgHosts = get_labelled_right(cm.rHosts.getDPORule());
		const auto &gHosts = get_graph(lgHosts);
		os << "CompositionMap(";
		for (const auto vp : asRange(vertices(gPatterns))) {
			auto vh = get(cm.map, gPatterns, gHosts, vp);
			os << vp << " -> " << vh << ", ";
		}
		os << ")";
		return os;
	}

private:

	const Rules::Real& rHosts;
	const Rules::Real& rPatterns;
	InvertibleVertexMap map;

	std::vector<const ComponentMatch *> componentMatches;
	CanonMatch canonMatch;
	IO::Logger& logger;
};

class RuleApplicationMap {
public:
	using PatternGraph = Rules::LabelledRule::LeftGraphType;
	using HostGraph = Rules::LabelledRule::RightGraphType;
	using LabelledPatternGraph = Rules::LabelledLeftGraph;
	using LabelledHostGraph = Rules::LabelledRightGraph;
	using InvertibleVertexMap = jla_boost::GraphMorphism::InvertibleVectorVertexMap<PatternGraph, HostGraph>;
	RuleApplicationMap(const Rules::Real& rHosts,
	                   const Rules::Real& rPatterns);

	bool pushComponentMap(const ComponentMap& partialMap);
	void popComponentMap();
	std::vector<const Graph::Single *> getLeftGraphs(const std::vector<const Graph::Single *>& graphs) const;

	std::vector<size_t> getProjectedReactionCenter() const;

	size_t getHostsNullVertex() const { return boost::graph_traits<HostGraph>::null_vertex(); }
	size_t getPatternsNullVertex() const { return boost::graph_traits<PatternGraph>::null_vertex(); }

	const PatternGraph& getPatternsGraph() const {
		return get_graph(get_labelled_left(rPatterns.getDPORule()));
	}

	const HostGraph& getHostsGraph() const {
		return get_graph(get_labelled_right(rHosts.getDPORule()));
	}

	const std::vector<int>& getHostCount() const;
	std::vector<size_t> getHostVertices() const;

	std::unique_ptr<Rules::Real> computeDerivation(IO::Logger& logger) const;
	std::unique_ptr<Rules::Real> getRuleState(IO::Logger& logger) const;

	const InvertibleVertexMap& getMap() const;
	const Rules::Real& getRule() const { return rPatterns; }

	friend std::ostream& operator<<(std::ostream& os, const RuleApplicationMap& cm) {
		const auto &lgPatterns = get_labelled_left(cm.rPatterns.getDPORule());
		const auto &gPatterns = get_graph(lgPatterns);
		const auto &lgHosts = get_labelled_right(cm.rHosts.getDPORule());
		const auto &gHosts = get_graph(lgHosts);
		os << "CompositionMap(";
		for (const auto vp : asRange(vertices(gPatterns))) {
			auto vh = get(cm.map, gPatterns, gHosts, vp);
			os << vp << " -> " << vh << ", ";
		}
		os << ")";
		return os;
	}

private:
	const Rules::Real& rHosts;
	const Rules::Real& rPatterns;
	InvertibleVertexMap map;

	std::vector<const ComponentMap *> addedComponentMaps;
	std::vector<int> hostCount;
};

class RuleApplication {
public:
	RuleApplication(const RuleApplicationMap& map,
	                 std::vector<const Graph::Single *> lhs,
	                 std::vector<const Graph::Single *> rhs,
	                DG::NonHyper::Edge e);

	std::vector<const Graph::Single *>
	transition(const std::vector<const Graph::Single *>& graphs);


	RuleApplicationMap map;
	std::vector<const Graph::Single *> lhs, rhs;
	DG::NonHyper::Edge hyperEdge;
};

}

#endif
