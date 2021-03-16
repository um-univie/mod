#ifndef MOD_LIB_STATESPACE_COMPONENT_MAP_HPP
#define MOD_LIB_STATESPACE_COMPONENT_MAP_HPP

#include <jla_boost/graph/morphism/models/Vector.hpp>
#include <mod/lib/Rules/LabelledRule.hpp>

#include <iostream>

namespace mod::lib::statespace {

class ComponentMap {
	// namespace GM = jla_boost::GraphMorphism;
	using Morphism = jla_boost::GraphMorphism::VectorVertexMap<Rules::LabelledRule::LeftGraphType, Rules::LabelledRule::RightGraphType>;
public:
	ComponentMap(Morphism &&m, size_t pid, size_t hid) :
	    m(std::move(m)), patternId(pid), hostId(hid) { }
	Morphism m;
	size_t patternId;
	size_t hostId;
};

class ComponentMatch {
public:
	using PatternGraph = Rules::LabelledRule::LeftGraphType;
	using HostGraph = Rules::LabelledRule::RightGraphType;
	using VertexMap = jla_boost::GraphMorphism::VectorVertexMap<PatternGraph, HostGraph>;

	ComponentMatch(size_t patternId, size_t hostId, const PatternGraph& gPattern, const HostGraph& gHost,
	               const VertexMap& match, size_t offset):
	    patternId(patternId), hostId(hostId), gPattern(gPattern), gHost(gHost), match(match), offset(offset) {}

	HostGraph::vertex_descriptor operator [] (PatternGraph::vertex_descriptor vp) const {
		auto vh = get(match, gPattern, gHost, vp);
		assert(boost::graph_traits<HostGraph>::null_vertex() != vh);
		return vh + offset;
	}

	size_t getPatternId() const {
		return patternId;
	}

	size_t getHostId() const {
		return hostId;
	}

	friend std::ostream& operator << (std::ostream& os, const ComponentMatch& cm) {
		os << "ComponentMatch(";
		for (const auto vp : asRange(vertices(cm.gPattern))) {
			auto vh = get(cm.match, cm.gPattern, cm.gHost, vp);
			if (vh == boost::graph_traits<HostGraph>::null_vertex()) {
				continue;
			}
			os << vp << " -> " << cm[vp] << ", ";
		}
		os << ")";
		return os;
	}

	size_t patternId;
	size_t hostId;
	const PatternGraph& gPattern;
	const HostGraph& gHost;
	const VertexMap& match;
	size_t offset;
};


template <typename VertexMap>
bool isValidComponentMap(const Rules::LabelledRule& rHosts,
                         const Rules::LabelledRule& rPatterns,
                         size_t patternId,
                         const VertexMap& map,
                         IO::Logger& logger) {
	    using jla_boost::GraphDPO::Membership;
	    const auto &lgPatterns = get_labelled_left(rPatterns);

		const auto &lgHosts = get_labelled_right(rHosts);
		const auto &gHosts = get_graph(rHosts);

		size_t cid = patternId;
		const auto &gp = get_component_graph(cid, lgPatterns);

		using LeftGraphType = Rules::LabelledRule::LeftGraphType;
		using RightGraphType = Rules::LabelledRule::RightGraphType;
		const auto vNullHosts = boost::graph_traits<RightGraphType>::null_vertex();
//		const auto vNullPatterns = boost::graph_traits<LeftGraphType>::null_vertex();

		const auto &gRightPatterns = get_graph(get_labelled_right(rPatterns));

//		logger.indent() << "-----" << std::endl;
		for (const auto vpSrc : asRange(vertices(gp))) {
			const auto vhSrc = get(map, get_graph(lgPatterns), get_graph(lgHosts), vpSrc);
//			logger.indent() << "vpSrc " << vpSrc << ", vhSrc"  << vhSrc << std::endl;
			assert(vhSrc != vNullHosts);
			for (auto ep :  asRange(out_edges(vpSrc, gRightPatterns))) {
				const auto vpTar = target(ep, gRightPatterns);
				const auto epMember = membership(rPatterns, ep);
				if (epMember == Membership::Context) {
					continue;
				}
				const auto vhTar = get(map, get_graph(lgPatterns), get_graph(lgHosts), vpTar);

				if (vhTar == vNullHosts) {
//					logger.indent() << "Have not mapped " << vpTar << " to host." << std::endl;
					continue;
				}

				if (edge(vhSrc, vhTar, gHosts).second) {
//					logger.indent() << "Invalid map: Results in parallel edges." << std::endl;
					return false;
				}
			}
		}
		return true;
}

}

#endif
