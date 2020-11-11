#include <mod/lib/statespace/DynamicDG.hpp>
#include <mod/lib/statespace/ComponentMap.hpp>
#include <mod/lib/Rules/Real.hpp>
#include <mod/lib/RC/MatchMaker/ComponentWiseUtil.hpp>
#include <mod/lib/Graph/LabelledGraph.hpp>
#include <mod/lib/LabelledUnionGraph.hpp>
#include <mod/lib/Rules/GraphToRule.hpp>
#include <mod/lib/DG/RuleApplicationUtils.hpp>
#include <mod/lib/IO/IO.hpp>
#include <mod/rule/Rule.hpp>
#include <iostream>

#include <mod/lib/RC/ComposeRuleRealGeneric.hpp>

namespace mod::lib::statespace {

void printKey(const DG::GraphMultiset& key) {
	std::cout << "Key: ";
	for (const Graph::Single *g : key) {
		std::cout << g->getName() << ", ";
	}
}

struct DerivationRule {
	std::vector<const lib::Graph::Single *> leftGraphs;
	std::unique_ptr<Rules::Real> rDerivation;
};

class CompositionMap {
	using LeftGraphType = Rules::LabelledRule::LeftGraphType;
	using RightGraphType = Rules::LabelledRule::RightGraphType;
	using InvertibleVertexMap = jla_boost::GraphMorphism::InvertibleVectorVertexMap<LeftGraphType, RightGraphType>;
public:
	CompositionMap(const std::vector<Graph::Single *>& graphs,
	               const Rules::Real& rHosts,
	               const Rules::Real& rPatterns,
	               IO::Logger& logger):
	        graphs(graphs),
	        rHosts(rHosts), rPatterns(rPatterns),
	        m(get_graph(get_labelled_left(rPatterns.getDPORule())),
	          get_graph(get_labelled_right(rHosts.getDPORule()))),
	        logger(logger), timesGraphAdded(graphs.size(), 0) {
	}

	bool addComponentMap(const ComponentMap& partialMap) {
		std::cout << "Adding Component Map: " << partialMap.patternId << ", " << partialMap.hostId << std::endl;
		const auto &lgPatterns = get_labelled_left(rPatterns.getDPORule());
		const auto &gPatterns = get_graph(lgPatterns);
		const auto &lgHosts = get_labelled_right(rHosts.getDPORule());
		const auto &gHosts = get_graph(lgHosts);
		const auto vNullHosts = boost::graph_traits<RightGraphType>::null_vertex();
		const auto vNullPatterns = boost::graph_traits<LeftGraphType>::null_vertex();

		timesGraphAdded[partialMap.hostId] += 1;
		const auto &gp = get_component_graph(partialMap.patternId, lgPatterns);
		for (const auto vp : asRange(vertices(gp))) {
//			std::cout << vp << " >> " << get(m, gPatterns, gHosts, vp) << std::endl;
			assert(get(m, gPatterns, gHosts, vp) == vNullPatterns);
			const auto vh = get(partialMap.m, gPatterns, gHosts, vp);
			assert(vh != vNullHosts);
			if (get_inverse(m, gPatterns, gHosts, vh) != vNullHosts) {
				return false;
			}
			put(m, gHosts, gPatterns, vp, vh);
//			std::cout << "mapping " << v << " -> " << vDomHost << std::endl;
		}
		return isValidComponentMap(rHosts.getDPORule(),
		                         rPatterns.getDPORule(),
		                         partialMap.patternId,
		                         m,
		                         logger);
	}

	void removeComponentMap(const ComponentMap& partialMap) {
		const auto &lgPatterns = get_labelled_left(rPatterns.getDPORule());
		const auto &gPatterns = get_graph(lgPatterns);
		const auto &lgHosts = get_labelled_right(rHosts.getDPORule());
		const auto &gHosts = get_graph(lgHosts);
		const auto vNullHosts = boost::graph_traits<RightGraphType>::null_vertex();
		const auto vNullPatterns = boost::graph_traits<LeftGraphType>::null_vertex();

		timesGraphAdded[partialMap.hostId] -= 1;

		const auto &gp = get_component_graph(partialMap.patternId, lgPatterns);
		for (const auto vp : asRange(vertices(gp))) {
			put(m, gPatterns, gHosts, vp, vNullHosts);
		}
	}

	DerivationRule compose() {
		std::vector<size_t> copyVertices;
		std::vector<const Graph::Single  *> lhs;
		const auto &lgHosts = get_labelled_right(rHosts.getDPORule());
		for (size_t i = 0; i < graphs.size(); i++) {
			if (timesGraphAdded[i] > 0)  {
				lhs.push_back(graphs[i]);
				const auto &gh = get_component_graph(i, lgHosts);
				for (const auto vh : asRange(vertices(gh))) {
					copyVertices.push_back(vh);
				}
			}
		}

		using HasTerm = GraphMorphism::HasTermData<InvertibleVertexMap>;
		using HasStereo = GraphMorphism::HasStereoData<InvertibleVertexMap>;
		constexpr LabelType labelType = HasTerm::value ? LabelType::Term : LabelType::String;
		auto rDer =  RC::composeRuleRealByMatch<labelType, HasStereo::value>(
		            rHosts, rPatterns, m, false, logger, &copyVertices);
		return DerivationRule{ std::move(lhs), std::move(rDer) };

	}

	friend std::ostream& operator<<(std::ostream& os, const CompositionMap& cm) {
		const auto &lgPatterns = get_labelled_left(cm.rPatterns.getDPORule());
		const auto &gPatterns = get_graph(lgPatterns);
		const auto &lgHosts = get_labelled_right(cm.rHosts.getDPORule());
		const auto &gHosts = get_graph(lgHosts);
		os << "CompositionMap(";
		for (const auto vp : asRange(vertices(gPatterns))) {
			auto vh = get(cm.m, gPatterns, gHosts, vp);
			os << vp << " -> " << vh << ", ";
		}
		os << ")";
		return os;
	}

private:
	const std::vector<Graph::Single *> graphs;
	const Rules::Real& rHosts;
	const Rules::Real& rPatterns;
	InvertibleVertexMap m;
	IO::Logger logger;

	std::vector<size_t> timesGraphAdded;
};


std::vector<std::vector<ComponentMap>> computeMorphisms(const Rules::Real& rHosts,
                                                        const Rules::Real& rPatterns,
                                                        const LabelSettings& labelSettings,
                                                        IO::Logger& logger)  {

	size_t nPatterns = rPatterns.getDPORule().numLeftComponents;
	size_t nHosts = rHosts.getDPORule().numRightComponents;
	std::vector<std::vector<ComponentMap>> maps(nPatterns);


	const auto& patterns = get_labelled_left(rPatterns.getDPORule());
	const auto& hosts = get_labelled_right(rHosts.getDPORule());
	auto mm = RC::makeRuleRuleComponentMonomorphism(patterns, hosts, true, labelSettings, 0, logger);

	for (size_t pid = 0; pid < nPatterns; pid++) {
		for (size_t hid = 0; hid < nHosts; hid++) {
			auto morphisms = mm(pid, hid);
			for (auto& m : morphisms) {
				bool isValid = isValidComponentMap(rHosts.getDPORule(),
				                                   rPatterns.getDPORule(),
				                                   pid, m, logger);


				std::cout << "MAP " << pid << ", " << hid << ": ";
				const auto &gp = get_component_graph(pid, patterns);
				for (const auto vp : asRange(vertices(gp))) {
					const auto vh = get(m, get_graph(patterns), get_graph(hosts), vp);
					std::cout << vp << " -> " << vh << ", ";

				}
				std::cout << std::endl;
				if (isValid) {
					maps[pid].push_back(ComponentMap(std::move(m), pid, hid));
				}
			}
		}
	}
	return maps;
}

std::vector<DerivationRule> composeRules(const std::vector<Graph::Single *>& hosts,
                                                       const Rules::Real& rHosts,
                                                       const Rules::Real& rPatterns,
                                                       const std::vector<std::vector<ComponentMap>>& maps,
                                                       IO::Logger& logger) {
	std::vector<DerivationRule> rDerivations;
	size_t nPatterns = rPatterns.getDPORule().numLeftComponents;
	size_t nHosts = rHosts.getDPORule().numRightComponents;

	for (size_t pid = 0; pid < nPatterns; pid++) {
		if (maps[pid].size() == 0) {
			return rDerivations;
		}
	}

	CompositionMap compositionMap(hosts, rHosts, rPatterns, logger);
	std::vector<size_t> stack;
	stack.push_back(0);

	while (stack.size() > 0) {
//		std::cout << compositionMap << std::endl;
		size_t cid = stack.size() - 1;
		size_t mid = stack.back();
		if (mid == maps[cid].size()) {
			stack.pop_back();
//			std::cout << "Popping map: " << cid << ", " << mid << std::endl;
			if (stack.size() > 0) {
				compositionMap.removeComponentMap(maps[cid-1][stack.back()]);
				stack.back() += 1;
			}
			continue;
		}
//		std::cout << "Adding map: " << cid << ", " << mid << std::endl;
		bool success = compositionMap.addComponentMap(maps[cid][mid]);
		if (success)  {
//			std::cout << "SUCCESS" << std::endl;
			if (stack.size() == maps.size()){
				std::cout << "found full" << std::endl;
				std::cout << compositionMap << std::endl;
				auto res = compositionMap.compose();
				assert(res.rDerivation);
				rDerivations.push_back(std::move(res));
				compositionMap.removeComponentMap(maps[cid][mid]);
				stack.back() += 1;
			} else {
//				std::cout << "moving" << std::endl;
				stack.push_back(0);
			}
		} else {
			compositionMap.removeComponentMap(maps[cid][mid]);
			stack.back() += 1;
		}

	}

	return rDerivations;
}

DynamicDG::DynamicDG(DG::NonHyperBuilder* builder, std::vector<const Rules::Real*> rules):
    dgBuilder(builder), rules(rules), labelSettings(builder->getLabelSettings()),
    logger(std::cout) {
	for (const Rules::Real *r : rules)  {
		cachedRules.emplace_back(r, *this);
	}
}


std::vector<DG::NonHyper::Edge>
DynamicDG::findNewEdges(const std::vector<const Graph::Single *>& graphs,
                        const Rules::Real& rHosts,
                        const Rules::Real& rPatterns,
                        std::vector<std::vector<ComponentMatch>>& compMatches,
                        const std::vector<bool>& isNewHost) {

	size_t nPatterns = rPatterns.getDPORule().numLeftComponents;

	std::vector<DG::HyperEdge> edges;
	for (size_t pid = 0; pid < nPatterns; pid++) {
		if (compMatches[pid].size() == 0) {
			return edges;
		}
	}

	struct DirtyMatch {
		const ComponentMatch *cm;
		size_t pid, mid;
	};

	std::vector<std::vector<bool>> isNewMatch(nPatterns);
	std::vector<DirtyMatch> newMatches;
	for (size_t pid = 0; pid < compMatches.size(); pid++) {
		for (size_t mid = 0; mid < compMatches[pid].size();  mid++) {
			isNewMatch[pid].push_back(isNewHost[compMatches[pid][mid].getHostId()]);
			if (isNewHost[compMatches[pid][mid].getHostId()]) {
				newMatches.push_back(DirtyMatch{&compMatches[pid][mid], pid, mid});
			}
		}
	}
	if (newMatches.size() == 9) {
		return edges;
	}

	size_t curPid = newMatches[0].pid;
	PartialMatch partialMatch(rHosts, rPatterns, logger);
	std::vector<size_t> stack;
	for (const auto& dm : newMatches) {
		if (dm.pid > curPid) {
			std::cout << "\t Filtering Maps\n";
			std::vector<ComponentMatch> filteredMatches;
			for (size_t mid = 0; mid < compMatches[curPid].size(); mid++) {
				if (!isNewMatch[curPid][mid]) {
					filteredMatches.push_back(std::move(compMatches[curPid][mid]));
				}
			}
			if (filteredMatches.size() == 0) {
				std::cout << "No more untouched maps left!\n";
				break;
			}
			compMatches[curPid] = std::move(filteredMatches);
			curPid = dm.pid;
		}
		stack.push_back(0);
		bool success = partialMatch.push(*dm.cm);
		assert(success);
		std::cout << "\tAPPLYING DM " << dm.pid << " " << dm.mid << std::endl;
		while (stack.size() > 0) {
			size_t pid = stack.size() - 1;
			size_t mid = stack.back();

			std::cout << "\t\t pid: " << pid << "/" <<compMatches.size() << " mid: " << mid << "/" << compMatches[pid].size()<< std::endl;
			std::cout << partialMatch << std::endl;

			if (pid == dm.pid && mid == 1) {
				std::cout << "\t\tdone with dirty map!" << std::endl;
				stack.pop_back();
				continue;
			}

			if (mid == compMatches[pid].size()) {
				stack.pop_back();
	//			std::cout << "Popping map: " << cid << ", " << mid << std::endl;
				if (stack.size() > 0) {
					if (pid-1 != dm.pid) {
						partialMatch.pop();
					}
					stack.back() += 1;
				}
				continue;
			}

			bool success = true;
			if (pid != dm.pid) {
				std::cout << "pushing " << pid << ", " << mid << compMatches[pid][mid] << std::endl;
				success = partialMatch.push(compMatches[pid][mid]);
			}
			if (success) {
				std::cout << "good push!" << std::endl;
				if (stack.size() == compMatches.size()){
					std::cout << "found full" << std::endl;
					std::cout << partialMatch << std::endl;
					auto res = partialMatch.apply();
					assert(res);
					edges.push_back(dgBuilder.applyDerivationRule(
					                    partialMatch.lhs(graphs), *res, rPatterns).first);
					if (pid != dm.pid) {
						partialMatch.pop();
					}
					stack.back() += 1;
				} else {
	//				std::cout << "moving" << std::endl;
					std::cout << "not full yet!" << std::endl;
					stack.push_back(0);
				}
			} else {
				std::cout << "bad push, popping!" << std::endl;
				partialMatch.pop();
				stack.back() += 1;
			}

		}
		partialMatch.pop();
	}
	return edges;
}

std::vector<std::vector<ComponentMatch>>
DynamicDG::CachedRule::getMatches(const std::vector<const Graph::Single *>& graphs) {
	size_t nComponents = rule->getDPORule().numLeftComponents;
	std::vector<std::vector<ComponentMatch>> matches(nComponents);

	size_t offset = 0;
	size_t hid = 0;
	for (const Graph::Single *g : graphs) {
		const auto& dpoRule = rule->getDPORule();
		const auto& dpoHost = g->getBindRule()->getRule().getDPORule();
		const auto& lgRule = get_labelled_left(dpoRule);;
		const auto& lgHost = get_labelled_right(dpoHost);

		if (graphMatches.find(g) == graphMatches.end()) {
			std::cout << "SAVING MATCHES FOR " << g->getName() << std::endl;

			auto mm = RC::makeRuleRuleComponentMonomorphism(lgRule, lgHost, true,
			                                                ddg.labelSettings, 0,
			                                                ddg.logger);

			for (size_t pid = 0; pid < nComponents; pid++) {
				std::vector<ComponentMatch::VertexMap> compMatches;
				auto morphisms = mm(pid, 0);

				for (auto& m : morphisms) {
					if (isValidComponentMap(dpoHost, dpoRule, pid, m, ddg.logger)) {
						compMatches.push_back(std::move(m));
					}
				}

				graphMatches[g].push_back(std::move(compMatches));
			}
		}

		const std::vector<std::vector<ComponentMatch::VertexMap>>& compMatches = graphMatches[g];

		for (size_t pid = 0; pid < nComponents; pid++) {
			for (const auto& vm : compMatches[pid]) {
				matches[pid].emplace_back(pid, hid, get_graph(lgRule), get_graph(lgHost),
				                          vm, offset);
				std::cout << matches[pid].back() << std::endl;
			}
		}

		hid += 1;
		offset += num_vertices(g->getGraph());
	}

	return matches;
}
std::vector<DG::NonHyper::Edge>
DynamicDG::findReusableEdges(const std::vector<DG::NonHyper::Edge>& foundEdges,
                             const std::vector<const Graph::Single *>& graphs,
                             const std::vector<bool>& isNewHost) {
	std::vector<DG::NonHyper::Edge> edges;

	for (const auto& e : foundEdges)  {
		const auto lhs = dgBuilder.lhs(e);
		bool isValid = true;
		for (const Graph::Single *g : lhs) {
			auto it = std::find(graphs.begin(), graphs.end(), g);
			if (it == graphs.end()) {
				isValid = false;
				break;
			}
			auto idx = std::distance(graphs.begin(), it);
			if (isNewHost[idx]) {
				isValid = false;
				break;
			}
		}
		if (isValid) {
			edges.push_back(e);
		}
	}
	return edges;

}

std::vector<DG::NonHyper::Edge> DynamicDG::apply(const std::vector<const Graph::Single *>& hosts) {
	LabelledUnionGraph<lib::Graph::LabelledGraph> ugHosts;
	for (const Graph::Single* g : hosts) {
		ugHosts.push_back(&g->getLabelledGraph());
	}
	const std::unique_ptr<Rules::Real> rHosts = lib::Rules::graphToRule(ugHosts, lib::Rules::Membership::Right, "G");

	CachedState& state = cacheAndGetState(hosts);

	std::set<DG::NonHyper::Edge> res;


	std::vector<bool> isNewHost(hosts.size(), true);
	std::vector<DG::NonHyper::Edge> cachedEdges;
	if (state.parent.size() > 0) {
		std::vector<const Graph::Single *> newGraphs = dgBuilder.rhs(state.usedEdge);
		for (size_t hid = 0; hid < hosts.size(); hid++) {
			if (std::find(newGraphs.begin(), newGraphs.end(), hosts[hid]) == newGraphs.end()){
				isNewHost[hid] = false;
			}
		}
		cachedEdges = cachedStates[state.parent].edges;
	}

	for (CachedRule& cr : cachedRules) {
		auto compMatches = cr.getMatches(hosts);
		const auto reusedEdges = findReusableEdges(cachedEdges, hosts, isNewHost);
		std::cout << " >> Reusable Edges: " << reusedEdges.size() << std::endl;
		const auto newEdges = findNewEdges(hosts, *rHosts, *cr.rule,
		                                    compMatches, isNewHost);
		std::cout << " >> New Edges: " << newEdges.size() << std::endl;
		res.insert(reusedEdges.begin(), reusedEdges.end());
		res.insert(newEdges.begin(), newEdges.end());
	}

	DG::GraphMultiset parent(hosts);
	std::cout << "UPDATING CACHE: " << std::endl;
	for (auto e : res) {
		const auto lhs = DG::GraphMultiset(dgBuilder.lhs(e));
		const auto rhs = dgBuilder.rhs(e);

		size_t cur_lhs = 0;
		std::vector<const Graph::Single *> newState;
		for (const Graph::Single* g : parent) {
			if (cur_lhs < lhs.size() && g == *(lhs.begin()+cur_lhs)) {
				cur_lhs += 1;
				continue;
			}
			newState.push_back(g);
		}
		newState.insert(newState.end(), rhs.begin(), rhs.end());

		DG::GraphMultiset newStateKey(std::move(newState));
		if (cachedStates.find(newStateKey) == cachedStates.end()) {
			std::cout << "SAVING ";
			printKey(newStateKey);
			std::cout << std::endl;

			cachedStates[newStateKey] = CachedState(parent, e);
		} else {
			std::cout << "KEY ALREADY EXISTS ";
			printKey(newStateKey);
			std::cout << std::endl;

		}
	}
	state.edges = std::vector<DG::NonHyper::Edge>(res.begin(), res.end());

//	for (const Rules::Real *rPatterns : rules) {
//		std::cout << "COMPUTING " << rPatterns->getName() << std::endl;
//		const auto maps = computeMorphisms(*rHosts, *rPatterns, labelSettings, logger);
//		const auto rDerivations = composeRules(hosts, *rHosts, *rPatterns, maps, logger);
//		std::cout << "Found Ders: " << rDerivations.size() << std::endl;
//		for (const auto& dr : rDerivations) {
//			auto edge = dgBuilder.applyDerivationRule(dr.leftGraphs, *dr.rDerivation, *rPatterns).first;
//			res.push_back(edge);
//		}
//	}
	return state.edges;
}


void printDerivation(const std::vector<const Graph::Single *>& lhs,
                     const std::vector<const Graph::Single *>& rhs) {

	    std::cout << "[ ";
		for (const Graph::Single *g : lhs)  {
			std::cout << g->getGraphDFS().first << ", ";
		}
		std::cout << " ] -> [ ";

		for (const Graph::Single *g : rhs)  {
			std::cout << g->getGraphDFS().first << ", ";
		}
		std::cout << " ]" << std::endl;
}

std::vector<std::shared_ptr<RuleApplication>>
DynamicDG::updateRuleAndCache( const Rules::Real& rHosts,
                                      const Rules::Real& rPatterns,
                                      const std::vector<std::vector<ComponentMap>>& maps,
                                      const RuleApplicationMap& usedAppMap,
                                      const std::vector<std::shared_ptr<RuleApplication>>& foundApplications,
                                      IO::Logger& logger
                                      ) {

	std::vector<const Graph::Single *> hosts = dgBuilder.getGraphs(rHosts);
	printKey(DG::GraphMultiset(hosts));
	std::cout << std::endl;
	std::vector<std::shared_ptr<RuleApplication>> out;
	std::cout << "UPDATING RULE WITH MAP " << usedAppMap << std::endl;

	const auto &lgHosts = get_labelled_right(rHosts.getDPORule());
	const auto &gHosts = get_graph(get_labelled_right(rHosts.getDPORule()));
	std::vector<size_t> reactionCenterVertices = usedAppMap.getProjectedReactionCenter();
	std::cout << "FOUND REACTION CENTER: ";
	for (auto v : reactionCenterVertices) {
		std::cout << v << ", ";
	}
	std::cout << std::endl;
	std::sort(reactionCenterVertices.begin(), reactionCenterVertices.end());
	std::vector<bool> usedHost(hosts.size(), false);
	const auto &usedMap = usedAppMap.getMap();
	const auto &gUsedRule = get_graph(get_labelled_left(usedAppMap.getRule().getDPORule()));
	for (size_t v : asRange(vertices(gUsedRule))) {
		    auto vHost = get(usedMap, gUsedRule, gHosts, v);
			auto compHost = rHosts.getDPORule().rightComponents[vHost];
			usedHost[compHost] = true;
	}

	std::cout << "USED HOSTS ";
	for (auto v : usedHost) {
		std::cout << ((v) ? "TRUE" : "FALSE") << ", ";
	}
	std::cout << std::endl;

	for (const std::shared_ptr<RuleApplication>& ra : foundApplications) {
		const auto& raMap = ra->map.getMap();
		std::unordered_set<size_t> hostIds;
		const auto &gPatterns = get_graph(get_labelled_left(rPatterns.getDPORule()));
		bool shouldUpdate = false;
		bool shouldDelete = false;
		std::cout << "DETERMINING IF WE SHOULD UPDATE/DELETE" << std::endl;
		for (size_t v : asRange(vertices(gPatterns)))  {
			std::cout << "UPDATING MAP " << rHosts.getName() << " " << rPatterns.getName() << " " << ra->map << std::endl;
			auto vHost = get(raMap, gPatterns, gHosts, v);
			auto lower = std::lower_bound(reactionCenterVertices.begin(), reactionCenterVertices.end(), vHost);
			std::cout << "COMPARING: " << vHost << " " << *lower << std::endl;
			if (lower != reactionCenterVertices.end() && *lower == vHost) {
				shouldDelete = true;
				break;
			}
			auto compHost = rHosts.getDPORule().rightComponents[vHost];
			hostIds.insert(compHost);
			if (usedHost[compHost]) {
				shouldUpdate = true;
//				shouldDelete = true; //YOYO
//				break;
			}
		}

		if (shouldDelete) {
			std::cout << "DELETING APP" << std::endl;
			continue;
		}
		if (shouldUpdate) {
			std::vector<size_t> copyVertices;
			std::vector<const Graph::Single *> lhs;
			const auto &lgHosts = get_labelled_right(rHosts.getDPORule());
			for (size_t hid : hostIds)  {
				lhs.push_back(hosts[hid]);
				const auto &gh = get_component_graph(hid, lgHosts);
				for (const auto vh : asRange(vertices(gh))) {
					copyVertices.push_back(vh);
				}

				std::cout << "USING GRAPHS: " << hid << " " << lhs.back()->getName() << std::endl;
			}
			std::cout << "HOST VERTICES: ";
			for (auto v : copyVertices) {
				std::cout << v << ", ";
			}
			std::cout << std::endl;
			using InvertibleVertexMap = RuleApplicationMap::InvertibleVertexMap;
			using HasTerm = GraphMorphism::HasTermData<InvertibleVertexMap>;
			using HasStereo = GraphMorphism::HasStereoData<InvertibleVertexMap>;
			constexpr LabelType labelType = HasTerm::value ? LabelType::Term : LabelType::String;
			auto rDer = RC::composeRuleRealByMatch<labelType, HasStereo::value>(
			            rHosts, rPatterns, raMap, false, logger, &copyVertices);
			assert(rDer);
			auto derResult = dgBuilder.applyDerivationRule(lhs, *rDer, rPatterns);
			out.push_back(std::make_shared<RuleApplication>(ra->map, lhs,
			                                                derResult.second,
			                                                derResult.first));
			printDerivation(lhs, derResult.second);
			DG::GraphMultiset derKey(out.back()->transition(hosts));
			if (states.find(derKey) == states.end()) {
				std::cout << "SAVING ";
				printKey(derKey);
				std::cout << std::endl;
				using HasTerm = GraphMorphism::HasTermData<InvertibleVertexMap>;
				using HasStereo = GraphMorphism::HasStereoData<InvertibleVertexMap>;
				constexpr LabelType labelType = HasTerm::value ? LabelType::Term : LabelType::String;
				auto rState = RC::composeRuleRealByMatch<labelType, HasStereo::value>(
				            rHosts, rPatterns, raMap, false, logger);
				states[derKey] = State{DG::GraphMultiset(hosts),
				        std::move(rState), &rPatterns, &out.back()->map};
			} else {
				std::cout << "KEY EXISTS ";
				printKey(derKey);
				std::cout << std::endl;
			}

		} else {
			std::cout << " >> CAN REUSE APP" << std::endl;
			out.push_back(ra);
		}
	}

	std::cout << "Finding dirtyCompMaps" << std::endl;

	std::vector<const ComponentMap *> dirtyCompMaps;
	std::vector<size_t> mapsi;
	for (size_t pid = 0; pid < maps.size(); pid++) {
		size_t i = 0;
		for (const auto& compMap : maps[pid]) {
			i += 1;
			assert(compMap.m.size() > 0);
			if (!usedHost[compMap.hostId]) {
				continue;
			}
//			else { YOYO
//				    dirtyCompMaps.push_back(&compMap);
//					mapsi.push_back(i-1);
//					continue;

//			}

			const auto& gPatterns = get_graph(get_labelled_left(rPatterns.getDPORule()));
			const auto& gp = get_component_graph(pid, get_labelled_left(rPatterns.getDPORule()));
			for (auto vp : asRange(vertices(gp))) {
				auto vh = get(compMap.m, gPatterns, gHosts, vp);
				auto lower = std::lower_bound(reactionCenterVertices.begin(), reactionCenterVertices.end(), vh);
				if (lower != reactionCenterVertices.end() && *lower == vh) {
					dirtyCompMaps.push_back(&compMap);
					mapsi.push_back(i-1);
					break;
				}
			}
		}
	}

	std::cout << "FOUND " << dirtyCompMaps.size() << " DIRTY MAPS" << std::endl;

	RuleApplicationMap newMap(rHosts, rPatterns);
	size_t i = 0;
	for (const ComponentMap *dcm : dirtyCompMaps) {
		std::vector<size_t> stack, pidMap;
		for (size_t i = 0; i < maps.size(); i++) {
			pidMap.push_back(i);
		}
		stack.push_back(mapsi[i]);
		i += 1;
		std::swap(pidMap[0], pidMap[dcm->patternId]);

		std::cout << "	Pushing DCM FIRST (" << dcm->patternId << ", " << dcm->hostId << ")\n";

		while (stack.size() > 0) {
	//		std::cout << compositionMap << std::endl;
			size_t cid = pidMap[stack.size() - 1];
			size_t mid = stack.back();
			if (stack.size() == 1 && mid == mapsi[i] + 1) {
				break;
			}
			if (mid == maps[cid].size()) {
				stack.pop_back();
	//			std::cout << "Popping map: " << cid << ", " << mid << std::endl;
				if (stack.size() > 0) {
					newMap.popComponentMap();
					stack.back() += 1;
				}
				continue;
			}
			std::cout << "Adding map: " << cid << ", " << mid << "/" << maps[cid].size() <<  std::endl;
			assert(cid < maps.size() && mid < maps[cid].size());
			bool success = newMap.pushComponentMap(maps[cid][mid]);
			if (success)  {
	//			std::cout << "SUCCESS" << std::endl;
				if (stack.size() == maps.size()){
					std::cout << "found full" << std::endl;
					std::cout << newMap << std::endl;
					auto lhs = newMap.getLeftGraphs(hosts);
					auto rDer = newMap.computeDerivation(logger);
					assert(rDer);
					auto derResult = dgBuilder.applyDerivationRule(lhs, *rDer, rPatterns);
					out.push_back(std::make_shared<RuleApplication>(newMap, lhs,
					                                                derResult.second,
					                                                derResult.first));
					DG::GraphMultiset test(derResult.second);
					std::cout << "RHS ";
					printKey(test);
					std::cout << derResult.second[0]->getGraphDFS().first << std::endl;
					std::cout << std::endl;
					printDerivation(lhs, derResult.second);
					DG::GraphMultiset derKey(out.back()->transition(hosts));
					if (states.find(derKey) == states.end()) {
						std::cout << "SAVING ";
						printKey(derKey);
						std::cout << std::endl;

						states[derKey] = State{DG::GraphMultiset(hosts), newMap.getRuleState(logger),
						        &rPatterns, &out.back()->map};
					} else {
						std::cout << "KEY EXISTS ";
						printKey(derKey);
						std::cout << std::endl;
					}
					newMap.popComponentMap();
					stack.back() += 1;
				} else {
	//				std::cout << "moving" << std::endl;
					stack.push_back(0);
				}
			} else {
				newMap.popComponentMap();
				stack.back() += 1;
			}

		}
	}

	// Now for the updating of the new maps



	return out;


}

std::vector<std::shared_ptr<RuleApplication>>
DynamicDG::applyRuleAndCache(const std::vector<const Graph::Single *>& hosts,
                                      const Rules::Real& rHosts,
                                      const Rules::Real& rPatterns,
                                      const std::vector<std::vector<ComponentMap>>& maps,
                                      IO::Logger& logger
                                      ) {
	size_t nPatterns = rPatterns.getDPORule().numLeftComponents;
	size_t nHosts = rHosts.getDPORule().numRightComponents;
	std::vector<std::shared_ptr<RuleApplication>> out;
	std::cout << "NUM Patterns: " << nPatterns << " npattern vertices: " << num_vertices(get_graph(rPatterns.getDPORule())) << std::endl;

	for (size_t pid = 0; pid < nPatterns; pid++) {
		if (maps[pid].size() == 0) {
			return out;
		}
	}

	RuleApplicationMap raMap(rHosts, rPatterns);
	std::vector<size_t> stack;
	stack.push_back(0);

	while (stack.size() > 0) {
		std::cout << raMap << std::endl;
		size_t cid = stack.size() - 1;
		size_t mid = stack.back();
		if (mid == maps[cid].size()) {
			stack.pop_back();
//			std::cout << "Popping map: " << cid << ", " << mid << std::endl;
			if (stack.size() > 0) {
				raMap.popComponentMap();
				stack.back() += 1;
			}
			continue;
		}
		std::cout << "Adding map: " << cid << ", " << mid << std::endl;
		bool success = raMap.pushComponentMap(maps[cid][mid]);
		if (success)  {
			std::cout << "SUCCESS" << std::endl;
			if (stack.size() == maps.size()){
				std::cout << "found full" << std::endl;
				std::cout << raMap << std::endl;
				auto lhs = raMap.getLeftGraphs(hosts);
				auto rDer = raMap.computeDerivation(logger);
				assert(rDer);
				auto derResult = dgBuilder.applyDerivationRule(lhs, *rDer, rPatterns);
				out.push_back(std::make_shared<RuleApplication>(raMap, lhs,
				                                                derResult.second,
				                                                derResult.first));
				DG::GraphMultiset test(derResult.second);
				std::cout << "RHS ";
				printKey(test);
				std::cout << std::endl;
				DG::GraphMultiset derKey(out.back()->transition(hosts));
				if (states.find(derKey) == states.end()) {
					std::cout << "SAVING ";
					printKey(derKey);
					std::cout << std::endl;
					states[derKey] = State{DG::GraphMultiset(hosts),
					        raMap.getRuleState(logger), &rPatterns,
					        &out.back()->map};
				} else {
					std::cout << "KEY EXISTS ";
					printKey(derKey);
					std::cout << std::endl;
				}
				raMap.popComponentMap();
				stack.back() += 1;
			} else {
//				std::cout << "moving" << std::endl;
				stack.push_back(0);
			}
		} else {
			raMap.popComponentMap();
			stack.back() += 1;
		}

	}
	return out;
}

DynamicDG::CachedState& DynamicDG::cacheAndGetState(const std::vector<const Graph::Single *>& graphs) {
	DG::GraphMultiset stateKey(graphs);
	if (cachedStates.find(stateKey) != cachedStates.end()) {
		std::cout << "FOUND STATE ";
		printKey(stateKey);
		std::cout<<std::endl;
		return cachedStates[stateKey];
	}

	std::cout << "INIT STATE ";
	printKey(stateKey);
	std::cout<<std::endl;
	cachedStates[stateKey] = CachedState();
	return cachedStates[stateKey];
}

const DynamicDG::State& DynamicDG::getState(const std::vector<const Graph::Single *>& graphs) {
	DG::GraphMultiset stateKey(graphs);
	if (states.find(stateKey) != states.end()) {
		std::cout << "FOUND STATE " << std::endl;
		return states[stateKey];
	}

	LabelledUnionGraph<lib::Graph::LabelledGraph> ugHosts;
	for (const Graph::Single* g : graphs) {
		ugHosts.push_back(&g->getLabelledGraph());
	}
	std::cout << " >> CREATING NEW STATE ";
	printKey(stateKey);
	std::cout << std::endl;

	std::unique_ptr<Rules::Real> rState = lib::Rules::graphToRule(ugHosts, lib::Rules::Membership::Right, "G");
	states[stateKey] = State{DG::GraphMultiset(), std::move(rState), nullptr, nullptr};

	return states[stateKey];
}

std::vector<DG::NonHyper::Edge> DynamicDG::applyAndCache(const std::vector<const Graph::Single *>& hosts) {
	IO::Logger logger(std::cout);
	const State& state = getState(hosts);

	std::vector<DG::NonHyper::Edge> res;
	std::vector<std::vector<std::shared_ptr<RuleApplication>>> applications;

	DG::GraphMultiset key(hosts);

	if (state.parent.size() == 0) {
		for (const Rules::Real *rPatterns : rules) {
			std::cout << "COMPUTING " << rPatterns->getName() << std::endl;
			const auto maps = computeMorphisms(*state.state, *rPatterns, labelSettings, logger);
			std::cout << "APPLYING" << std::endl;
			const auto rDerivations = applyRuleAndCache(hosts, *state.state, *rPatterns, maps, logger);
			applications.push_back(rDerivations);
			std::cout << "Found Ders: " << rDerivations.size() << std::endl;
			for (const auto& dr : rDerivations) {
				res.push_back(dr->hyperEdge);
			}
		}

		cachedApplications[key] = std::move(applications);
	} else {
		std::cout << "COMPUTINIG CACHED!" << std::endl;
		std::cout << "PARENT: ";
		printKey(state.parent);
		std::cout << std::endl;
		assert(cachedApplications.find(state.parent) != cachedApplications.end());
		for (const Rules::Real *rPatterns : rules) {
			std::cout << "COMPUTING " << rPatterns->getName() << std::endl;
			const auto maps = computeMorphisms(*state.state, *rPatterns, labelSettings, logger);
			const auto rDerivations = updateRuleAndCache(*state.state, *rPatterns, maps,
			                                             *state.map, cachedApplications[state.parent][applications.size()],
			                                             logger);
			applications.push_back(rDerivations);
			std::cout << "Found Ders: " << rDerivations.size() << std::endl;
			for (const auto& dr : rDerivations) {
				res.push_back(dr->hyperEdge);
			}
		}
		cachedApplications[key] = std::move(applications);

	}
	return res;
}

}
