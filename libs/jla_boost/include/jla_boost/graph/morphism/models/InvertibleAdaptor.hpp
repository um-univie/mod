#ifndef JLA_BOOST_GRAPH_MORPHISM_MODELS_INVERTIBLE_ADAPTOR_H
#define JLA_BOOST_GRAPH_MORPHISM_MODELS_INVERTIBLE_ADAPTOR_H

// This is just some convenience that wraps a map and its inverse,
// and exposes them as an invertible vertex map.

#include <jla_boost/graph/morphism/Concepts.hpp>

namespace jla_boost {
namespace GraphMorphism {

template<typename VertexMap, typename VertexMapInverse>
struct InvertibleVertexMapAdaptor {
	using GraphDom = typename VertexMapTraits<VertexMap>::GraphDom;
	using GraphCodom = typename VertexMapTraits<VertexMap>::GraphCodom;
	using Storable = std::integral_constant<bool, VertexMapTraits<VertexMap>::Storable::value && VertexMapTraits<VertexMapInverse>::Storable::value>;
	static_assert(std::is_same<GraphDom, typename VertexMapTraits<VertexMapInverse>::GraphCodom>::value, "Incompatible vertex maps.");
	static_assert(std::is_same<GraphCodom, typename VertexMapTraits<VertexMapInverse>::GraphDom>::value, "Incompatible vertex maps.");

	InvertibleVertexMapAdaptor(VertexMap m, VertexMapInverse mInverse) : m(std::move(m)), mInverse(std::move(mInverse)) { }
private:
	VertexMap m;
	VertexMapInverse mInverse;
public:
	friend typename boost::graph_traits<GraphCodom>::vertex_descriptor
	get(const InvertibleVertexMapAdaptor &m, const GraphDom &gDom, const GraphCodom &gCodom,
			typename boost::graph_traits<GraphDom>::vertex_descriptor v) {
		return get(m.m, gDom, gCodom, v);
	}

	friend typename boost::graph_traits<GraphDom>::vertex_descriptor
	get_inverse(const InvertibleVertexMapAdaptor &m, const GraphDom &gDom, const GraphCodom &gCodom,
			typename boost::graph_traits<GraphCodom>::vertex_descriptor v) {
		return get(m.mInverse, gCodom, gDom, v);
	}
};

template<typename VertexMap, typename VertexMapInverse>
InvertibleVertexMapAdaptor<VertexMap, VertexMapInverse>
makeInvertibleVertexMapAdaptor(VertexMap &&m, VertexMapInverse &&mInverse) {
	return InvertibleVertexMapAdaptor<VertexMap, VertexMapInverse>(std::forward<VertexMap>(m), std::forward<VertexMapInverse>(mInverse));
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_MODELS_INVERTIBLE_ADAPTOR_H */