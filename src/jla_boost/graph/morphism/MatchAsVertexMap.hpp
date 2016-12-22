#ifndef JLA_BOOST_GRAPH_MORPHISM_MATCHASMORPHISM_H
#define	JLA_BOOST_GRAPH_MORPHISM_MATCHASMORPHISM_H

// This is just some convenience that wraps a map and it's inverse,
// and exposes them as an invertible vertex map.

#include <jla_boost/graph/morphism/VertexMap.hpp>

namespace jla_boost {
namespace GraphMorphism {

template<typename GraphLeftT, typename GraphRightT, typename Map, typename MapInv>
struct MatchAsVertexMap {
	using GraphLeft = GraphLeftT;
	using GraphRight = GraphRightT;
	using Storable = std::false_type;

	template<typename GraphLeftU, typename GraphRightU>
	struct Reinterpret {
		using type = MatchAsVertexMap<GraphLeftU, GraphRightU, Map, MapInv>;
	};

	template<typename GraphLeftU, typename GraphRightU>
	static MatchAsVertexMap<GraphLeftU, GraphRightU, Map, MapInv>
	reinterpret(MatchAsVertexMap<GraphLeft, GraphRight, Map, MapInv> &&m, const GraphLeft&, const GraphRight&, const GraphLeftU&, const GraphRightU&) {
		return MatchAsVertexMap<GraphLeftU, GraphRightU, Map, MapInv>(m.map, m.mapInv);
	}

	MatchAsVertexMap(Map map, MapInv mapInv) : map(std::move(map)), mapInv(std::move(mapInv)) { }
private:
	Map map;
	MapInv mapInv;
public:

	friend typename boost::graph_traits<GraphRight>::vertex_descriptor
	get(const MatchAsVertexMap<GraphLeft, GraphRight, Map, MapInv> &m, const GraphLeft &gLeft, const GraphRight &gRight,
			typename boost::graph_traits<GraphLeft>::vertex_descriptor v) {
		return get(m.map, v);
	}

	friend typename boost::graph_traits<GraphLeft>::vertex_descriptor
	get_inverse(const MatchAsVertexMap<GraphLeft, GraphRight, Map, MapInv> &m, const GraphLeft &gLeft, const GraphRight &gRight,
			typename boost::graph_traits<GraphRight>::vertex_descriptor v) {
		return get(m.mapInv, v);
	}
};

template<typename GraphLeft, typename GraphRight, typename Next>
struct MatchAsVertexMapWrapper {

	MatchAsVertexMapWrapper(const GraphLeft &gLeft, const GraphRight &gRight, Next next) : gLeft(gLeft), gRight(gRight), next(next) { }

	template<typename MapLR, typename MapRL>
	bool operator()(MapLR mLeftRight, MapRL mRightLeft) const {
		return next(MatchAsVertexMap<GraphLeft, GraphRight, MapLR, MapRL>(mLeftRight, mRightLeft), gLeft, gRight);
	}
private:
	const GraphLeft &gLeft;
	const GraphRight &gRight;
	Next next;
};

template<typename GraphLeft, typename GraphRight, typename Next>
MatchAsVertexMapWrapper<GraphLeft, GraphRight, Next> makeMatchAsVertexMapWrapper(const GraphLeft &gLeft, const GraphRight &gRight, Next &&next) {
	return MatchAsVertexMapWrapper<GraphLeft, GraphRight, Next>(gLeft, gRight, std::forward<Next>(next));
}

} // namespace GraphMorphism
} // namespace jla_boost


#endif	/* JLA_BOOST_GRAPH_MORPHISM_MATCHASMORPHISM_H */