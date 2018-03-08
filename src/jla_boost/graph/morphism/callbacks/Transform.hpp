#ifndef JLA_BOOST_GRAPH_MORPHISM_CALLBACKS_TRANSFORM_HPP
#define JLA_BOOST_GRAPH_MORPHISM_CALLBACKS_TRANSFORM_HPP

#include <jla_boost/graph/morphism/VertexMap.hpp>

namespace jla_boost {
namespace GraphMorphism {

template<typename Trans, typename Next>
struct Transform {

	Transform(Trans t, Next next) : t(t), next(next) { }

	template<typename VertexMap>
	bool operator()(VertexMap &&m,
			const typename VertexMapTraits<VertexMap>::GraphDom &gDom,
			const typename VertexMapTraits<VertexMap>::GraphCodom &gCodom) const {
		return next(VertexMapTraits<VertexMap>::transform(t, std::forward<VertexMap>(m), gDom, gCodom), gDom, gCodom);
	}
private:
	Trans t;
	Next next;
};

template<typename Trans, typename Next>
Transform<Trans, Next> makeTransform(Trans p, Next next) {
	return Transform<Trans, Next>(p, next);
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_CALLBACKS_TRANSFORM_HPP */