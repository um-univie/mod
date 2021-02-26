#ifndef MOD_LIB_RULES_UTIL_HPP
#define MOD_LIB_RULES_UTIL_HPP

#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/Rules/Real.hpp>
#include <mod/lib/Graph/Properties/String.hpp>
#include <mod/lib/Graph/Properties/Stereo.hpp>
#include <mod/lib/Stereo/CloneUtil.hpp>

namespace mod::lib::Rules {


std::vector<std::unique_ptr<Graph::Single>> splitRightRule(const lib::Rules::LabelledRule &rDPO,
                                                     const LabelType labelType,
                                                     const bool withStereo) {
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
    if(rDPO.numRightComponents == 0) MOD_ABORT; // continue;
	using Vertex = lib::Graph::Vertex;
	using Edge = lib::Graph::Edge;
	using SideVertex = boost::graph_traits<lib::Rules::DPOProjection>::vertex_descriptor;
	using SideEdge = boost::graph_traits<lib::Rules::DPOProjection>::edge_descriptor;

	std::vector<GraphData> products(rDPO.numRightComponents);
	const auto &compMap = rDPO.rightComponents;
	const auto &gRight = get_right(rDPO);
	auto rpString = get_string(get_labelled_right(rDPO));
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

	if(withStereo && has_stereo(rDPO)) {
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
			const auto &lgRight = get_labelled_right(rDPO);
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
	std::vector<std::unique_ptr<Graph::Single>> out;
	for(auto &g : products) {
		// check against the database
		out.push_back(std::make_unique<lib::Graph::Single>(std::move(g.gPtr), std::move(g.pStringPtr),
		                                                  std::move(g.pStereoPtr)));
	}
	return out;
}

}



#endif
