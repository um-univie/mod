#ifndef JLA_BOOST_GRAPH_MORPHISM_VERTEX_MAP_H
#define JLA_BOOST_GRAPH_MORPHISM_VERTEX_MAP_H

#include <jla_boost/Functional.hpp>
#include <jla_boost/graph/PairToRangeAdaptor.hpp>
#include <jla_boost/graph/morphism/Traits.hpp>

#include <boost/concept_check.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/graph_traits.hpp>

// - VertexMapConcept
// - InvertibleVertexMapConcept
// - WritableVertexMapConcept

namespace jla_boost {
namespace GraphMorphism {

template<typename M>
struct VertexMapTraits;

// Uni-directional
// =============================================================================

template<typename M>
struct VertexMapConcept {
	using Traits = VertexMapTraits<M>;
	using GraphDom = typename Traits::GraphDom;
	using GraphCodom = typename Traits::GraphCodom;
	using Storable = typename Traits::Storable;

	BOOST_CONCEPT_ASSERT((boost::GraphConcept<GraphDom>));
	BOOST_CONCEPT_ASSERT((boost::GraphConcept<GraphCodom>));

	using VertexDom = typename boost::graph_traits<GraphDom>::vertex_descriptor;
	using VertexCodom = typename boost::graph_traits<GraphCodom>::vertex_descriptor;

	BOOST_CONCEPT_USAGE(VertexMapConcept) {
		[[maybe_unused]] constexpr bool storable = Storable::value;
		const M &cVertexMap = vertexMap;
		[[maybe_unused]] VertexCodom vCodom = get(cVertexMap, gDom, gCodom, vDom);
		[[maybe_unused]] const auto mReinterpret = Traits::template reinterpret<GraphDom, GraphCodom>(
				std::move(vertexMap), gDom, gCodom, gDom, gCodom);
		const auto trans = [](auto &&m, auto &gDom, auto &gCodom) { return std::move(m); };
		[[maybe_unused]] const auto mTransform = Traits::transform(trans, std::move(vertexMap), gDom, gCodom);
	}
private:
	M vertexMap;
	GraphDom gDom;
	GraphCodom gCodom;
	VertexDom vDom;
};

template<typename M>
struct GraphMapConcept : VertexMapConcept<M> {
	using Traits = VertexMapTraits<M>;
	using GraphDom = typename Traits::GraphDom;
	using GraphCodom = typename Traits::GraphCodom;

	using EdgeDom = typename boost::graph_traits<GraphDom>::edge_descriptor;
	using EdgeCodom = typename boost::graph_traits<GraphCodom>::edge_descriptor;

	BOOST_CONCEPT_USAGE(GraphMapConcept) {
		const M &cGraphMap = graphMap;
		[[maybe_unused]] const EdgeCodom eCodom = get(cGraphMap, gDom, gCodom, eDom);
	}
private:
	M graphMap;
	GraphDom gDom;
	GraphCodom gCodom;
	EdgeDom eDom;
};

// Bi-directional
// =============================================================================

template<typename M>
struct InvertibleVertexMapConcept : VertexMapConcept<M> {
	using Traits = VertexMapTraits<M>;
	using GraphDom = typename Traits::GraphDom;
	using GraphCodom = typename Traits::GraphCodom;
	using VertexDom = typename boost::graph_traits<GraphDom>::vertex_descriptor;
	using VertexCodom = typename boost::graph_traits<GraphCodom>::vertex_descriptor;

	BOOST_CONCEPT_USAGE(InvertibleVertexMapConcept) {
		const M &cVertexMap = vertexMap;
		[[maybe_unused]] const VertexDom vDom = get_inverse(cVertexMap, gDom, gCodom, vCodom);
	}
private:
	M vertexMap;
	GraphDom gDom;
	GraphCodom gCodom;
	VertexCodom vCodom;
};

template<typename M>
struct InvertibleGraphMapConcept : InvertibleVertexMapConcept<M> {
	using Traits = VertexMapTraits<M>;
	using GraphDom = typename Traits::GraphDom;
	using GraphCodom = typename Traits::GraphCodom;
	using EdgeDom = typename boost::graph_traits<GraphDom>::edge_descriptor;
	using EdgeCodom = typename boost::graph_traits<GraphCodom>::edge_descriptor;

	BOOST_CONCEPT_USAGE(InvertibleGraphMapConcept) {
		const M &cGraphMap = graphMap;
		[[maybe_unused]] const EdgeDom eDom = get_inverse(cGraphMap, gDom, gCodom, eCodom);
	}
private:
	M graphMap;
	GraphDom gDom;
	GraphCodom gCodom;
	EdgeCodom eCodom;
};

// Writeable
// =============================================================================

template<typename M>
struct WritableVertexMapConcept : VertexMapConcept<M> {
	using Traits = VertexMapTraits<M>;
	using GraphDom = typename Traits::GraphDom;
	using GraphCodom = typename Traits::GraphCodom;
	using VertexDom = typename boost::graph_traits<GraphDom>::vertex_descriptor;
	using VertexCodom = typename boost::graph_traits<GraphCodom>::vertex_descriptor;

	BOOST_CONCEPT_USAGE(WritableVertexMapConcept) {
		const VertexDom vDom;
		const VertexCodom vCodom;
		put(vertexMap, gDom, gCodom, vDom, vCodom);
		syncSize(gDom, gCodom);
	}
private:
	M vertexMap;
	GraphDom gDom;
	GraphCodom gCodom;
};

template<typename M>
struct WritableGraphMapConcept : WritableVertexMapConcept<M> {
	using Traits = VertexMapTraits<M>;
	using GraphDom = typename Traits::GraphDom;
	using GraphCodom = typename Traits::GraphCodom;
	using EdgeDom = typename boost::graph_traits<GraphDom>::edge_descriptor;
	using EdgeCodom = typename boost::graph_traits<GraphCodom>::edge_descriptor;

	BOOST_CONCEPT_USAGE(WritableGraphMapConcept) {
		const EdgeDom eDom;
		const EdgeCodom eCodom;
		put(graphMap, gDom, gCodom, eDom, eCodom);
	}
private:
	M graphMap;
	GraphDom gDom;
	GraphCodom gCodom;
};

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_VERTEX_MAP_H */