#include <mod/lib/Rules/Application/PartialMatch.hpp>
#include <mod/lib/Graph/LabelledGraph.hpp>
#include <mod/lib/RC/ComposeRuleRealGeneric.hpp>
#include <mod/lib/Rules/GraphToRule.hpp>
#include <iostream>

#include <mod/lib/Graph/Properties/String.hpp>
#include <mod/lib/Graph/Properties/Stereo.hpp>
#include <mod/lib/Graph/LabelledGraph.hpp>
#include <mod/lib/LabelledUnionGraph.hpp>
#include <mod/lib/Rules/GraphToRule.hpp>


namespace mod::lib::Rules::Application {

PartialMatch::PartialMatch(const Rules::Real& rule): rule(rule),
    morphism(get_left(rule.getDPORule()), get_graph(hosts)),
    logger(std::cout) {}

size_t PartialMatch::updateHostGraph(const ComponentMatch& cm) {
	std::pair<const Graph::Single*, size_t> hostKey = std::make_pair(cm.host, cm.graphInstance);

	auto it = hostIndexMap.find(hostKey);
	addedGraph.push_back(it == hostIndexMap.end());
	std::cout << "updating new graph " << "(" << cm.host << ", " << cm.graphInstance << ")" << std::endl;
	if (addedGraph.back()) {
		size_t idx = get_graph(hosts).size();
		hostIndexMap[hostKey] = idx;
		hosts.push_back(&cm.host->getLabelledGraph());
		lhs.push_back(cm.host);
		morphism.resizeRight(get_left(rule.getDPORule()), get_graph(hosts));
		std::cout << "added new graph " << idx << " (" << cm.host << ", " << cm.graphInstance << ")" << std::endl;
		return idx;
	} else {
		return it->second;
	}
}

std::pair<bool, bool> PartialMatch::tryPush(const ComponentMatch& cm) {
	assert(!isFull());

	compMatches.push_back(cm);

	const auto& lRule = get_labelled_left(rule.getDPORule());
	const auto& gCorePattern = get_graph(get_labelled_left(rule.getDPORule()));
	const auto& gCoreHost = get_graph(hosts);

	// update host graph if necesarry
	size_t hostIndex = updateHostGraph(cm);
	assert(hostIndex < get_graph(hosts).size());


	const auto& compPatternGraph = get_graph(get_component_graph_v2(cm.componentIndex, lRule));
	const auto& compHostGraph = get_graph(cm.host->getLabelledGraph());

	for (auto vCompPattern : asRange(vertices(compPatternGraph))) {
		// get the core pattern vertex and ensure it is not already mapped in the partial match
		auto vCorePattern = get_component_core_vertex(cm.componentIndex, vCompPattern, lRule);
		assert(get(morphism, gCorePattern, gCoreHost, vCorePattern) == boost::graph_traits<HostGraph>::null_vertex());

		//  get the union graph vertex and ensure the component match maps vCompPattern
		auto vCompHost = get(*cm.morphism, compPatternGraph, compHostGraph, vCompPattern);
		assert(vCompHost != boost::graph_traits<ComponentMatch::GraphType>::null_vertex());
		auto vCoreHost = HostGraph::vertex_descriptor{hostIndex, vCompHost};

		// ensure morphism will be injective
		if (get_inverse(morphism, gCorePattern, gCoreHost, vCoreHost) != boost::graph_traits<MatchGraph>::null_vertex()) {
			this->pop();
			return std::make_pair(false, false);
		}
		put(morphism, gCorePattern, gCoreHost, vCorePattern, vCoreHost);
	}

	return std::make_pair(true, addedGraph.back());
}

void PartialMatch::pop() {
	assert(compMatches.size() > 0);
	const auto& cm = compMatches.back();


	if (addedGraph.back()) {
		std::cout << "removing graph" << std::endl;
		hosts.pop_back();
		lhs.pop_back();
		morphism.resizeRight(get_left(rule.getDPORule()), get_graph(hosts));

		std::pair<const Graph::Single*, size_t> hostKey = std::make_pair(cm.host, cm.graphInstance);
		auto it = hostIndexMap.find(hostKey);
		std::cout << "popping map " << "(" << cm.host << ", " << cm.graphInstance << ")" << std::endl;
		assert(it != hostIndexMap.end());
		hostIndexMap.erase(it);
	}

	addedGraph.pop_back();
	compMatches.pop_back();
}

bool PartialMatch::isFull() const {
	return compMatches.size() == rule.getDPORule().numLeftComponents;
}

bool PartialMatch::lastPushIsNewInstance() const {
	return addedGraph.back();
}

const std::vector<ComponentMatch>& PartialMatch::getCompMatches() const {
	return compMatches;
}

const std::vector<const Graph::Single*>& PartialMatch::getLhs() const {
	return lhs;
}

std::unique_ptr<Rules::Real> PartialMatch::apply() const {
	const std::unique_ptr<Rules::Real> rHost = lib::Rules::graphToRule(hosts, lib::Rules::Membership::Right, "G");
	using RuleHostGraph = Rules::LabelledRule::RightGraphType;
	using RuleMorphism = jla_boost::GraphMorphism::InvertibleVectorVertexMap<MatchGraph, RuleHostGraph>;

	const auto& gLeft = get_left(rule.getDPORule());
	const auto& gRuleHost = get_right(rHost->getDPORule());
	const auto& ugHost = get_graph(hosts);
	RuleMorphism ruleMorphism(gLeft, gRuleHost);
	for (auto vLeft : asRange(vertices(gLeft))) {
		auto vUGHost = get(morphism, gLeft, ugHost, vLeft);
		auto vRuleHost = ugHost.get_vertex_idx_offset(vUGHost.gIdx) + vUGHost.v;
		put(ruleMorphism, gLeft, gRuleHost, vLeft, vRuleHost);
	}


	using HasTerm = GraphMorphism::HasTermData<RuleMorphism>;
	using HasStereo = GraphMorphism::HasStereoData<RuleMorphism>;
	constexpr LabelType labelType = HasTerm::value ? LabelType::Term : LabelType::String;
	return RC::composeRuleRealByMatch<labelType, HasStereo::value>(
	            *rHost, rule, ruleMorphism, false, logger);


}

std::ostream& operator << (std::ostream& os, const PartialMatch& pm) {
	const auto& gLeft = get_left(pm.rule.getDPORule());
	const auto& ugHost = get_graph(pm.hosts);
	os << "PartialMatch(";
	for (auto vLeft : asRange(vertices(gLeft))) {
		auto vUGHost = get(pm.morphism, gLeft, ugHost, vLeft);
		os << " " << vLeft << ":";
		if (vUGHost == boost::graph_traits<PartialMatch::HostGraph>::null_vertex()) {
			os << "-1";
		} else {
			os << ugHost.get_vertex_idx_offset(vUGHost.gIdx) + vUGHost.v;
		}
	}
	os << " )";
	return os;
}

}
