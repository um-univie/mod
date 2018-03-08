#ifndef JLA_BOOST_GRAPH_MORPHISM_CALLBACKS_STORE_HPP
#define JLA_BOOST_GRAPH_MORPHISM_CALLBACKS_STORE_HPP

#include <jla_boost/graph/morphism/VertexMap.hpp>

namespace jla_boost {
namespace GraphMorphism {

template<typename OutputIterator>
struct Store {

	Store(OutputIterator iter) : iter(iter) { }

	template<typename VertexMap>
	bool operator()(VertexMap &&m,
			const typename VertexMapTraits<VertexMap>::GraphDom &gDom,
			const typename VertexMapTraits<VertexMap>::GraphCodom &gCodom) const {
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

#endif /* JLA_BOOST_GRAPH_MORPHISM_CALLBACKS_STORE_HPP */