#ifndef MOD_LIB_STEREO_CLONEUTIL_H
#define MOD_LIB_STEREO_CLONEUTIL_H

#include <mod/lib/Stereo/Configuration/Configuration.hpp>

#include <cassert>
#include <vector>

namespace mod {
namespace lib {
namespace Stereo {

template<typename LGraphDom, typename GraphCodom, typename InverseVertexMorphism, typename InverseEdgeMorphism>
struct Cloner {
	using VertexCodom = typename boost::graph_traits<GraphCodom>::vertex_descriptor;
	using EdgeCodom = typename boost::graph_traits<GraphCodom>::edge_descriptor;
public:

	Cloner(const LGraphDom &lgDom, const GraphCodom &gCodom, InverseVertexMorphism mInverseVertex, const InverseEdgeMorphism &mInverseEdge)
	: lgDom(lgDom), gCodom(gCodom), mInverseVertex(mInverseVertex), mInverseEdge(mInverseEdge) { }

	std::unique_ptr<lib::Stereo::Configuration> extractConfiguration(const VertexCodom &vCodom) const {
		const auto &gDom = get_graph(lgDom);
		const auto &pStereoDom = get_stereo(lgDom);
		const auto vDom = mInverseVertex(vCodom);
		const lib::Stereo::Configuration &conf = *pStereoDom[vDom];
		if(conf.getFixation() == lib::Stereo::Fixation::free()) {
			return conf.cloneFree(lib::Stereo::getGeometryGraph());
		} else {
			std::vector<std::size_t> offsetMap(out_degree(vDom, gDom));
			assert(out_degree(vDom, gDom) == out_degree(vCodom, gCodom));
			std::size_t offsetCodom = 0;
			for(const auto &eOutCodom : asRange(out_edges(vCodom, gCodom))) {
				const auto vAdjDom = mInverseVertex(target(eOutCodom, gCodom));
				// we don't use find_if as we want the distance, also for forward iterators
				std::size_t offsetDom = [&]() {
					std::size_t i = 0;
					for(const auto &eDomCand : asRange(out_edges(vDom, gDom))) {
						if(target(eDomCand, gDom) == vAdjDom) return i;
						++i;
					}
					assert(false);
					return i; // to prevent warnings
				}();
				offsetMap[offsetDom] = offsetCodom;
				++offsetCodom;
			}
			return conf.clone(lib::Stereo::getGeometryGraph(), offsetMap);
		}
	}

	lib::Stereo::EdgeCategory getEdgeCategory(const EdgeCodom &eCodom) const {
		const auto &pStereoDom = get_stereo(lgDom);
		const auto eDom = mInverseEdge(eCodom);
		return pStereoDom[eDom];
	}
private:
	const LGraphDom &lgDom;
	const GraphCodom &gCodom;
	InverseVertexMorphism mInverseVertex;
	InverseEdgeMorphism mInverseEdge;
};

template<typename LGraphDom, typename GraphCodom, typename InverseVertexMorphism, typename InverseEdgeMorphism>
auto makeCloner(const LGraphDom &lgDom, const GraphCodom &gCodom, InverseVertexMorphism &&mInverseVertex, InverseEdgeMorphism &&mInverseEdge) {
	return Cloner<LGraphDom, GraphCodom, InverseVertexMorphism, InverseEdgeMorphism>(lgDom, gCodom, mInverseVertex, mInverseEdge);
}

} // namespace Stereo
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_STEREO_CLONEUTIL_H */