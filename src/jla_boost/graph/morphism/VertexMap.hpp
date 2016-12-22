#ifndef JLA_BOOST_GRAPH_MORPHISM_VERTEX_MAP_H
#define JLA_BOOST_GRAPH_MORPHISM_VERTEX_MAP_H

#include <jla_boost/Functional.hpp>
#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <boost/concept_check.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/graph_traits.hpp>

// - VertexMapConcept
// - InvertibleVertexMapConcept
// - WritableVertexMapConcept
// - ReinterpretedVertexMap
// - VertexMapTraits
// - AsPropertyMap
// - AsInvertedVertexMap

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
	// TODO: can we in a good way check Reinterpret with other graph types?
	using Reinterpreter = typename Traits::template Reinterpret<GraphLeft, GraphRight>;
	using ReinterpretType = typename Reinterpreter::type;

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

// ReinterpretedVertexMap
//------------------------------------------------------------------------------

template<typename VertexMap, typename GraphLeftT, typename GraphRightT>
struct ReinterpretedVertexMap {
	using GraphLeft = GraphLeftT;
	using GraphRight = GraphRightT;
	using Storable = typename VertexMapTraits<VertexMap>::Storable;
public:
	using GraphLeftOrig = typename VertexMapTraits<VertexMap>::GraphLeft;
	using GraphRightOrig = typename VertexMapTraits<VertexMap>::GraphRight;
public:

	ReinterpretedVertexMap(VertexMap m, const GraphLeftOrig &gLeft, const GraphRightOrig &gRight)
	: m(std::move(m)), gLeft(gLeft), gRight(gRight) { }

	friend typename boost::graph_traits<GraphRight>::vertex_descriptor
	get(const ReinterpretedVertexMap &m, const GraphLeft&, const GraphRight&,
			typename boost::graph_traits<GraphLeft>::vertex_descriptor vLeftU) {
		return get(m.m, m.gLeft, m.gRight, vLeftU);
	}

	friend typename boost::graph_traits<GraphLeft>::vertex_descriptor
	get_inverse(const ReinterpretedVertexMap &m, const GraphLeft&, const GraphRight&,
			typename boost::graph_traits<GraphRightOrig>::vertex_descriptor vRightU) {
		return get_inverse(m.m, m.gLeft, m.gRight, vRightU);
	}
private:
	VertexMap m;
	const GraphLeftOrig &gLeft;
	const GraphRightOrig &gRight;
};

// VertexMapTraits
//------------------------------------------------------------------------------

template<typename MMaybeRef>
struct VertexMapTraits {
	using M = typename std::remove_reference<MMaybeRef>::type;
	using GraphLeft = typename M::GraphLeft;
	using GraphRight = typename M::GraphRight;
	using Storable = typename M::Storable;

	template<typename GraphLeftNew, typename GraphRightNew>
	struct Reinterpret {
		using type = ReinterpretedVertexMap<M, GraphLeftNew, GraphRightNew>;
	};

	template<typename GraphLeftNew, typename GraphRightNew>
	static typename Reinterpret<GraphLeftNew, GraphRightNew>::type
	reinterpret(M m, const GraphLeft &gLeft, const GraphRight &gRight, const GraphLeftNew&, const GraphRightNew&) {
		return typename Reinterpret<GraphLeftNew, GraphRightNew>::type(std::move(m), gLeft, gRight);
	}
};

template<typename M>
struct VertexMapTraits<std::reference_wrapper<M> > : VertexMapTraits<M> {
	using Storable = std::false_type;
};


// AsPropertyMap
//------------------------------------------------------------------------------

template<typename VertexMap, typename GraphLeft, typename GraphRight>
struct AsPropertyMap {
	BOOST_CONCEPT_ASSERT((VertexMapConcept<VertexMap>));
public:

	AsPropertyMap(VertexMap m, const GraphLeft &gLeft, const GraphRight &gRight) : m(std::move(m)), gLeft(gLeft), gRight(gRight) { }

	friend typename boost::graph_traits<GraphRight>::vertex_descriptor
	get(const AsPropertyMap &m, typename boost::graph_traits<GraphLeft>::vertex_descriptor v) {
		return get(m.m, m.gLeft, m.gRight, v);
	}
private:
	VertexMap m;
	const GraphLeft &gLeft;
	const GraphRight &gRight;
};

template<typename VertexMap, typename GraphLeft, typename GraphRight>
AsPropertyMap<VertexMap, GraphLeft, GraphRight> makeAsPropertyMap(VertexMap &&m, const GraphLeft &gLeft, const GraphRight &gRight) {
	return AsPropertyMap<VertexMap, GraphLeft, GraphRight>(std::forward<VertexMap>(m), gLeft, gRight);
}


// InvertedVertexMap
//------------------------------------------------------------------------------

template<typename VertexMap>
struct InvertedVertexMap {
	BOOST_CONCEPT_ASSERT((InvertibleVertexMapConcept<VertexMap>));
public: // VertexMap
	using GraphLeft = typename VertexMapTraits<VertexMap>::GraphRight;
	using GraphRight = typename VertexMapTraits<VertexMap>::GraphLeft;
	using Storable = typename VertexMapTraits<VertexMap>::Storable;
public:

	InvertedVertexMap(VertexMap m) : m(std::move(m)) { }

	friend typename boost::graph_traits<GraphRight>::vertex_descriptor
	get(const InvertedVertexMap &m, const GraphLeft &gLeft, const GraphRight &gRight,
			typename boost::graph_traits<GraphLeft>::vertex_descriptor v) {
		return get_inverse(m.m, gRight, gLeft, v);
	}

	friend typename boost::graph_traits<GraphLeft>::vertex_descriptor
	get_inverse(const InvertedVertexMap &m, const GraphLeft &gLeft, const GraphRight &gRight,
			typename boost::graph_traits<GraphRight>::vertex_descriptor v) {
		return get(m.m, gRight, gLeft, v);
	}
private:
	VertexMap m;
};

template<typename VertexMap>
InvertedVertexMap<VertexMap> makeInvertedVertexMap(VertexMap &&m) {
	return InvertedVertexMap<VertexMap>(std::forward<VertexMap>(m));
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_VERTEX_MAP_H */