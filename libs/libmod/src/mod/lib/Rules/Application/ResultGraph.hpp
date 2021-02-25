#ifndef MOD_LIB_RULES_APPLICATION_RESULT_GRAPH_HPP
#define MOD_LIB_RULES_APPLICATION_RESULT_GRAPH_HPP

#include <mod/lib/Rules/LabelledRule.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/LabelledUnionGraph.hpp>
#include <jla_boost/graph/morphism/models/Vector.hpp>
#include <mod/lib/Graph/Properties/String.hpp>
#include <mod/lib/Graph/Properties/Stereo.hpp>

#include <iostream>

namespace mod::lib::Rules::Application {

std::vector<std::unique_ptr<Graph::Single>> applyMatch(const LabelledRule& rule,
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
		std::cout << "TRYING " << vRight << std::endl;
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
				std::cout << "		ruleStack: POPPING " << vRuleSrc << ", " << ruleStack.size() << std::endl;

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
				std::cout << "			> added  vertex: " << vProductSrc << ", " << vHostSrc << std::endl;


				std::cout << "			> vRuleSrc Range: " <<std::endl;;
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
						std::cout << "				> added  edge: " << vProductSrc << ", " << vProductTar << std::endl;
					}
				}
				if (vHostSrc == vHostNull) {
					continue;
				}

				std::cout << "			> vHostSrc Range: " <<std::endl;;
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
							std::cout << "				> added  edge: " << vProductSrc << ", " << vProductTar << std::endl;
						}
					} else {
						if (vProductTar == GraphData::null_vertex()) {
							hostStack.push_back(vHostTar);
							continue;
						}
						auto eProduct = add_edge(vProductSrc, vProductTar, *p.gPtr);
						assert(eProduct.second);
						p.pStringPtr->addEdge(eProduct.first, stringHost[eHost]);
						std::cout << "				> added  edge: " << vProductSrc << ", " << vProductTar << std::endl;
					}
				}
			} // while ruleStack

			while (!hostStack.empty()) {
				const auto vHostSrc = hostStack.back();
				hostStack.pop_back();
				assert(get_inverse(morphism, gLeft, gHost, vHostSrc) == vRuleNull);
				std::cout << "		hostStack: POPPING " << vHostSrc << ", " << hostStack.size() << std::endl;

				if (G2H[gHost.get_vertex_idx_offset(vHostSrc.gIdx) + vHostSrc.v] != GraphData::null_vertex()) {
					continue;
				}

				const auto vProductSrc = add_vertex(*p.gPtr);
				p.pStringPtr->addVertex(vProductSrc, stringHost[vHostSrc]);
				G2H[gHost.get_vertex_idx_offset(vHostSrc.gIdx) + vHostSrc.v] = vProductSrc;
				std::cout << "			> added  vertex: " << vProductSrc << std::endl;;

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

#endif
