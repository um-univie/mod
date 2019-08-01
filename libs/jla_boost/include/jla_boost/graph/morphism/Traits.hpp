#ifndef JLA_BOOST_GRAPH_MORPHISM_TRAITS_HPP
#define JLA_BOOST_GRAPH_MORPHISM_TRAITS_HPP

#include <jla_boost/graph/morphism/models/Reinterpreted.hpp>

#include <utility>

namespace jla_boost {
namespace GraphMorphism {
namespace detail {

template<typename...>
using Void = void;

template<typename M, typename GDom, typename GCodom, typename = void>
struct HasReinterpret : std::false_type {
};

template<typename M, typename GDom, typename GCodom>
struct HasReinterpret<M, GDom, GCodom, Void<decltype(&M::template reinterpret<GDom, GCodom>)> > : std::true_type {
};

template<typename Trans, typename M, typename GDom, typename GCodom, typename = void>
struct HasTransform : std::false_type {
};

template<typename Trans, typename M, typename GDom, typename GCodom>
struct HasTransform<Trans, M, GDom, GCodom, Void<decltype(M::transform(std::declval<Trans>(), std::declval<M>(), std::declval<GDom>(), std::declval<GCodom>()))> > : std::true_type {
};

} // namespace detail

template<typename M>
struct VertexMapTraits {
	using GraphDom = typename M::GraphDom;
	using GraphCodom = typename M::GraphCodom;
	using Storable = typename M::Storable;

	template<typename GraphDomNew, typename GraphCodomNew>
	static auto reinterpret(M m, const GraphDom &gDom, const GraphCodom &gCodom, const GraphDomNew &gDomNew, const GraphCodomNew &gCodomNew) {
		return reinterpretImpl(std::move(m), gDom, gCodom, gDomNew, gCodomNew, detail::HasReinterpret<M, GraphDomNew, GraphCodomNew>());
	}

	template<typename Trans, typename GraphDom, typename GraphCodom>
	static auto transform(Trans trans, M m, const GraphDom &gDom, const GraphCodom &gCodom) {
		return transformImpl(trans, std::move(m), gDom, gCodom, detail::HasTransform<Trans, M, GraphDom, GraphCodom>());
	}
private:

	template<typename GraphDomNew, typename GraphCodomNew>
	static auto reinterpretImpl(M m, const GraphDom &gDom, const GraphCodom &gCodom, const GraphDomNew &gDomNew, const GraphCodomNew &gCodomNew, std::true_type) {
		return M::reinterpret(std::move(m), gDom, gCodom, gDomNew, gCodomNew);
	}

	template<typename GraphDomNew, typename GraphCodomNew>
	static auto reinterpretImpl(M m, const GraphDom &gDom, const GraphCodom &gCodom, const GraphDomNew &gDomNew, const GraphCodomNew &gCodomNew, std::false_type) {
		using Reinterpreted = ReinterpretedVertexMap<M, GraphDomNew, GraphCodomNew>;
		return Reinterpreted(std::move(m), gDom, gCodom);
	}
private:

	template<typename Trans, typename GraphDom, typename GraphCodom>
	static auto transformImpl(Trans trans, M m, const GraphDom &gDom, const GraphCodom &gCodom, std::true_type) {
		return M::transform(trans, std::move(m), gDom, gCodom);
	}

	template<typename Trans, typename GraphDom, typename GraphCodom>
	static auto transformImpl(Trans trans, M m, const GraphDom &gDom, const GraphCodom &gCodom, std::false_type) {
		return trans(std::move(m), gDom, gCodom);
	}
};

template<typename M>
struct VertexMapTraits<const M> : VertexMapTraits<M> {
};

template<typename M>
struct VertexMapTraits<std::reference_wrapper<M> > : VertexMapTraits<M> {
	using Storable = std::false_type;
};

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_TRAITS_HPP */