#include <mod/lib/Rules/Application/CanonRule.hpp>

#include <mod/lib/Rules/LabelledRule.hpp>

#include <graph_canon/aut/implicit_size_2.hpp>
#include <graph_canon/aut/pruner_basic.hpp>
#include <graph_canon/canonicalization.hpp>
#include <graph_canon/compare.hpp>
#include <graph_canon/edge_handler/all_equal.hpp>
#include <graph_canon/invariant/cell_split.hpp>
#include <graph_canon/invariant/partial_leaf.hpp>
#include <graph_canon/invariant/quotient.hpp>
#include <graph_canon/refine/WL_1.hpp>
#include <graph_canon/refine/degree_1.hpp>
#include <graph_canon/target_cell/flm.hpp>
#include <graph_canon/tree_traversal/dfs.hpp>
#include <graph_canon/tree_traversal/bfs-exp.hpp>
#include <graph_canon/visitor/debug.hpp>
#include <graph_canon/visitor/stats.hpp>

#include <perm_group/permutation/built_in.hpp>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <vector>
#include <iostream>

namespace mod::lib::Rules {

struct GraphData {
	using SideVertex = boost::graph_traits<LabelledRule::LabelledLeftType::GraphType>::vertex_descriptor;
public:
	GraphData() : gPtr(new lib::Rules::GraphType()),
	    pStringPtr(new lib::Rules::LabelledRule::PropStringType(*gPtr)) {}

public:
	std::unique_ptr<lib::Rules::GraphType> gPtr;
	std::unique_ptr<lib::Rules::LabelledRule::PropStringType> pStringPtr;
};

GraphData makeCanonizableRule(const LabelledRule& rDPO) {
	const auto& gGraph = get_graph(rDPO);

	using jla_boost::GraphDPO::Membership;

	GraphData data;
	auto& outGraph = *data.gPtr;
	auto& outString = *data.pStringPtr;

	for (auto v : asRange(vertices(gGraph))) {
		auto outV = add_vertex(outGraph);
		assert(v == outV);
		std::string leftStr = "";
		std::string rightStr = "";
		outGraph[outV].membership = Membership::Context;
		if (membership(rDPO, v) == Membership::Left) {
			leftStr = rDPO.pString->getLeft()[v];
		} else if (membership(rDPO, v) == Membership::Right) {
			rightStr = rDPO.pString->getRight()[v];
		} else {
			leftStr = rDPO.pString->getLeft()[v];
			rightStr = rDPO.pString->getRight()[v];
		}
		outString.add(outV, leftStr, rightStr);
	}

	for (auto e : asRange(edges(gGraph))) {
		auto src = source(e, gGraph);
		auto tar = target(e, gGraph);


		auto outV = add_vertex(outGraph);
		outGraph[outV].membership = Membership::Context;

		std::string leftStr = "";
		std::string rightStr = "";
		if (membership(rDPO, e) == Membership::Left) {
			leftStr = rDPO.pString->getLeft()[e];
		} else if (membership(rDPO, e) == Membership::Right) {
			rightStr = rDPO.pString->getRight()[e];
		} else {
			leftStr = rDPO.pString->getLeft()[e];
			rightStr = rDPO.pString->getRight()[e];
		}
		outString.add(outV, leftStr, rightStr);
		auto srcE = add_edge(src, outV, outGraph).first;
		outString.add(srcE, "-", "-");
		auto tarE = add_edge(outV, tar, outGraph).first;
		outString.add(tarE, "-", "-");

	}

	return data;
}

std::unique_ptr<AutGroup> getRuleAutomorphisms(const LabelledRule& r) {
	GraphData data = makeCanonizableRule(r);

	auto can = graph_canon::canonicalizer<int, graph_canon::edge_handler_all_equal,
	        false, false>(graph_canon::edge_handler_all_equal());

	const auto &graph = *data.gPtr;
	const auto idx = get(boost::vertex_index_t(), graph);
	const auto vis = graph_canon::make_visitor(
	        graph_canon::traversal_bfs_exp(), graph_canon::target_cell_flm(), graph_canon::refine_WL_1(),
	        graph_canon::aut_pruner_basic(), graph_canon::aut_implicit_size_2(), graph_canon::refine_degree_1(),
	        graph_canon::invariant_partial_leaf(), graph_canon::invariant_cell_split(), graph_canon::invariant_quotient()
	        //			, debug_visitor(g.getLabelledGraph())
	        , graph_canon::stats_visitor()
	);
	const auto &str = *data.pStringPtr;
	const auto vLess = [&str](Vertex a, Vertex b) {
		return std::make_pair(str.getLeft()[a], str.getRight()[a]) <
		        std::make_pair(str.getLeft()[b], str.getRight()[b]);
	};
	auto res = can(graph, idx, vLess, vis);
	auto autPtr = std::move(get(graph_canon::aut_pruner_basic::result_t(), res.second));



	size_t degree = num_vertices(get_graph(r));
	std::unique_ptr<AutGroup> gensRes = std::make_unique<AutGroup>(degree);
	auto gens = autPtr->generators(); // skip the first, it should be the identity
	for(const auto &p : asRange(++begin(gens), end(gens)))
		gensRes->add_generator(std::vector<int>(p.begin(), p.begin()+degree));

	return gensRes;
}



}//Rules

