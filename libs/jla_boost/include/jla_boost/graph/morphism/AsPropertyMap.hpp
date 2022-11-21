#ifndef JLA_BOOST_GRAPHMORPHISM_AS_PROPERTY_MAP_HPP
#define JLA_BOOST_GRAPHMORPHISM_AS_PROPERTY_MAP_HPP

#include <jla_boost/graph/morphism/Concepts.hpp>

#include <boost/concept_check.hpp>

namespace jla_boost {
namespace GraphMorphism {

template<typename VertexMap, typename GraphDom, typename GraphCodom>
struct AsPropertyMap {
	BOOST_CONCEPT_ASSERT((VertexMapConcept<VertexMap>));
public:
	AsPropertyMap(VertexMap m, const GraphDom &gDom, const GraphCodom &gCodom)
	: m(std::move(m)), gDom(gDom), gCodom(gCodom) { }

	friend typename boost::graph_traits<GraphCodom>::vertex_descriptor
	get(const AsPropertyMap &map, typename boost::graph_traits<GraphDom>::vertex_descriptor v) {
		return get(map.m, map.gDom, map.gCodom, v);
	}
private:
	VertexMap m;
	const GraphDom &gDom;
	const GraphCodom &gCodom;
};

template<typename VertexMap, typename GraphDom, typename GraphCodom>
AsPropertyMap<VertexMap, GraphDom, GraphCodom> makeAsPropertyMap(VertexMap &&m, const GraphDom &gDom, const GraphCodom &gCodom) {
	return AsPropertyMap<VertexMap, GraphDom, GraphCodom>(std::forward<VertexMap>(m), gDom, gCodom);
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPHMORPHISM_AS_PROPERTY_MAP_HPP */