#ifndef JLA_BOOST_GRAPHMORPHISM_AS_MODELS_INVERTED_HPP
#define JLA_BOOST_GRAPHMORPHISM_AS_MODELS_INVERTED_HPP

#include <jla_boost/graph/morphism/VertexMap.hpp>

#include <boost/concept_check.hpp>

namespace jla_boost {
namespace GraphMorphism {

template<typename VertexMap>
struct InvertedVertexMap {
	BOOST_CONCEPT_ASSERT((InvertibleVertexMapConcept<VertexMap>));
public: // VertexMap
	using GraphDom = typename VertexMapTraits<VertexMap>::GraphCodom;
	using GraphCodom = typename VertexMapTraits<VertexMap>::GraphDom;
	using Storable = typename VertexMapTraits<VertexMap>::Storable;
public:

	InvertedVertexMap(VertexMap m) : m(std::move(m)) { }

	friend typename boost::graph_traits<GraphCodom>::vertex_descriptor
	get(const InvertedVertexMap &m, const GraphDom &gDom, const GraphCodom &gCodom,
			typename boost::graph_traits<GraphDom>::vertex_descriptor v) {
		return get_inverse(m.m, gCodom, gDom, v);
	}

	friend typename boost::graph_traits<GraphDom>::vertex_descriptor
	get_inverse(const InvertedVertexMap &m, const GraphDom &gDom, const GraphCodom &gCodom,
			typename boost::graph_traits<GraphCodom>::vertex_descriptor v) {
		return get(m.m, gCodom, gDom, v);
	}
private:
	VertexMap m;
};

template<typename VertexMap>
InvertedVertexMap<VertexMap> makeInvertedVertexMap(VertexMap &&m) {
	return InvertedVertexMap<VertexMap>(std::forward<VertexMap>(m));
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPHMORPHISM_AS_MODELS_INVERTED_HPP */