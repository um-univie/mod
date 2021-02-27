#include <mod/lib/Rules/Application/ResultGraph.hpp>

namespace mod::lib::Rules::Application {

using LabelledHost = LabelledUnionGraph<Graph::LabelledGraph>;
using HostGraph = LabelledHost::GraphType;
using LabelledLeft = LabelledLeftGraph;
using LeftGraph = LabelledLeft::GraphType;
using RuleGraph = LabelledRule::GraphType;
using Match = jla_boost::GraphMorphism::InvertibleVectorVertexMap<LeftGraph, HostGraph>;

class ResultGraph {
public:
	using GraphType = Graph::LabelledGraph::GraphType;
	using Vertex = GraphType::vertex_descriptor;
	using PropString = Graph::LabelledGraph::PropStringType;
	using PropStereo = Graph::LabelledGraph::PropStereoType;

	ResultGraph(const LabelledRightGraph::PropStringType& stringRule,
	            const LabelledHost::PropString& stringHost) :
	    gPtr(new Graph::GraphType()),
	    pStringPtr(new PropString(*gPtr)),
	    stringRule(stringRule),
	    stringHost(stringHost) {}

	Vertex addVertex(RuleGraph::vertex_descriptor vRule,
	               HostGraph::vertex_descriptor vHost,
	               const HostGraph& gHost,
	               std::vector<Vertex>& R2H,
	               std::vector<Vertex>& G2H) {
		const auto vProductSrc = add_vertex(*gPtr);
		assert(vRule != RuleGraph::null_vertex() || vHost != HostGraph::null_vertex());
		pStringPtr->addVertex(vProductSrc, (vRule != RuleGraph::null_vertex())
		                      ?stringRule[vRule]:stringHost[vHost]);
		if (vRule != RuleGraph::null_vertex()) {
			R2H[vRule] = vProductSrc;
		}
		if (vHost != HostGraph::null_vertex()) {
			G2H[gHost.get_vertex_idx_offset(vHost.gIdx) + vHost.v] = vProductSrc;
		}
		return vProductSrc;
	}

	void addEdge(Vertex src, Vertex tar, LabelledRightGraph::GraphType::edge_descriptor edge) {
		if (src > tar) { return; }
		const auto eH = add_edge(src, tar, *gPtr);
		pStringPtr->addEdge(eH.first, stringRule[edge]);
	}

	void addEdge(Vertex src, Vertex tar, HostGraph::edge_descriptor edge) {
		if (src > tar) { return; }
		const auto eH = add_edge(src, tar, *gPtr);
		pStringPtr->addEdge(eH.first, stringHost[edge]);
	}

	static Vertex null_vertex() {
		return boost::graph_traits<GraphType>::null_vertex();
	}

public:
	std::unique_ptr<GraphType> gPtr;
	std::unique_ptr<PropString> pStringPtr;
	std::unique_ptr<PropStereo> pStereoPtr;
	//std::vector<SideVertex> vertexMap;

	const LabelledRightGraph::PropStringType &stringRule;
	const LabelledHost::PropString &stringHost;

};

std::vector<std::unique_ptr<Graph::Single>> applyMatch(const LabelledRule& rule,
                                             const LabelledHost& host,
                                             const Match& match) {
	std::vector<std::unique_ptr<Graph::Single>> products;
	std::vector<ResultGraph::Vertex> R2H(num_vertices(get_graph(rule)), ResultGraph::null_vertex());
	std::vector<ResultGraph::Vertex> G2H(num_vertices(get_graph(host)), ResultGraph::null_vertex());

	const auto &gLeft = get_graph(get_labelled_left(rule));
	const auto &gRight = get_graph(get_labelled_right(rule));
	const auto &gHost = get_graph(host);
	const auto vHostNull = boost::graph_traits<LabelledUnionGraph<Graph::LabelledGraph>::GraphType>::null_vertex();
	const auto vRuleNull = boost::graph_traits<LabelledRule::GraphType>::null_vertex();

	const auto& stringRight = get_string(get_labelled_right(rule));
	const auto& stringHost = get_string(host);

	auto host2index = [&] (HostGraph::vertex_descriptor vHost) {
		return gHost.get_vertex_idx_offset(vHost.gIdx) + vHost.v;
	};

	std::vector<std::pair<RuleGraph::vertex_descriptor, HostGraph::vertex_descriptor>> stack;


	auto handleRuleVertex = [&] (RuleGraph::vertex_descriptor vRuleSrc,
	        HostGraph::vertex_descriptor vHostSrc,
	        ResultGraph::Vertex vProductSrc,
	        ResultGraph& res) {
		if (vRuleSrc == vRuleNull) {return;}

		for (const auto eRule : asRange(out_edges(vRuleSrc, gRight))) {
			const auto vRuleTar = target(eRule, gRight);
			auto vProductTar = R2H[vRuleTar];
			if (vProductTar == ResultGraph::null_vertex()) {
				const auto vHostTar = get(match, gRight, gHost, vRuleTar);
				stack.emplace_back(vRuleTar, vHostTar);
				vProductTar = res.addVertex(vRuleTar, vHostTar, gHost, R2H, G2H);
			}
			res.addEdge(vProductSrc, vProductTar, eRule);
		}
	};

	auto handleHostVertex = [&] (RuleGraph::vertex_descriptor vRuleSrc,
	        HostGraph::vertex_descriptor vHostSrc,
	        ResultGraph::Vertex vProductSrc,
	        ResultGraph& res) {

		if (vHostSrc == vHostNull) {return;}

		for (const auto eHost : asRange(out_edges(vHostSrc, gHost))) {
			const auto vHostTar = target(eHost, gHost);
			const auto vRuleTar = get_inverse(match, gLeft, gHost, vHostTar);
			auto vProductTar = G2H[host2index(vHostTar)];
			bool hasRuleEdge = false;

			if (vRuleSrc != vRuleNull && vRuleTar != vRuleNull) {
				const auto eRule = edge(vRuleSrc, vRuleTar, gLeft);
				if (membership(rule, vRuleTar) == Membership::Left ||
				        (eRule.second && membership(rule, eRule.first) == Membership::Left)) {
					continue;
				}
				hasRuleEdge = eRule.second;
			}

			if (vProductTar == ResultGraph::null_vertex()) {
				stack.emplace_back(vRuleTar, vHostTar);
				vProductTar = res.addVertex(vRuleTar, vHostTar, gHost, R2H, G2H);
			}

			if (!hasRuleEdge) {
				res.addEdge(vProductSrc, vProductTar, eHost);
			}
		}

	};

	for (const auto vRight : asRange(vertices(gRight))) {
//		std::cout << "TRYING " << vRight << std::endl;
		if (R2H[vRight] != ResultGraph::null_vertex()) {
			continue;
		}

		ResultGraph p(stringRight, stringHost);
		stack.emplace_back(vRight, get(match, gLeft, gHost, vRight));
		p.addVertex(vRight, stack.back().second, gHost, R2H, G2H);
		while (!stack.empty()) {
			const auto [vSrcRule, vSrcHost] = stack.back();
			stack.pop_back();

			assert(vSrcRule != vRuleNull || vSrcHost != vHostNull);
			const auto vSrcResult = (vSrcRule != vRuleNull)	? R2H[vSrcRule] : G2H[host2index(vSrcHost)];
			assert(vSrcResult != ResultGraph::null_vertex());
			handleRuleVertex(vSrcRule, vSrcHost, vSrcResult, p);
			handleHostVertex(vSrcRule, vSrcHost, vSrcResult, p);
		}
		products.push_back(std::make_unique<lib::Graph::Single>(std::move(p.gPtr), std::move(p.pStringPtr),
			                                              std::move(p.pStereoPtr)));
	}
	return products;
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

std::vector<std::unique_ptr<Graph::Single>> applyMatch2(const LabelledRule& rule,
			                                 const LabelledUnionGraph<Graph::LabelledGraph>& host,
			                                 const jla_boost::GraphMorphism::InvertibleVectorVertexMap<Rules::LabelledRule::LeftGraphType, LabelledUnionGraph<Graph::LabelledGraph>::GraphType>&  morphism) {

	struct GraphData {
		using GraphType = Graph::LabelledGraph::GraphType;
		using Vertex = GraphType::vertex_descriptor;
		using PropString = Graph::LabelledGraph::PropStringType;
		using PropStereo = Graph::LabelledGraph::PropStereoType;
	public:
		GraphData() :
		    gPtr(new Graph::GraphType()),
		    pStringPtr(new PropString(*gPtr)) {}

		static Vertex null_vertex() {
			return boost::graph_traits<GraphType>::null_vertex();
		}

	public:
		std::unique_ptr<GraphType> gPtr;
		std::unique_ptr<PropString> pStringPtr;
		std::unique_ptr<PropStereo> pStereoPtr;
		//std::vector<SideVertex> vertexMap;
	};

	std::vector<GraphData> products;
	std::vector<GraphData::Vertex> R2H(num_vertices(get_graph(rule)), GraphData::null_vertex());
	std::vector<GraphData::Vertex> G2H(num_vertices(get_graph(host)), GraphData::null_vertex());

	std::vector<LabelledUnionGraph<Graph::LabelledGraph>::GraphType::vertex_descriptor> hostStack;
	std::vector<LabelledRule::GraphType::vertex_descriptor> ruleStack;

	const auto &gLeft = get_graph(get_labelled_left(rule));
	const auto &gRight = get_graph(get_labelled_right(rule));
	const auto &gHost = get_graph(host);
	const auto vHostNull = boost::graph_traits<LabelledUnionGraph<Graph::LabelledGraph>::GraphType>::null_vertex();
	const auto vRuleNull = boost::graph_traits<LabelledRule::GraphType>::null_vertex();

	const auto& stringRight = get_string(get_labelled_right(rule));
	const auto& stringHost = get_string(host);
	for (const auto vRight : asRange(vertices(gRight))) {
//		std::cout << "TRYING " << vRight << std::endl;
		if (R2H[vRight] != GraphData::null_vertex()) {
			continue;
		}

		products.emplace_back();
		ruleStack.push_back(vRight);
		const auto& p = products.back();
		while (!ruleStack.empty() || !hostStack.empty()) {
			while (!ruleStack.empty()) {
				const auto vRuleSrc = ruleStack.back();
				ruleStack.pop_back();
//				std::cout << "		ruleStack: POPPING " << vRuleSrc << ", " << ruleStack.size() << std::endl;

				if (R2H[vRuleSrc] != GraphData::null_vertex()) {
					continue;
				}

				const auto vHostSrc = get(morphism, gLeft, gHost, vRuleSrc);

				const auto vProductSrc = add_vertex(*p.gPtr);
				p.pStringPtr->addVertex(vProductSrc, stringRight[vRuleSrc]);
				R2H[vRuleSrc] = vProductSrc;
				if (vHostSrc != vHostNull) {
					G2H[gHost.get_vertex_idx_offset(vHostSrc.gIdx) + vHostSrc.v] = vProductSrc;
				}
//				std::cout << "			> added  vertex: " << vProductSrc << ", " << vHostSrc << std::endl;


//				std::cout << "			> vRuleSrc Range: " <<std::endl;;
				for (const auto eRule : asRange(out_edges(vRuleSrc, gRight))) {
					const auto vRuleTar = target(eRule, gRight);
					const auto vProductTar = R2H[vRuleTar];
					if (vProductTar == GraphData::null_vertex()) {
						ruleStack.push_back(vRuleTar);
					} else if (vProductTar != GraphData::null_vertex()){
						//assert(!edge(vProductSrc, vProductTar, *p.gPtr).second);
						auto eProduct = add_edge(vProductSrc, vProductTar, *p.gPtr);
						assert(eProduct.second);
						p.pStringPtr->addEdge(eProduct.first, stringRight[eRule]);
//						std::cout << "				> added  edge: " << vProductSrc << ", " << vProductTar << std::endl;
					}
				}
				if (vHostSrc == vHostNull) {
					continue;
				}

//				std::cout << "			> vHostSrc Range: " <<std::endl;;
				for (const auto eHost : asRange(out_edges(vHostSrc, gHost))) {
					const auto vHostTar = target(eHost, gHost);
					const auto vRuleTar = get_inverse(morphism, gLeft, gHost, vHostTar);
					const auto vProductTar = G2H[gHost.get_vertex_idx_offset(vHostTar.gIdx) + vHostTar.v];
					if (vRuleTar != vRuleNull) {
						const auto eRule = edge(vRuleSrc, vRuleTar, gLeft);
						if (membership(rule, vRuleTar) == Membership::Left ||
						        (eRule.second && membership(rule, eRule.first) == Membership::Left)) {
							continue;
						} else if (vProductTar == GraphData::null_vertex()) {
							ruleStack.push_back(vRuleTar);
							continue;
						}

						if (!eRule.second) {
							auto eProduct = add_edge(vProductSrc, vProductTar, *p.gPtr);
							assert(eProduct.second);
							p.pStringPtr->addEdge(eProduct.first, stringHost[eHost]);
//							std::cout << "				> added  edge: " << vProductSrc << ", " << vProductTar << std::endl;
						}
					} else {
						if (vProductTar == GraphData::null_vertex()) {
							hostStack.push_back(vHostTar);
							continue;
						}
						auto eProduct = add_edge(vProductSrc, vProductTar, *p.gPtr);
						assert(eProduct.second);
						p.pStringPtr->addEdge(eProduct.first, stringHost[eHost]);
//						std::cout << "				> added  edge: " << vProductSrc << ", " << vProductTar << std::endl;
					}
				}
			} // while ruleStack

			while (!hostStack.empty()) {
				const auto vHostSrc = hostStack.back();
				hostStack.pop_back();
				assert(get_inverse(morphism, gLeft, gHost, vHostSrc) == vRuleNull);
//				std::cout << "		hostStack: POPPING " << vHostSrc << ", " << hostStack.size() << std::endl;

				if (G2H[gHost.get_vertex_idx_offset(vHostSrc.gIdx) + vHostSrc.v] != GraphData::null_vertex()) {
					continue;
				}

				const auto vProductSrc = add_vertex(*p.gPtr);
				p.pStringPtr->addVertex(vProductSrc, stringHost[vHostSrc]);
				G2H[gHost.get_vertex_idx_offset(vHostSrc.gIdx) + vHostSrc.v] = vProductSrc;
//				std::cout << "			> added  vertex: " << vProductSrc << std::endl;;

				for (const auto eHost : asRange(out_edges(vHostSrc, gHost))) {
					const auto vHostTar = target(eHost, gHost);
					const auto vRuleTar = get_inverse(morphism, gLeft, gHost, vHostTar);
					const auto vProductTar = G2H[gHost.get_vertex_idx_offset(vHostTar.gIdx) + vHostTar.v];
					if (vRuleTar != vRuleNull) {
						if (membership(rule, vRuleTar) == Membership::Left) {
							continue;
						} else if (vProductTar == GraphData::null_vertex()) {
							ruleStack.push_back(vRuleTar);
							continue;
						}

						auto eProduct = add_edge(vProductSrc, vProductTar, *p.gPtr);
						assert(eProduct.second);
						p.pStringPtr->addEdge(eProduct.first, stringHost[eHost]);
					} else {
						if (vProductTar == GraphData::null_vertex()) {
							hostStack.push_back(vHostTar);
							continue;
						}
						auto eProduct = add_edge(vProductSrc, vProductTar, *p.gPtr);
						assert(eProduct.second);
						p.pStringPtr->addEdge(eProduct.first, stringHost[eHost]);
					}
				}

			}
		}


	}
	std::vector<std::unique_ptr<Graph::Single>> out;
	for(auto &g : products) {
		//out.emplace_back(std::move(g.gPtr), std::move(g.pStringPtr), std::move(g.pStereoPtr));
		out.push_back(std::make_unique<lib::Graph::Single>(std::move(g.gPtr), std::move(g.pStringPtr),
		                                                  std::move(g.pStereoPtr)));
	}
	return out;


}

}
