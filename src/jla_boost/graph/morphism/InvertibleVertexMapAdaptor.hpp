#ifndef JLA_BOOST_GRAPH_MORPHISM_INVERTIBLE_VERTEX_MAP_ADAPTOR_H
#define JLA_BOOST_GRAPH_MORPHISM_INVERTIBLE_VERTEX_MAP_ADAPTOR_H

// This is just some convenience that wraps a map and its inverse,
// and exposes them as an invertible vertex map.

#include <jla_boost/graph/morphism/VertexMap.hpp>

namespace jla_boost {
namespace GraphMorphism {

template<typename VertexMap, typename VertexMapInverse>
struct InvertibleVertexMapAdaptor {
	using GraphLeft = typename VertexMapTraits<VertexMap>::GraphLeft;
	using GraphRight = typename VertexMapTraits<VertexMap>::GraphRight;
	using Storable = std::integral_constant<bool, VertexMapTraits<VertexMap>::Storable::value && VertexMapTraits<VertexMapInverse>::Storable::value>;
	static_assert(std::is_same<GraphLeft, typename VertexMapTraits<VertexMapInverse>::GraphRight>::value, "Incompatible vertex maps.");
	static_assert(std::is_same<GraphRight, typename VertexMapTraits<VertexMapInverse>::GraphLeft>::value, "Incompatible vertex maps.");

	InvertibleVertexMapAdaptor(VertexMap m, VertexMapInverse mInverse) : m(std::move(m)), mInverse(std::move(mInverse)) { }
private:
	VertexMap m;
	VertexMapInverse mInverse;
public:

	friend typename boost::graph_traits<GraphRight>::vertex_descriptor
	get(const InvertibleVertexMapAdaptor &m, const GraphLeft &gLeft, const GraphRight &gRight,
			typename boost::graph_traits<GraphLeft>::vertex_descriptor v) {
		return get(m.m, gLeft, gRight, v);
	}

	friend typename boost::graph_traits<GraphLeft>::vertex_descriptor
	get_inverse(const InvertibleVertexMapAdaptor &m, const GraphLeft &gLeft, const GraphRight &gRight,
			typename boost::graph_traits<GraphRight>::vertex_descriptor v) {
		return get(m.mInverse, gRight, gLeft, v);
	}
};

template<typename VertexMap, typename VertexMapInverse>
InvertibleVertexMapAdaptor<VertexMap, VertexMapInverse>
makeInvertibleVertexMapAdaptor(VertexMap &&m, VertexMapInverse &&mInverse) {
	return InvertibleVertexMapAdaptor<VertexMap, VertexMapInverse>(std::forward<VertexMap>(m), std::forward<VertexMapInverse>(mInverse));
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_INVERTIBLE_VERTEX_MAP_ADAPTOR_H */