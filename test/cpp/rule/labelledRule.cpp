#include <mod/Config.hpp>
#include <mod/Derivation.hpp>
#include <mod/Error.hpp>
#include <mod/graph/Graph.hpp>
#include <mod/dg/Builder.hpp>
#include <mod/dg/DG.hpp>
#include <mod/rule/Rule.hpp>
#include <mod/lib/Rules/Real.hpp>
#include <mod/lib/Rules/LabelledRule.hpp>
#include <iostream>

#undef NDEBUG

#include <cassert>

using namespace mod;

void testConnectedComponents(){
	auto r = rule::Rule::fromGMLString(R"(
	                                   rule [
	                                   ruleID "AddEdge"
	                                   left [ node [id 2 label "C"] ]
	                                   context [ node [ id 0 label "C" ] node [ id 1 label "C" ] ]
	                                   right [ edge [ source 0 target 1 label "-" ] node [id 3 label "C"]]
	                                   ]
	                                   )", false);

	const auto& labelledRule = r->getRule().getDPORule();
	assert(labelledRule.numLeftComponents == 3);
	assert(labelledRule.numRightComponents == 2);

	assert(labelledRule.leftComponentVertexToCoreVertex.size() == labelledRule.numLeftComponents);
	assert(labelledRule.rightComponentVertexToCoreVertex.size() == labelledRule.numRightComponents);

	const auto& lRule = get_labelled_left(labelledRule);
	for (size_t cid = 0; cid < labelledRule.numLeftComponents; cid++) {
		assert(num_vertices(get_graph(get_component_graph_v2(cid, lRule))) == 1);
	}

	assert(get_component_core_vertex(2, 0, lRule) == 2);

	const auto& rRule = get_labelled_right(labelledRule);
	std::cout << (get_component_core_vertex(2, 0, lRule)) << std::endl;
	std::cout << (get_component_core_vertex(0, 1, rRule)) << std::endl;
	std::cout << (get_component_core_vertex(0, 0, rRule)) << std::endl;
	std::cout << (get_component_core_vertex(1, 0, rRule)) << std::endl;
	assert(get_component_core_vertex(0, 0, rRule) == 0);
	assert(get_component_core_vertex(0, 1, rRule) == 1);
	assert(get_component_core_vertex(1, 0, rRule) == 3);


}

int main() {
	testConnectedComponents();
}
