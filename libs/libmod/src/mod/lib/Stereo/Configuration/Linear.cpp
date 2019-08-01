#include "Linear.hpp"

#include <mod/Error.hpp>

namespace mod {
namespace lib {
namespace Stereo {

Linear::Linear(const GeometryGraph &g, const std::array<EmbeddingEdge, 2> &edges)
: StaticDegree<2>(g.linear, edges) { }

std::unique_ptr<Configuration> Linear::cloneFree(const GeometryGraph &g) const {
	return std::make_unique<Linear>(g, edges);
}

std::unique_ptr<Configuration> Linear::clone(const GeometryGraph &g, const std::vector<std::size_t> &offsetMap) const {
	auto c = std::make_unique<Linear>(g, edges);
	for(auto &emb : c->edges) {
		if(emb.type == EmbeddingEdge::Type::Edge) {
			emb.offset = offsetMap[emb.offset];
		}
	}
	return c;
}

bool Linear::morphismIso(const Configuration &cCodom, std::vector<std::size_t> &perm) const {
	MOD_ABORT;
}

bool Linear::morphismSpec(const Configuration &cCodom, std::vector<std::size_t> &perm) const {
	MOD_ABORT;
}

std::pair<std::string, bool> Linear::asPrettyStringImpl(std::function<std::size_t(const EmbeddingEdge&)> getNeighbourId) const {
	return std::make_pair("\\modStereoShape{Linear}", false);
}

} // namespace Stereo
} // namespace lib
} // namespace mod