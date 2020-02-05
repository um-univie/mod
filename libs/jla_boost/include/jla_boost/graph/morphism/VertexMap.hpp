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

// VertexMapConcept
// -----------------------------------------------------------------------------

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
		bool storable = Storable::value;
		(void) storable;
		const M &cVertexMap = vertexMap;
		VertexCodom vCodom = get(cVertexMap, gDom, gCodom, vDom);
		(void) vCodom;
		auto mReinterpret = Traits::template reinterpret<GraphDom, GraphCodom>(std::move(vertexMap), gDom, gCodom, gDom, gCodom);
		(void) mReinterpret;
		auto trans = [](auto &&m, auto &gDom, auto &gCodom) {
			return std::move(m);
		};
		auto mTransform = Traits::transform(trans, std::move(vertexMap), gDom, gCodom);
		(void) mTransform;
	}
private:
	M vertexMap;
	GraphDom gDom;
	GraphCodom gCodom;
	VertexDom vDom;
};

// InvertibleVertexMapConcept
// -----------------------------------------------------------------------------

template<typename M>
struct InvertibleVertexMapConcept : VertexMapConcept<M> {
	using Traits = VertexMapTraits<M>;
	using GraphDom = typename Traits::GraphDom;
	using GraphCodom = typename Traits::GraphCodom;
	using VertexDom = typename boost::graph_traits<GraphDom>::vertex_descriptor;
	using VertexCodom = typename boost::graph_traits<GraphCodom>::vertex_descriptor;

	BOOST_CONCEPT_USAGE(InvertibleVertexMapConcept) {
		const M &cVertexMap = vertexMap;
		VertexDom vDom = get_inverse(cVertexMap, gDom, gCodom, vCodom);
		(void) vDom;
	}
private:
	M vertexMap;
	GraphDom gDom;
	GraphCodom gCodom;
	VertexCodom vCodom;
};

// WritableVertexMapConcept
// -----------------------------------------------------------------------------

template<typename M>
struct WritableVertexMapConcept : VertexMapConcept<M> {
	using Traits = VertexMapTraits<M>;
	using GraphDom = typename Traits::GraphDom;
	using GraphCodom = typename Traits::GraphCodom;
	using VertexDom = typename boost::graph_traits<GraphDom>::vertex_descriptor;
	using VertexCodom = typename boost::graph_traits<GraphCodom>::vertex_descriptor;

	BOOST_CONCEPT_USAGE(WritableVertexMapConcept) {
		VertexDom vDom;
		VertexCodom vCodom;
		put(vertexMap, gDom, gCodom, vDom, vCodom);
	}
private:
	M vertexMap;
	GraphDom gDom;
	GraphCodom gCodom;
};

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_VERTEX_MAP_H */