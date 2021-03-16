#include <mod/lib/statespace/MatchNetwork.hpp>
#include <mod/lib/Graph/Properties/String.hpp>
#include <mod/lib/Stereo/CloneUtil.hpp>
#include <mod/lib/Graph/Properties/Stereo.hpp>
#include <mod/lib/Graph/Collection.hpp>
#include <mod/Config.hpp>
#include <mod/graph/Graph.hpp>
#include <iostream>

namespace mod::lib::statespace {

bool hasNonTrivialPath(MatchNetwork::Vertex src, MatchNetwork::Vertex tar,
                       const std::vector<std::vector<MatchNetwork::Vertex>>& neigbors) {
	std::vector<bool> seen(neigbors.size(), false);
	std::vector<MatchNetwork::Vertex> stack = {src};
	seen[src] = true;

	while (stack.size() > 0) {
		MatchNetwork::Vertex v = stack.back();
		stack.pop_back();
		for (MatchNetwork::Vertex w : neigbors[v]) {
			if (w == tar && v != src) {
				return true;
			} else if (!seen[w] && w != tar) {
				stack.push_back(w);
				seen[w] = true;
			}
		}
	}

	return false;

}

std::vector<std::shared_ptr<graph::Graph>> splitMatchGraph(const Rules::Real& rule,
                                                            LabelSettings labelSettings) {
	struct GraphData {
		using SideVertex = boost::graph_traits<lib::Rules::DPOProjection>::vertex_descriptor;
	public:
		GraphData() : gPtr(new lib::Graph::GraphType()), pStringPtr(new lib::Graph::PropString(*gPtr)) {}

	public:
		std::unique_ptr<lib::Graph::GraphType> gPtr;
		std::unique_ptr<lib::Graph::PropString> pStringPtr;
		std::unique_ptr<lib::Graph::PropStereo> pStereoPtr;
		std::vector<SideVertex> vertexMap;
	};

	using Vertex = lib::Graph::Vertex;

	std::vector<std::shared_ptr<graph::Graph>> out;

	const auto& rDPO = rule.getDPORule();

	std::vector<GraphData> products(rDPO.numLeftComponents);
	const auto &compMap = rDPO.leftComponents;
	const auto &gRight = get_left(rDPO);
	auto rpString = get_string(get_labelled_left(rDPO));
	assert(num_vertices(gRight) == num_vertices(get_graph(rDPO)));
	std::vector<Vertex> vertexMap(num_vertices(gRight));
	for(const auto vSide : asRange(vertices(gRight))) {
		const auto comp = compMap[get(boost::vertex_index_t(), gRight, vSide)];
		auto &p = products[comp];
		const auto v = add_vertex(*p.gPtr);
		vertexMap[get(boost::vertex_index_t(), gRight, vSide)] = v;
		p.pStringPtr->addVertex(v, rpString[vSide]);
	}
	for(const auto eSide : asRange(edges(gRight))) {
		const auto vSideSrc = source(eSide, gRight);
		const auto vSideTar = target(eSide, gRight);
		const auto comp = compMap[get(boost::vertex_index_t(), gRight, vSideSrc)];
		assert(comp == compMap[get(boost::vertex_index_t(), gRight, vSideTar)]);
		const auto vCompSrc = vertexMap[get(boost::vertex_index_t(), gRight, vSideSrc)];
		const auto vCompTar = vertexMap[get(boost::vertex_index_t(), gRight, vSideTar)];
		const auto epComp = add_edge(vCompSrc, vCompTar, *products[comp].gPtr);
		assert(epComp.second);
		products[comp].pStringPtr->addEdge(epComp.first, rpString[eSide]);
	}

	if (labelSettings.withStereo && has_stereo(rDPO)) {
		// make the inverse vertex maps
		for(auto &p : products)
			p.vertexMap.resize(num_vertices(*p.gPtr));
		for(const auto vSide : asRange(vertices(gRight))) {
			const auto comp = compMap[get(boost::vertex_index_t(), gRight, vSide)];
			auto &p = products[comp];
			const auto v = vertexMap[get(boost::vertex_index_t(), gRight, vSide)];
			p.vertexMap[get(boost::vertex_index_t(), *p.gPtr, v)] = vSide;
		}

		for(auto &p : products) {
			const auto &lgRight = get_labelled_left(rDPO);
			assert(has_stereo(lgRight));
			const auto vertexMap = [&p](const auto &vProduct) {
				return p.vertexMap[get(boost::vertex_index_t(), *p.gPtr, vProduct)];
			};
			const auto edgeMap = [&p, &lgRight](const auto &eProduct) {
				const auto &g = *p.gPtr;
				const auto &gSide = get_graph(lgRight);
				const auto vSrc = source(eProduct, g);
				const auto vTar = target(eProduct, g);
				const auto vSrcSide = p.vertexMap[get(boost::vertex_index_t(), g, vSrc)];
				const auto vTarSide = p.vertexMap[get(boost::vertex_index_t(), g, vTar)];
				const auto epSide = edge(vSrcSide, vTarSide, gSide);
				assert(epSide.second);
				return epSide.first;
			};
			const auto inf = Stereo::makeCloner(lgRight, *p.gPtr, vertexMap, edgeMap);
			p.pStereoPtr = std::make_unique<lib::Graph::PropStereo>(*p.gPtr, inf);
		} // end foreach product
	} // end of stereo prop
	// wrap them
	for(auto &g : products) {
		// check against the database
		auto gSingle = std::make_unique<lib::Graph::Single>(std::move(g.gPtr), std::move(g.pStringPtr),
		                                                  std::move(g.pStereoPtr));
		out.push_back(graph::Graph::makeGraph(std::move(gSingle)));
	}
	return out;
}

MatchNetwork::MatchNetwork(const std::vector<const Rules::Real *>& rules,
                           LabelSettings labelSettings): rules(rules),
        graphDB(labelSettings, getConfig().graph.isomorphismAlg.get()),
        labelSettings(labelSettings) {
	std::unordered_map<const Graph::Single *, MatchNetwork::Vertex> graph2vertex;

	int duplicateComponents = 0;
	// Create vertices
	for (size_t ruleIdx = 0; ruleIdx < rules.size(); ruleIdx++) {
		const Rules::Real& rule = *rules[ruleIdx];
		std::vector<std::shared_ptr<graph::Graph>> patternGraphs = splitMatchGraph(rule, labelSettings);
		for (size_t patternIdx = 0; patternIdx < patternGraphs.size(); patternIdx++) {
			std::pair<std::shared_ptr<graph::Graph>, bool>  res = graphDB.tryInsert(patternGraphs[patternIdx]);
			Vertex v = boost::graph_traits<GraphType>::null_vertex();
			if (res.second) {
				assert(graph2vertex.find(&res.first->getGraph()) == graph2vertex.end());
				v = add_vertex(graph);
				graph[v].graph = &res.first->getGraph();
				graph2vertex[&res.first->getGraph()] = v;
			} else {
				assert(graph2vertex.find(&res.first->getGraph()) != graph2vertex.end());
				v = graph2vertex[&res.first->getGraph()];
				duplicateComponents  += 1;
			}
			graph[v].patternIndices.push_back(RulePatternIdx{ruleIdx, patternIdx});
		}
	}

	std::vector<std::vector<Vertex>> neighbors(num_vertices(graph));

	//  Create edges
	for (auto v : asRange(vertices(graph))) {
		std::vector<Vertex> children;
		for (auto w : asRange(vertices(graph))) {
			if (v == w) { continue; }
			if (Graph::Single::monomorphism(*graph[v].graph, *graph[w].graph, 1, labelSettings)) {
				neighbors[v].push_back(w);
			}
		}
	}

	// Compute transitive reduction
	int duplicateEdges = 0;
	for (auto src : asRange(vertices(graph))) {
		for (auto tar : neighbors[src]) {
			if (hasNonTrivialPath(src, tar, neighbors)) {
				duplicateEdges += 1;
				continue;
			}

			add_edge(src, tar, graph);
		}
	}

	double avgOutDegree = 0;
	double avgInDegree = 0;

	// Find roots
	for (auto v : asRange(vertices(graph))) {
		if (in_degree(v, graph) == 0) {
			roots.push_back(v);
		}
		avgOutDegree += out_degree(v, graph);
		avgInDegree += in_degree(v, graph);
	}
	avgInDegree /= num_vertices(graph);
	avgOutDegree /= num_vertices(graph);

	std::cout << "MatchNetwork(|V|=" << num_vertices(graph) << ", |E|=" << num_edges(graph) << "), roots=" << roots.size() << std::endl;
	std::cout << "dupVerts=" << duplicateComponents << ", dupEdges=" << duplicateEdges << ", avgInDegree=" << avgInDegree << ", avgOutDegree=" << avgOutDegree << std::endl;


}

MatchNetwork::~MatchNetwork() = default;

bool MatchNetwork::containsPattern(Vertex v, const Graph::Single *host) {
	if (graph[v].isEmbeddable.find(host) != graph[v].isEmbeddable.end())  {
		return graph[v].isEmbeddable[host];
	} else {
		bool res = Graph::Single::monomorphism(*graph[v].graph, *host, 1, labelSettings) > 0;
		graph[v].isEmbeddable[host] = res;
		return res;
	}
}

std::vector<size_t> MatchNetwork::getValidRules(const std::vector<const Graph::Single *>& hosts) {
	std::vector<size_t> out;
	std::vector<bool> seen(num_vertices(graph), false);
	std::vector<std::pair<Vertex, std::vector<const Graph::Single*>>> stack, stackNext;
	for (auto v : roots) {
		stack.push_back(std::make_pair(v, hosts));
	}
	std::vector<bool> isValidRule(rules.size(), true);
	std::vector<int> matchedComponents(rules.size(), 0);

	while (stack.size() > 0) {
		auto [v, filteredHosts] = stack.back();
		stack.pop_back();
		bool hasValidRule = false;
		for (auto idx : graph[v].patternIndices) {
			if (isValidRule[idx.ruleIdx]) {
				hasValidRule = true;
				break;
			}
		}

		if (!hasValidRule) {
			for (auto e : asRange(out_edges(v, graph))) {
				Vertex w = target(e, graph);
				if (!seen[w]) {
					stackNext.push_back(std::make_pair(w, filteredHosts));
					seen[w] = true;
				}
			}
		} else {
			auto newFilteredHosts = filteredHosts;
			while (newFilteredHosts.size() > 0) {
				const Graph::Single *host = newFilteredHosts.back();
				if (containsPattern(v, host)) {
					break;
				}
				newFilteredHosts.pop_back();
			}
			if (newFilteredHosts.size() == 0) {
				for (auto idx : graph[v].patternIndices) {
					isValidRule[idx.ruleIdx] = false;
				}
			} else {
				for (auto idx : graph[v].patternIndices) {
					matchedComponents[idx.ruleIdx] += 1;
					if (matchedComponents[idx.ruleIdx] == rules[idx.ruleIdx]->getDPORule().numLeftComponents) {
						out.push_back(idx.ruleIdx);
					}
				}
				for (auto e : asRange(out_edges(v, graph))) {
					Vertex w = target(e, graph);
					if (!seen[w]) {
						stackNext.push_back(std::make_pair(w, newFilteredHosts));
						seen[w] = true;
					}
				}
			}
		}
		if (stack.size() == 0) {
			std::swap(stack, stackNext);
		}
	}

	return out;
}

}
