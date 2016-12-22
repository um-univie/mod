#ifndef JLA_BOOST_GRAPH_MORPHISM_PREIMAGESIZEDVERTEXMAP_HPP
#define JLA_BOOST_GRAPH_MORPHISM_PREIMAGESIZEDVERTEXMAP_HPP

namespace jla_boost {
namespace GraphMorphism {

template<typename VertexMap>
struct PreimageSizedVertexMap {
	using GraphLeft = typename VertexMapTraits<VertexMap>::GraphLeft;
	using GraphRight = typename VertexMapTraits<VertexMap>::GraphRight;
	using Storable = typename VertexMapTraits<VertexMap>::Storable;
public:

	PreimageSizedVertexMap(VertexMap m, std::size_t preimageSize)
	: m(std::move(m)), preimageSize(preimageSize) { }

	friend typename boost::graph_traits<GraphRight>::vertex_descriptor
	get(const PreimageSizedVertexMap &m, const GraphLeft &gLeft, const GraphRight &gRight,
			typename boost::graph_traits<GraphLeft>::vertex_descriptor v) {
		return get(m.m, gLeft, gRight, v);
	}

	friend typename boost::graph_traits<GraphLeft>::vertex_descriptor
	get_inverse(const PreimageSizedVertexMap &m, const GraphLeft &gLeft, const GraphRight &gRight,
			typename boost::graph_traits<GraphRight>::vertex_descriptor v) {
		return get_inverse(m.m, gLeft, gRight, v);
	}
public:

	friend std::size_t get_preimage_size(const PreimageSizedVertexMap &m) {
		return m.preimageSize;
	}
private:
	VertexMap m;
	std::size_t preimageSize;
};

template<typename VertexMap>
PreimageSizedVertexMap<VertexMap> makePreimageSizedVertexMap(VertexMap &&m, std::size_t preimageSize) {
	return PreimageSizedVertexMap<VertexMap>(std::forward<VertexMap>(m), preimageSize);
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_PREIMAGESIZEDVERTEXMAP_HPP */