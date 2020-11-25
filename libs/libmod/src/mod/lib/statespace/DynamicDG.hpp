#ifndef MOD_LIB_STATESPACE_DYNAMIC_DG_HPP
#define MOD_LIB_STATESPACE_DYNAMIC_DG_HPP

#include <mod/lib/DG/NonHyperBuilder.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/statespace/ComponentMap.hpp>
#include <mod/lib/statespace/RuleApplication.hpp>
#include <mod/lib/statespace/CanonRule.hpp>
#include <mod/lib/statespace/MatchNetwork.hpp>

namespace mod::lib::statespace{

class DynamicDG {
	struct State {
		DG::GraphMultiset  parent;
		std::unique_ptr<Rules::Real> state;
		const Rules::Real *rule;
		const RuleApplicationMap* map;
	};

	struct CachedState {
		CachedState(): isComputed(false) {}
		CachedState(DG::GraphMultiset parent, DG::NonHyper::Edge usedEdge):
		    parent(parent), usedEdge(usedEdge), isComputed(false) {}
		DG::GraphMultiset parent;
		bool isComputed;
		DG::HyperEdge usedEdge;
		std::vector<DG::HyperEdge> edges;
	};

	struct CachedRule {
		CachedRule(const Rules::Real *rule, DynamicDG& ddg);

		std::vector<std::vector<ComponentMatch>>
		getMatches(const std::vector<const Graph::Single *>& graphs);

		void storeMatches(const Graph::Single *graph);
		bool isCanonMatch(size_t pid, const ComponentMatch::VertexMap& match,
		                    const Graph::Single *g);

		bool isValid(const std::vector<const Graph::Single *>& graphs);

		const Rules::AutGroup& getAutsGroup();


		std::vector<std::unique_ptr<Graph::Single>> lhsGraphs;
		std::vector<std::unordered_map<const Graph::Single *, bool>> hasMatch;
		const Rules::Real * rule;
		DynamicDG& ddg;
		std::map<const Graph::Single *, std::vector<std::vector<ComponentMatch::VertexMap>>> graphMatches;
		std::unique_ptr<lib::Rules::AutGroup> rAuts = nullptr;
	};


public:
	DynamicDG(lib::DG::Builder& builder, std::vector<const Rules::Real*> rules,
	          LabelSettings labelSettings);


	std::vector<DG::NonHyper::Edge> apply(const std::vector<const Graph::Single *>& graphs);

	std::vector<DG::NonHyper::Edge> applyAndCache(const std::vector<const Graph::Single *>& graphs);
private:

	const State& getState(const std::vector<const Graph::Single *>& graphs);

	CachedState& cacheAndGetState(const std::vector<const Graph::Single *>& graphs);

	std::vector<DG::HyperEdge> findNewEdges(const std::vector<const Graph::Single *>& graphs,
	                                        const Rules::Real& rHosts,
	                                        const Rules::Real& rPatterns,
	                                        std::vector<std::vector<ComponentMatch>>& compMatches,
	                                        const std::vector<bool>& isNewHost);

	std::vector<DG::NonHyper::Edge>
	findReusableEdges(const std::vector<DG::NonHyper::Edge>& foundEdges,
	                  const std::vector<const Graph::Single *>& graphs,
	                  const std::vector<bool>& isNewHost);


	std::vector<const Graph::Single *> rhs(DG::NonHyper::Edge) const;



	std::vector<std::shared_ptr<RuleApplication>>
	applyRuleAndCache(const std::vector<const Graph::Single *>& hosts,
	                       const Rules::Real& rHosts,
	                       const Rules::Real& rPatterns,
	                       const std::vector<std::vector<ComponentMap>>& maps,
	                       IO::Logger& logger
	                       );

	std::vector<std::shared_ptr<RuleApplication>>
	updateRuleAndCache( const Rules::Real& rHosts,
	                                      const Rules::Real& rPatterns,
	                                      const std::vector<std::vector<ComponentMap>>& maps,
	                                      const RuleApplicationMap& usedMap,
	                                      const std::vector<std::shared_ptr<RuleApplication>>& foundApplications,
	                                      IO::Logger& logger
	                                      );


	lib::DG::Builder& dgBuilder;
	std::vector<const Rules::Real*> rules;
	std::map<DG::GraphMultiset, State> states;
	std::map<DG::GraphMultiset, std::vector<std::vector<std::shared_ptr<RuleApplication>>>> cachedApplications;

	std::unordered_map<DG::GraphMultiset, CachedState> cachedStates;
	std::vector<CachedRule> cachedRules;



	const LabelSettings labelSettings;
	IO::Logger logger;
	int verbosity = 0;
	MatchNetwork matchNetwork;
};

}

#endif
