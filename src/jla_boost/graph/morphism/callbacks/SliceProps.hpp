#ifndef JLA_BOOST_GRAPH_MORPHISM_CALLBACKS_SLICE_PROPS_HPP
#define JLA_BOOST_GRAPH_MORPHISM_CALLBACKS_SLICE_PROPS_HPP

#include <jla_boost/graph/morphism/models/PropertyVertexMap.hpp>

namespace jla_boost {
namespace GraphMorphism {
namespace detail {

template<typename M>
struct SlicePropsImpl {

	static M slice(M &&m) {
		return std::move(m);
	}
};

template<typename M, typename ...Props>
struct SlicePropsImpl<PropertyVertexMap<M, Props...> > {

	static M slice(PropertyVertexMap<M, Props...> &&m) {
		return std::move(m.m);
	}
};

} // namespace detail

template<typename Next>
struct SliceProps {

	SliceProps(Next next) : next(next) { }

	template<typename VertexMap>
	bool operator()(VertexMap &&m,
			const typename VertexMapTraits<VertexMap>::GraphDom &gDom,
			const typename VertexMapTraits<VertexMap>::GraphCodom &gCodom) const {
		return next(detail::SlicePropsImpl<VertexMap>::slice(std::move(m)), gDom, gCodom);
	}
private:
	Next next;
};

template<typename Next>
SliceProps<Next> makeSliceProps(Next next) {
	return SliceProps<Next>(next);
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_CALLBACKS_SLICE_PROPS_HPP */