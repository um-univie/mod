#ifndef JLA_BOOST_GRAPH_CONCEPTS_HPP
#define JLA_BOOST_GRAPH_CONCEPTS_HPP

#include <boost/concept_check.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/graph_traits.hpp>

namespace jla_boost {

template<typename G>
struct VertexIndexedGraphConcept {
	BOOST_CONCEPT_ASSERT((boost::GraphConcept<G>));
	using Traits = boost::graph_traits<G>;
	using Vertex = typename Traits::vertex_descriptor;
	using SizeType = typename Traits::vertices_size_type;

	BOOST_CONCEPT_USAGE(VertexIndexedGraphConcept) {
		const G &gConst = g;
		auto idx = get(boost::vertex_index_t(), g);
		n = get(idx, v);
		n = get(boost::vertex_index_t(), g, v);
	}
private:
	G g;
	Vertex v;
	SizeType n;
};

} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_CONCEPTS_HPP */