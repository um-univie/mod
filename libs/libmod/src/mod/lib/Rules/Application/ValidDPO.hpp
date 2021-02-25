#ifndef MOD_LIB_RULES_APPLICATION_VALIDDPO_HPP
#define MOD_LIB_RULES_APPLICATION_VALIDDPO_HPP

#include <jla_boost/graph/morphism/models/Vector.hpp>
#include <mod/lib/Rules/LabelledRule.hpp>
#include <mod/lib/LabelledUnionGraph.hpp>

#include <iostream>

namespace mod::lib::Rules::Application {

template <typename VertexMap>
bool isValidDPOMatch(const Rules::LabelledRule& rule,
                      const LabelledLeftGraph& lgPatterns,
                      size_t compIndex,
                     const LabelledUnionGraph<Graph::LabelledGraph>& host,
                         const VertexMap& map) {
	    using jla_boost::GraphDPO::Membership;
	    const auto &gPatterns = get_graph(lgPatterns);

		const auto &gHost = get_graph(host);

		const auto vNullHosts = boost::graph_traits<LabelledUnionGraph<Graph::LabelledGraph>::GraphType>::null_vertex();
//		const auto vNullPatterns = boost::graph_traits<LeftGraphType>::null_vertex();

		const auto &gRightPatterns = get_graph(get_labelled_right(rule));

		const auto &gp = get_component_graph(compIndex, lgPatterns);
//		logger.indent() << "-----" << std::endl;
		// ensure we don't create parallel edges
		for (const auto vpSrc : asRange(vertices(gp))) {
			const auto vhSrc = get(map, gPatterns, gHost, vpSrc);
			//std::cout << "vpSrc " << vpSrc << ", vhSrc "  << vhSrc << std::endl;
			assert(vhSrc != vNullHosts);

			for (auto ep :  asRange(out_edges(vpSrc, gRightPatterns))) {
				const auto vpTar = target(ep, gRightPatterns);
				const auto epMember = membership(rule, ep);
				if (epMember == Membership::Context) {
					continue;
				}

				const auto vhTar = get(map, gPatterns, gHost, vpTar);
				if (vhTar == vNullHosts) {
//					logger.indent() << "Have not mapped " << vpTar << " to host." << std::endl;
					continue;
				}

				if (edge(vhSrc, vhTar, gHost).second) {
					std::cout << "Invalid map: Results in parallel edges." << std::endl;
					return false;
				}
			}
		}
		return true;
}

template <typename LabelledHostGraph, typename VertexMap>
bool isValidDPOMatch(const Rules::LabelledRule& rule,
                      const LabelledRule::ComponentGraph_v2& lgPatterns,
                      size_t compIndex,
                     const LabelledHostGraph& host,
                         const VertexMap& map) {
	    using jla_boost::GraphDPO::Membership;
	    const auto &gPatterns = get_graph(lgPatterns);

		const auto &gHost = get_graph(host);

		const auto vNullHosts = boost::graph_traits<typename LabelledHostGraph::GraphType>::null_vertex();
//		const auto vNullPatterns = boost::graph_traits<LeftGraphType>::null_vertex();

		const auto &gRightPatterns = get_graph(get_labelled_right(rule));
		const auto &gCoreLeft = get_graph(get_labelled_left(rule));

//		logger.indent() << "-----" << std::endl;
		for (const auto vpSrc : asRange(vertices(gPatterns))) {
			const auto vhSrc = get(map, gPatterns, gHost, vpSrc);
			//std::cout << "vpSrc " << vpSrc << ", vhSrc "  << vhSrc << std::endl;
			assert(vhSrc != vNullHosts);

			// ensure we don't create parallel edges
			const auto vpSrcCore = rule.leftComponentVertexToCoreVertex[compIndex][vpSrc];
			for (auto ep :  asRange(out_edges(vpSrcCore, gRightPatterns))) {
				const auto vpTarCore = target(ep, gRightPatterns);
				const auto epMember = membership(rule, ep);
				if (epMember == Membership::Context) {
					continue;
				}

				const auto& vpTar = rule.coreVertexToLeftComponentVertex[vpTarCore];
				if (vpTar.first == boost::graph_traits<LabelledRule::ComponentGraph_v2::GraphType>::null_vertex()
				        || vpTar.first != compIndex){
					continue;
				}

				const auto vhTar = get(map, gPatterns, gHost, vpTar.second);
				if (vhTar == vNullHosts) {
//					logger.indent() << "Have not mapped " << vpTar << " to host." << std::endl;
					continue;
				}

				if (edge(vhSrc, vhTar, gHost).second) {
					//std::cout << "Invalid map: Results in parallel edges." << std::endl;
					return false;
				}
			}

			// Ensure we have no dangling edges after vertex deletion.
			const auto vMember = membership(rule, vpSrcCore);
			if (vMember != Membership::Left) {
				continue;
			}

			if (degree(vhSrc, gHost) != degree(vpSrc, gPatterns)) {
				return false;
			}
			for (auto ep :  asRange(out_edges(vpSrcCore, gCoreLeft))) {
				const auto epMember = membership(rule, ep);
				if (epMember != Membership::Left) {
					return false;
				}
			}
		}

		return true;
}

}


#endif
