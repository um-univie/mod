#ifndef JLA_BOOST_GRAPH_MORPHISM_VERTEX_MAP_H
#define JLA_BOOST_GRAPH_MORPHISM_VERTEX_MAP_H

#include <jla_boost/Functional.hpp>
#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <boost/concept_check.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/graph_traits.hpp>

// - VertexMapConcept
// - ReinterpretableVertexMapConcept
// - InvertibleVertexMapConcept
// - WriteableVertexMapConcept
// - VertexMapTraits
// - StoreVertexMap
// - Reinterpreter

namespace jla_boost {
namespace GraphMorphism {

template<typename M>
struct VertexMapTraits;

template<typename M>
struct VertexMapConcept {
	using Traits = VertexMapTraits<M>;
	using GraphLeft = typename Traits::GraphLeft;
	using GraphRight = typename Traits::GraphRight;
	using Storable = typename Traits::Storable;

	BOOST_CONCEPT_ASSERT((boost::GraphConcept<GraphLeft>));
	BOOST_CONCEPT_ASSERT((boost::GraphConcept<GraphRight>));

	using VertexLeft = typename boost::graph_traits<GraphLeft>::vertex_descriptor;
	using VertexRight = typename boost::graph_traits<GraphRight>::vertex_descriptor;

	BOOST_CONCEPT_USAGE(VertexMapConcept) {
		bool storable = Storable::value;
		(void) storable;
		const M &cVertexMap = vertexMap;
		VertexLeft vLeft;
		VertexRight vRight = get(cVertexMap, gLeft, gRight, vLeft);
		(void) vRight;
	}
private:
	M vertexMap;
	GraphLeft gLeft;
	GraphRight gRight;
};

template<typename M>
struct ReinterpretableVertexMapConcept : VertexMapConcept<M> {
	using Traits = VertexMapTraits<M>;
	using GraphLeft = typename Traits::GraphLeft;
	using GraphRight = typename Traits::GraphRight;
	// TODO: can we in a good way check Reinterpret with other graph types?
	using Reinterpreter = typename Traits::template Reinterpret<GraphLeft, GraphRight>;
	using ReinterpretType = typename Reinterpreter::type;

	BOOST_CONCEPT_USAGE(ReinterpretableVertexMapConcept) {
		ReinterpretType mReinterpret = Traits::template reinterpret<GraphLeft, GraphRight>(std::move(vertexMap), gLeft, gRight, gLeft, gRight);
		(void) mReinterpret;
	}
private:
	M vertexMap;
	GraphLeft gLeft;
	GraphRight gRight;
};

template<typename M>
struct InvertibleVertexMapConcept : VertexMapConcept<M> {
	using Traits = VertexMapTraits<M>;
	using GraphLeft = typename Traits::GraphLeft;
	using GraphRight = typename Traits::GraphRight;
	using VertexLeft = typename boost::graph_traits<GraphLeft>::vertex_descriptor;
	using VertexRight = typename boost::graph_traits<GraphRight>::vertex_descriptor;

	BOOST_CONCEPT_USAGE(InvertibleVertexMapConcept) {
		const M &cVertexMap = vertexMap;
		VertexRight vRight;
		VertexLeft vLeft = get_inverse(cVertexMap, gLeft, gRight, vRight);
		(void) vLeft;
	}
private:
	M vertexMap;
	GraphLeft gLeft;
	GraphRight gRight;
};

template<typename M>
struct WritableVertexMapConcept : VertexMapConcept<M> {
	using Traits = VertexMapTraits<M>;
	using GraphLeft = typename Traits::GraphLeft;
	using GraphRight = typename Traits::GraphRight;
	using VertexLeft = typename boost::graph_traits<GraphLeft>::vertex_descriptor;
	using VertexRight = typename boost::graph_traits<GraphRight>::vertex_descriptor;

	BOOST_CONCEPT_USAGE(WritableVertexMapConcept) {
		VertexLeft vLeft;
		VertexRight vRight;
		put(vertexMap, gLeft, gRight, vLeft, vRight);
	}
private:
	M vertexMap;
	GraphLeft gLeft;
	GraphRight gRight;
};

template<typename M>
struct VertexMapTraits {
	using GraphLeft = typename M::GraphLeft;
	using GraphRight = typename M::GraphRight;
	using Storable = typename M::Storable;

	template<typename GraphLeftU, typename GraphRightU>
	struct Reinterpret {
		using type = typename M::template Reinterpret<GraphLeftU, GraphRightU>::type;
	};

	template<typename GraphLeftU, typename GraphRightU>
	static typename Reinterpret<GraphLeftU, GraphRightU>::type
	reinterpret(M &&m, const GraphLeft &gLeft, const GraphRight &gRight, const GraphLeftU &gLeftReinterpreted, const GraphRightU &gRightReinterpreted) {
		return M::template reinterpret<GraphLeftU, GraphRightU>(std::move(m), gLeft, gRight, gLeftReinterpreted, gRightReinterpreted);
	}
};

// StoreVertexMap
//------------------------------------------------------------------------------

template<typename OutputIterator>
struct StoreVertexMap {

	StoreVertexMap(OutputIterator iter) : iter(iter) { }

	template<typename VertexMap>
	bool operator()(VertexMap &&m,
			const typename VertexMapTraits<VertexMap>::GraphLeft &gLeft,
			const typename VertexMapTraits<VertexMap>::GraphRight &gRight) const {
		BOOST_CONCEPT_ASSERT((VertexMapConcept<VertexMap>));
		static_assert(VertexMapTraits<VertexMap>::Storable::value, "Only storable vertex maps should be stored.");
		*iter++ = std::forward<VertexMap>(m);
		return true;
	}
private:
	mutable OutputIterator iter;
};

template<typename OutputIterator>
StoreVertexMap<OutputIterator> makeStoreVertexMap(OutputIterator iter) {
	return StoreVertexMap<OutputIterator>(iter);
}

// Reinterpreter
//------------------------------------------------------------------------------

template<typename Graph>
struct ReinterpreterTraits {
	//	using type = ...;

	//	static const type &unwrap(const Graph &g) {
	//		return ...;
	//	}
};

namespace detail {

template<typename GraphInput, typename GraphDesired>
struct ReinterpreterHelper {
	using type = GraphInput;

	static const type &unwrap(const GraphInput &g) {
		return g;
	}
};

template<typename Graph>
struct ReinterpreterHelper<Graph, Graph> {
	using type = typename ReinterpreterTraits<Graph>::type;

	static const type &unwrap(const Graph &g) {
		return ReinterpreterTraits<Graph>::unwrap(g);
	}
};

} // namespace detail

template<typename Graph, typename Next>
struct Reinterpreter {

	Reinterpreter(Next next) : next(next) { }

	template<typename VertexMap>
	bool operator()(VertexMap &&m,
			const typename VertexMapTraits<VertexMap>::GraphLeft &gLeft,
			const typename VertexMapTraits<VertexMap>::GraphRight &gRight) const {
		BOOST_CONCEPT_ASSERT((ReinterpretableVertexMapConcept<VertexMap>));
		using GraphLeft = typename VertexMapTraits<VertexMap>::GraphLeft;
		using GraphRight = typename VertexMapTraits<VertexMap>::GraphRight;
		using GraphLeftUnwrapped = typename detail::ReinterpreterHelper<GraphLeft, Graph>::type;
		using GraphRightUnwrapped = typename detail::ReinterpreterHelper<GraphRight, Graph>::type;
		const GraphLeftUnwrapped &gLeftUnwrapped = detail::ReinterpreterHelper<GraphLeft, Graph>::unwrap(gLeft);
		const GraphRightUnwrapped &gRightUnwrapped = detail::ReinterpreterHelper<GraphRight, Graph>::unwrap(gRight);
		return next(VertexMapTraits<VertexMap>::template reinterpret<GraphLeftUnwrapped, GraphRightUnwrapped>(std::forward<VertexMap>(m),
				gLeft, gRight, gLeftUnwrapped, gRightUnwrapped),
				gLeftUnwrapped, gRightUnwrapped);
	}
private:
	Next next;
};

template<typename Graph, typename Next>
Reinterpreter<Graph, Next> makeReinterpreter(Next &&next) {
	return Reinterpreter<Graph, Next>(std::forward<Next>(next));
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_VERTEX_MAP_H */