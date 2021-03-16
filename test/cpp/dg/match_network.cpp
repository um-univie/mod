#include <mod/Config.hpp>
#include <mod/Derivation.hpp>
#include <mod/graph/Graph.hpp>
#include <mod/rule/Rule.hpp>
#include <mod/graph/Printer.hpp>
#include <mod/lib/statespace/MatchNetwork.hpp>

#include <iostream>

#undef NDEBUG

#include <cassert>

using namespace mod;

void testTreeConstruct() {
	auto rules = {
	    rule::Rule::ruleGMLString(R"(
	    rule [
	        ruleID "r1"
	        left []
	        context [
	            node [ id 0 label "A" ]
	            node [ id 1 label "A" ]
	            edge [ source 0 target 1 label "-" ]
	        ]
	        right []
	    ]
	    )", false),

	    rule::Rule::ruleGMLString(R"(
	    rule [
	        ruleID "r1B"
	        left []
	        context [
	            node [ id 0 label "A" ]
	            node [ id 1 label "A" ]
	            edge [ source 0 target 1 label "-" ]
	        ]
	        right []
	    ]
	    )", false),

	    rule::Rule::ruleGMLString(R"(
	    rule [
	        ruleID "r2"
	        left []
	        context [
	            node [ id 0 label "A" ]
	            node [ id 1 label "A" ]
	            node [ id 2 label "A" ]
	            edge [ source 0 target 1 label "-" ]
	            edge [ source 1 target 2 label "-" ]
	        ]
	        right []
	    ]
	    )", false),

	    rule::Rule::ruleGMLString(R"(
	    rule [
	        ruleID "r3"
	        left []
	        context [
	            node [ id 0 label "A" ]
	            node [ id 1 label "A" ]
	            node [ id 2 label "A" ]
	            node [ id 3 label "B" ]
	            edge [ source 0 target 1 label "-" ]
	            edge [ source 1 target 2 label "-" ]
	            edge [ source 2 target 3 label "-" ]
	        ]
	        right []
	    ]
	    )", false),

	    rule::Rule::ruleGMLString(R"(
	    rule [
	        ruleID "r1"
	        left []
	        context [
	            node [ id 0 label "A" ]
	            node [ id 1 label "B" ]
	            edge [ source 0 target 1 label "-" ]
	        ]
	        right []
	    ]
	    )", false),

	    rule::Rule::ruleGMLString(R"(
	    rule [
	        ruleID "r1"
	        left []
	        context [
	            node [ id 0 label "A" ]
	            node [ id 1 label "B" ]
	            node [ id 2 label "C" ]
	            edge [ source 0 target 1 label "-" ]
	            edge [ source 1 target 2 label "-" ]
	        ]
	        right []
	    ]
	    )", false),
	};

	std::vector<const lib::Rules::Real *> libRules;
	for (const auto& r : rules) {
		libRules.push_back(&r->getRule());
	}
	LabelSettings labelSettings = LabelSettings{LabelType::String, LabelRelation::Isomorphism};
	lib::statespace::MatchNetwork matchNetwork(libRules, labelSettings);

	std::vector<std::shared_ptr<graph::Graph>> gs = {
	    graph::Graph::graphDFS("[B][A]"),
	    graph::Graph::graphDFS("[A][A][A]"),
	};

	std::vector<const lib::Graph::Single*> libGraphs;
	for (auto g : gs)  { libGraphs.push_back(&g->getGraph()); }

	auto ruleIndices = matchNetwork.getValidRules(libGraphs);
	std::cout << "Found  valid rules: " << ruleIndices.size() << std::endl;
	for (auto idx : ruleIndices) { std::cout << idx << ", "; }
	std::cout << std::endl;
	assert(ruleIndices.size() == 4);

}

int main() {
	testTreeConstruct();
	return 0;
}
