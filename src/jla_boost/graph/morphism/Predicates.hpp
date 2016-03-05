#ifndef JLA_BOOST_GRAPH_MORPHISM_PREDICATES_H
#define	JLA_BOOST_GRAPH_MORPHISM_PREDICATES_H

#include <jla_boost/Functional.hpp>
#include <jla_boost/graph/morphism/VertexMap.hpp>

// - PropertyPredicate
// - Limit
// - Filter

namespace jla_boost {
namespace GraphMorphism {

// PropertyPredicate
//------------------------------------------------------------------------------

template<typename PropLeft, typename PropRight, typename Next>
struct PropertyPredicate {

	PropertyPredicate(PropLeft pLeft, PropRight pRight, Next next) : pLeft(pLeft), pRight(pRight), next(next) { }

	template<typename VertexOrEdge>
	bool operator()(VertexOrEdge veLeft, VertexOrEdge veRight) {
		return get(pLeft, veLeft) == get(pRight, veRight)
				&& next(veLeft, veRight);
	}
private:
	PropLeft pLeft;
	PropRight pRight;
	Next next;
};

template<typename PropLeft, typename PropRight, typename Next = AlwaysTrue>
PropertyPredicate<PropLeft, PropRight, Next> makePropertyPredicate(PropLeft &&pLeft, PropRight &&pRight, Next next = AlwaysTrue()) {
	return PropertyPredicate<PropLeft, PropRight, Next>(std::forward<PropLeft>(pLeft), std::forward<PropRight>(pRight), next);
}

// Limit
//------------------------------------------------------------------------------

template<typename Next>
struct Limit {

	Limit(std::size_t maxHits, Next next)
	: maxHits(maxHits), numHits(0), next(next) { }

	template<typename VertexMap, typename GraphLeft, typename GraphRight>
	bool operator()(VertexMap &&m, const GraphLeft &gLeft, const GraphRight &gRight) const {
		BOOST_CONCEPT_ASSERT((VertexMapConcept<VertexMap>));
		if(maxHits == 0) return false;
		numHits++;
		bool res = next(std::forward<VertexMap>(m), gLeft, gRight);
		if(!res) return res;
		else return numHits != maxHits;
	}

	std::size_t getNumHits() const {
		return numHits;
	}

	bool getLimitReached() const {
		return numHits == maxHits;
	}
private:
	std::size_t maxHits;
	mutable std::size_t numHits;
	Next next;
};

template<typename Next = AlwaysTrue>
Limit<Next> makeLimit(std::size_t maxHits, Next next = AlwaysTrue()) {
	return Limit<Next>(maxHits, next);
}

// Filter
//------------------------------------------------------------------------------

template<typename P, typename Next>
struct Filter {

	Filter(P p, Next next) : p(p), next(next) { }

	template<typename VertexMap, typename GraphLeft, typename GraphRight>
	bool operator()(VertexMap &&m, const GraphLeft &gLeft, const GraphRight &gRight) const {
		BOOST_CONCEPT_ASSERT((VertexMapConcept<VertexMap>));
		if(p(m, gLeft, gRight)) return next(std::move(m), gLeft, gRight);
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

#endif	/* JLA_BOOST_GRAPH_MORPHISM_PREDICATES_H */