#ifndef JLA_BOOST_GRAPH_MORPHISM_CALLBACKS_HPP
#define JLA_BOOST_GRAPH_MORPHISM_CALLBACKS_HPP

#include <jla_boost/Functional.hpp>
#include <jla_boost/graph/morphism/VertexMap.hpp>

// - Limit
// - Filter
// - Store

namespace jla_boost {
namespace GraphMorphism {

// Limit
//------------------------------------------------------------------------------

template<typename Next>
struct Limit {

	Limit(std::size_t maxHits, Next next)
	: maxHits(maxHits), numHits(0), next(next) { }

	template<typename VertexMap, typename GraphLeft, typename GraphRight, typename ...Args>
	bool operator()(VertexMap &&m, const GraphLeft &gLeft, const GraphRight &gRight) const {
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


// StoreVertexMap
//------------------------------------------------------------------------------

template<typename OutputIterator>
struct Store {

	Store(OutputIterator iter) : iter(iter) { }

	template<typename VertexMap>
	bool operator()(VertexMap &&m,
			const typename VertexMapTraits<VertexMap>::GraphLeft &gLeft,
			const typename VertexMapTraits<VertexMap>::GraphRight &gRight) const {
		static_assert(VertexMapTraits<VertexMap>::Storable::value, "Only storable vertex maps should be stored.");
		*iter++ = std::forward<VertexMap>(m);
		return true;
	}
private:
	mutable OutputIterator iter;
};

template<typename OutputIterator>
Store<OutputIterator> makeStore(const OutputIterator &iter) {
	return Store<OutputIterator>(iter);
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_CALLBACKS_HPP */