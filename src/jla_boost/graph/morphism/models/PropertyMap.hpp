#ifndef JLA_BOOST_GRAPH_MORPHISM_MODELS_PROPERTY_MAP_H
#define	JLA_BOOST_GRAPH_MORPHISM_MODELS_PROPERTY_MAP_H

// This is just some convenience that wraps a map and it's inverse,
// and exposes them as an invertible vertex map.

#include <jla_boost/graph/morphism/VertexMap.hpp>

namespace jla_boost {
namespace GraphMorphism {

template<typename GraphDomT, typename GraphCodomT, typename Map, typename MapInv>
struct MatchAsVertexMap {
	using GraphDom = GraphDomT;
	using GraphCodom = GraphCodomT;
	using Storable = std::false_type;

	template<typename GraphDomU, typename GraphCodomU>
	static MatchAsVertexMap<GraphDomU, GraphCodomU, Map, MapInv>
	reinterpret(MatchAsVertexMap<GraphDom, GraphCodom, Map, MapInv> &&m, const GraphDom&, const GraphCodom&, const GraphDomU&, const GraphCodomU&) {
		return MatchAsVertexMap<GraphDomU, GraphCodomU, Map, MapInv>(m.map, m.mapInv);
	}

	MatchAsVertexMap(Map map, MapInv mapInv) : map(std::move(map)), mapInv(std::move(mapInv)) { }
private:
	Map map;
	MapInv mapInv;
public:

	friend typename boost::graph_traits<GraphCodom>::vertex_descriptor
	get(const MatchAsVertexMap<GraphDom, GraphCodom, Map, MapInv> &m, const GraphDom &gDom, const GraphCodom &gCodom,
			typename boost::graph_traits<GraphDom>::vertex_descriptor v) {
		return get(m.map, v);
	}

	friend typename boost::graph_traits<GraphDom>::vertex_descriptor
	get_inverse(const MatchAsVertexMap<GraphDom, GraphCodom, Map, MapInv> &m, const GraphDom &gDom, const GraphCodom &gCodom,
			typename boost::graph_traits<GraphCodom>::vertex_descriptor v) {
		return get(m.mapInv, v);
	}
};

template<typename GraphDom, typename GraphCodom, typename Next>
struct MatchAsVertexMapWrapper {

	MatchAsVertexMapWrapper(const GraphDom &gDom, const GraphCodom &gCodom, Next next) : gDom(gDom), gCodom(gCodom), next(next) { }

	template<typename MapDC, typename MapCD>
	bool operator()(MapDC mDomCodom, MapCD mCodomDom) const {
		return next(MatchAsVertexMap<GraphDom, GraphCodom, MapDC, MapCD>(mDomCodom, mCodomDom), gDom, gCodom);
	}
private:
	const GraphDom &gDom;
	const GraphCodom &gCodom;
	Next next;
};

template<typename GraphDom, typename GraphCodom, typename Next>
MatchAsVertexMapWrapper<GraphDom, GraphCodom, Next> makeMatchAsVertexMapWrapper(const GraphDom &gDom, const GraphCodom &gCodom, Next &&next) {
	return MatchAsVertexMapWrapper<GraphDom, GraphCodom, Next>(gDom, gCodom, std::forward<Next>(next));
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif	/* JLA_BOOST_GRAPH_MORPHISM_MODELS_PROPERTY_MAP_H */