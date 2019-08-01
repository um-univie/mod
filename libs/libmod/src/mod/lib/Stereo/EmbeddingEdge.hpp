#ifndef MOD_LIB_STEREO_EMBEDDINGEDGE_H
#define MOD_LIB_STEREO_EMBEDDINGEDGE_H

#include <mod/lib/Stereo/EdgeCategory.hpp>

#include <boost/graph/graph_traits.hpp>

namespace mod {
namespace lib {
namespace Stereo {

struct EmbeddingEdge {

	enum class Type {
		Edge, LonePair, Radical
	};
public:
	EmbeddingEdge(std::size_t offset, Type type, EdgeCategory cat);

	template<typename Graph>
	typename boost::graph_traits<Graph>::edge_descriptor
	getEdge(const typename boost::graph_traits<Graph>::vertex_descriptor &v, const Graph &g) const {
		assert(type == Type::Edge);
		assert(offset < out_degree(v, g));
		auto iter = out_edges(v, g).first;
		std::advance(iter, offset);
		return *iter;
	}
public:
	std::size_t offset;
	Type type;
	EdgeCategory cat;
};

} // namespace Stereo
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_STEREO_EMBEDDINGEDGE_H */