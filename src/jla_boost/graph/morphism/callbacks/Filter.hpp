#ifndef JLA_BOOST_GRAPH_MORPHISM_CALLBACKS_FILTER_HPP
#define JLA_BOOST_GRAPH_MORPHISM_CALLBACKS_FILTER_HPP

#include <jla_boost/graph/morphism/VertexMap.hpp>

namespace jla_boost {
namespace GraphMorphism {

template<typename P, typename Next>
struct Filter {

	Filter(P p, Next next) : p(p), next(next) { }

	template<typename VertexMap>
	bool operator()(VertexMap &&m,
			const typename VertexMapTraits<VertexMap>::GraphDom &gDom,
			const typename VertexMapTraits<VertexMap>::GraphCodom &gCodom) const {
		if(p(m, gDom, gCodom)) return next(std::move(m), gDom, gCodom);
		else return true;
	}
private:
	P p;
	Next next;
};

template<typename P, typename Next>
Filter<P, Next> makeFilter(P p, Next next) {
	return Filter<P, Next>(p, next);
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_CALLBACKS_FILTER_HPP */