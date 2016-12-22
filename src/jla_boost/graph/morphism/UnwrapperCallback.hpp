#ifndef JLA_BOOST_GRAPH_MORPHISM_UNWRAPPERCALLBACK_HPP
#define JLA_BOOST_GRAPH_MORPHISM_UNWRAPPERCALLBACK_HPP

// Forward a reinterpreted morphism where the domain and codomain graphs have
// been unwrapped if they match the given graph.

#include <jla_boost/graph/AdaptorTraits.hpp>
#include <jla_boost/graph/morphism/VertexMap.hpp>

namespace jla_boost {
namespace GraphMorphism {

template<typename Graph, typename Next>
struct UnwrapperBoth {

	UnwrapperBoth(Next next) : next(next) { }

	template<typename VertexMap>
	bool operator()(VertexMap &&m,
			const typename VertexMapTraits<VertexMap>::GraphLeft &gLeft,
			const typename VertexMapTraits<VertexMap>::GraphRight &gRight) const {
		BOOST_CONCEPT_ASSERT((VertexMapConcept<VertexMap>));
		using GraphLeft = typename VertexMapTraits<VertexMap>::GraphLeft;
		using GraphRight = typename VertexMapTraits<VertexMap>::GraphRight;
		using GraphLeftUnwrapped = typename GraphAdaptorTraits<GraphLeft>::type;
		using GraphRightUnwrapped = typename GraphAdaptorTraits<GraphRight>::type;
		const GraphLeftUnwrapped &gLeftUnwrapped = GraphAdaptorTraits<GraphLeft>::unwrap(gLeft);
		const GraphRightUnwrapped &gRightUnwrapped = GraphAdaptorTraits<GraphRight>::unwrap(gRight);
		return next(VertexMapTraits<VertexMap>::template reinterpret<GraphLeftUnwrapped, GraphRightUnwrapped>(
				std::forward<VertexMap>(m),
				gLeft, gRight, gLeftUnwrapped, gRightUnwrapped),
				gLeftUnwrapped, gRightUnwrapped);
	}
private:
	Next next;
};

template<typename Graph, typename Next>
UnwrapperBoth<Graph, Next> makeUnwrapperBoth(Next &&next) {
	return UnwrapperBoth<Graph, Next>(std::forward<Next>(next));
}

template<typename Graph, typename Next>
struct UnwrapperLeft {

	UnwrapperLeft(Next next) : next(next) { }

	template<typename VertexMap>
	bool operator()(VertexMap &&m,
			const typename VertexMapTraits<VertexMap>::GraphLeft &gLeft,
			const typename VertexMapTraits<VertexMap>::GraphRight &gRight) const {
		BOOST_CONCEPT_ASSERT((VertexMapConcept<VertexMap>));
		using GraphLeft = typename VertexMapTraits<VertexMap>::GraphLeft;
		using GraphRight = typename VertexMapTraits<VertexMap>::GraphRight;
		using GraphLeftUnwrapped = typename GraphAdaptorTraits<GraphLeft>::type;
		const GraphLeftUnwrapped &gLeftUnwrapped = GraphAdaptorTraits<GraphLeft>::unwrap(gLeft);
		return next(VertexMapTraits<VertexMap>::template reinterpret<GraphLeftUnwrapped, GraphRight>(
				std::forward<VertexMap>(m),
				gLeft, gRight, gLeftUnwrapped, gRight),
				gLeftUnwrapped, gRight);
	}
private:
	Next next;
};

template<typename Graph, typename Next>
UnwrapperLeft<Graph, Next> makeUnwrapperLeft(Next &&next) {
	return UnwrapperLeft<Graph, Next>(std::forward<Next>(next));
}

template<typename Graph, typename Next>
struct UnwrapperRight {

	UnwrapperRight(Next next) : next(next) { }

	template<typename VertexMap>
	bool operator()(VertexMap &&m,
			const typename VertexMapTraits<VertexMap>::GraphLeft &gLeft,
			const typename VertexMapTraits<VertexMap>::GraphRight &gRight) const {
		BOOST_CONCEPT_ASSERT((VertexMapConcept<VertexMap>));
		using GraphLeft = typename VertexMapTraits<VertexMap>::GraphLeft;
		using GraphRight = typename VertexMapTraits<VertexMap>::GraphRight;
		using GraphRightUnwrapped = typename GraphAdaptorTraits<GraphRight>::type;
		const GraphRightUnwrapped &gRightUnwrapped = GraphAdaptorTraits<GraphRight>::unwrap(gRight);
		return next(VertexMapTraits<VertexMap>::template reinterpret<GraphLeft, GraphRightUnwrapped>(
				std::forward<VertexMap>(m),
				gLeft, gRight, gLeft, gRightUnwrapped),
				gLeft, gRightUnwrapped);
	}
private:
	Next next;
};

template<typename Graph, typename Next>
UnwrapperRight<Graph, Next> makeUnwrapperRight(Next &&next) {
	return UnwrapperRight<Graph, Next>(std::forward<Next>(next));
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_UNWRAPPERCALLBACK_HPP */