#include <mod/lib/statespace/RuleApplication.hpp>
#include <mod/lib/RC/ComposeRuleRealGeneric.hpp>
#include <iostream>
#include <unordered_set>
#include <mod/lib/statespace/CanonMatch.hpp>

namespace mod::lib::statespace {

PartialMatch::PartialMatch(const Rules::Real& rHosts,
                           const Rules::Real& rPatterns,
                           const std::vector<const Graph::Single *>& hosts,
                           LabelSettings ls,
                           IO::Logger& logger) :
        rHosts(rHosts), rPatterns(rPatterns),
        map(getPatternsGraph(), getHostsGraph()),
        canonMatch(hosts, rPatterns, ls),
        logger(logger) {}

std::vector<const Graph::Single *>
PartialMatch::lhs(const std::vector<const Graph::Single *>& graphs) const {
	std::unordered_set<size_t> usedHostIds;
	for (const auto& cm :  componentMatches) {
		usedHostIds.insert(cm->getHostId());
	}

	std::vector<const Graph::Single *> lhs;
	for (size_t hid : usedHostIds) {
		lhs.push_back(graphs[hid]);
	}

	return lhs;
}

bool PartialMatch::push(const ComponentMatch& cm, lib::Rules::GraphAsRuleCache &graphAsRule) {
	const auto& gPatterns = getPatternsGraph();
	const auto& gHosts = getHostsGraph();

	componentMatches.push_back(&cm);

	const auto &gp = get_component_graph(cm.patternId,
	                                     get_labelled_left(rPatterns.getDPORule()));
	bool isCanon = canonMatch.push(cm, graphAsRule);
	for (const auto vp : asRange(vertices(gp))) {
		assert(get(map, gPatterns, gHosts, vp) == getHostsNullVertex());
		const auto vh = cm[vp];
		assert(vh != getHostsNullVertex());
		if (get_inverse(map, gPatterns, gHosts, vh) != getPatternsNullVertex()) {
			return false;
		}
		put(map, gPatterns, gHosts, vp, vh);
	}
	bool isValid = isValidComponentMap(rHosts.getDPORule(),
	                           rPatterns.getDPORule(),
	                           cm.patternId,
	                           map,
	                           logger);
	return isValid && isCanon;
}

void PartialMatch::pop() {
	assert(componentMatches.size() > 0);
	const ComponentMatch& cm = *componentMatches.back();
	const auto& gPatterns = getPatternsGraph();
	const auto& gHosts = getHostsGraph();
	const auto &gp = get_component_graph(cm.getPatternId(),
	                                     get_labelled_left(rPatterns.getDPORule()));
	for (const auto vp : asRange(vertices(gp))) {
		put(map, gPatterns, gHosts, vp, getHostsNullVertex());
	}

	componentMatches.pop_back();
	canonMatch.pop();
}

bool PartialMatch::empty() const {
	return componentMatches.size() == 0;
}

std::unique_ptr<Rules::Real> PartialMatch::apply() const {
	std::unordered_set<size_t> usedHostIds;
	for (const auto& cm : componentMatches) {
		usedHostIds.insert(cm->getHostId());
	}

	std::vector<size_t> copyVertices;
	const auto &lgHosts = get_labelled_right(rHosts.getDPORule());
	for (size_t hid : usedHostIds)  {
		const auto &gh = get_component_graph(hid, lgHosts);
		for (const auto vh : asRange(vertices(gh))) {
			copyVertices.push_back(vh);
		}
	}
	using HasTerm = GraphMorphism::HasTermData<InvertibleVertexMap>;
	using HasStereo = GraphMorphism::HasStereoData<InvertibleVertexMap>;
	constexpr LabelType labelType = HasTerm::value ? LabelType::Term : LabelType::String;
	return RC::composeRuleRealByMatch<labelType, HasStereo::value>(
	            rHosts, rPatterns, map, false, logger, &copyVertices);
}


RuleApplicationMap::RuleApplicationMap(const Rules::Real& rHosts,
                                       const Rules::Real& rPatterns) :
        rHosts(rHosts), rPatterns(rPatterns),
        map(getPatternsGraph(), getHostsGraph()),
        hostCount(rHosts.getDPORule().numRightComponents, 0) {}



bool RuleApplicationMap::pushComponentMap(const ComponentMap& componentMap) {
	IO::Logger logger(std::cout);
	const auto& gPatterns = getPatternsGraph();
	const auto& gHosts = getHostsGraph();

	addedComponentMaps.push_back(&componentMap);
	hostCount[componentMap.hostId] += 1;

	assert(componentMap.m.size() > 0);

	const auto &gp = get_component_graph(componentMap.patternId,
	                                     get_labelled_left(rPatterns.getDPORule()));
	for (const auto vp : asRange(vertices(gp))) {
		assert(get(map, gPatterns, gHosts, vp) == getHostsNullVertex());
		const auto vh = get(componentMap.m, gPatterns, gHosts, vp);
		assert(vh != getHostsNullVertex());
		if (get_inverse(map, gPatterns, gHosts, vh) != getPatternsNullVertex()) {
			return false;
		}
		put(map, gPatterns, gHosts, vp, vh);
	}
	return isValidComponentMap(rHosts.getDPORule(),
	                           rPatterns.getDPORule(),
	                           componentMap.patternId,
	                           map,
	                           logger);

}

void RuleApplicationMap::popComponentMap() {
	const ComponentMap& componentMap = *addedComponentMaps.back();
	const auto& gPatterns = getPatternsGraph();
	const auto& gHosts = getHostsGraph();
	const auto &gp = get_component_graph(componentMap.patternId,
	                                     get_labelled_left(rPatterns.getDPORule()));
	for (const auto vp : asRange(vertices(gp))) {
		put(map, gPatterns, gHosts, vp, getHostsNullVertex());
	}
	addedComponentMaps.pop_back();
	hostCount[componentMap.hostId] -= 1;
}

std::vector<size_t> RuleApplicationMap::getHostVertices() const {
	std::unordered_set<size_t> usedHostIds;
	for (const auto& cm :  addedComponentMaps) {
		usedHostIds.insert(cm->hostId);
	}

	std::vector<size_t> copyVertices;
	const auto &lgHosts = get_labelled_right(rHosts.getDPORule());
	for (size_t hid : usedHostIds)  {
		const auto &gh = get_component_graph(hid, lgHosts);
		for (const auto vh : asRange(vertices(gh))) {
			copyVertices.push_back(vh);
		}
	}

	return std::vector<size_t>(usedHostIds.begin(), usedHostIds.end());
}

std::unique_ptr<Rules::Real> RuleApplicationMap::computeDerivation(IO::Logger& logger) const {
	std::unordered_set<size_t> usedHostIds;
	for (const auto& cm :  addedComponentMaps) {
		usedHostIds.insert(cm->hostId);
	}

	std::cout << "COPY VERTICES: ";
	std::vector<size_t> copyVertices;
	const auto &lgHosts = get_labelled_right(rHosts.getDPORule());
	for (size_t hid : usedHostIds)  {
		const auto &gh = get_component_graph(hid, lgHosts);
		for (const auto vh : asRange(vertices(gh))) {
			std::cout << vh << ", ";
			copyVertices.push_back(vh);
		}
	}
	std::cout << std::endl;
	using HasTerm = GraphMorphism::HasTermData<InvertibleVertexMap>;
	using HasStereo = GraphMorphism::HasStereoData<InvertibleVertexMap>;
	constexpr LabelType labelType = HasTerm::value ? LabelType::Term : LabelType::String;
	return RC::composeRuleRealByMatch<labelType, HasStereo::value>(
	            rHosts, rPatterns, map, false, logger, &copyVertices);
}

std::unique_ptr<Rules::Real> RuleApplicationMap::getRuleState(IO::Logger& logger) const {
	using HasTerm = GraphMorphism::HasTermData<InvertibleVertexMap>;
	using HasStereo = GraphMorphism::HasStereoData<InvertibleVertexMap>;
	constexpr LabelType labelType = HasTerm::value ? LabelType::Term : LabelType::String;
	return RC::composeRuleRealByMatch<labelType, HasStereo::value>(
	            rHosts, rPatterns, map, false, logger);

}

std::vector<const Graph::Single *>
RuleApplicationMap::getLeftGraphs(const std::vector<const Graph::Single *>& graphs) const {
	std::unordered_set<size_t> usedHostIds;
	for (const auto& cm :  addedComponentMaps) {
		usedHostIds.insert(cm->hostId);
	}

	std::vector<const Graph::Single *> lhs;
	for (size_t hid = 0; hid < graphs.size(); hid++) {
		if (usedHostIds.find(hid) != usedHostIds.end()) {
			lhs.push_back(graphs[hid]);
		}
	}
	return lhs;
}


std::vector<size_t> RuleApplicationMap::getProjectedReactionCenter() const {
	using jla_boost::GraphDPO::Membership;
	const auto &gPatterns = get_graph(rPatterns.getDPORule());

	const auto &lgr = rPatterns.getDPORule();

	std::vector<size_t> out;
	for (size_t v : asRange(vertices(gPatterns)))  {
		auto vHost = get(map, getPatternsGraph(), getHostsGraph(), v);
		if (membership(lgr, v) != Membership::Context) {
			out.push_back(vHost);
		} else {
			for (auto e : asRange(out_edges(v, gPatterns))){
				if (membership(lgr, e) != Membership::Context) {
					out.push_back(vHost);
					break;
				}
			}
		}
	}
	return out;
}

const std::vector<int>& RuleApplicationMap::getHostCount() const {
	return hostCount;
}

const RuleApplicationMap::InvertibleVertexMap& RuleApplicationMap::getMap() const {
	return map;
}

RuleApplication::RuleApplication(const RuleApplicationMap& map,
                                 std::vector<const Graph::Single *> lhs,
                                 std::vector<const Graph::Single *> rhs,
                                 DG::NonHyper::Edge e) :
        map(map), lhs(lhs), rhs(rhs), hyperEdge(e) {
}

std::vector<const Graph::Single *>
RuleApplication::transition(const std::vector<const Graph::Single *>& graphs) {
	std::vector<const Graph::Single *> newGraphs;
	const auto& hostCounts = map.getHostCount();
	for (size_t hid = 0; hid < graphs.size(); hid++) {
		if (hostCounts[hid] == 0) {
			newGraphs.push_back(graphs[hid]);
		}
	}
	for (const Graph::Single *g : rhs) {
		newGraphs.push_back(g);
	}
	return newGraphs;
}


}
