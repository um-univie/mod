#ifndef JLA_BOOST_GRAPH_MORPHISM_CALLBACKS_LIMIT_HPP
#define JLA_BOOST_GRAPH_MORPHISM_CALLBACKS_LIMIT_HPP

#include <jla_boost/Functional.hpp>
#include <jla_boost/graph/morphism/Concepts.hpp>

namespace jla_boost {
namespace GraphMorphism {

template<typename Next>
struct Limit {

	Limit(std::size_t maxHits, Next next)
	: maxHits(maxHits), numHits(0), next(next) { }

	template<typename VertexMap>
	bool operator()(VertexMap &&m,
			const typename VertexMapTraits<VertexMap>::GraphDom &gDom,
			const typename VertexMapTraits<VertexMap>::GraphCodom &gCodom) const {
		if(maxHits == 0) return false;
		numHits++;
		bool res = next(std::forward<VertexMap>(m), gDom, gCodom);
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

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_CALLBACKS_LIMIT_HPP */