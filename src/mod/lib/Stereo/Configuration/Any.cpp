#include "Any.h"

#include <mod/Error.h>

namespace mod {
namespace lib {
namespace Stereo {

Any::Any(const GeometryGraph &g, const EmbeddingEdge *b, const EmbeddingEdge *e)
: DynamicDegree(g.any, b, e) { }

std::unique_ptr<Configuration> Any::cloneFree(const GeometryGraph &g) const {
	return std::make_unique<Any>(g, begin(), end());
}

std::unique_ptr<Configuration> Any::clone(const GeometryGraph &g, const std::vector<std::size_t> &offsetMap) const {
	auto c = std::make_unique<Any>(g, begin(), end());
	for(auto &emb : c->edges) {
		if(emb.type == EmbeddingEdge::Type::Edge) {
			emb.offset = offsetMap[emb.offset];
		}
	}
	return c;
}

bool Any::morphismIso(const Configuration &cCodom, std::vector<std::size_t> &perm) const {
	MOD_ABORT;
}

bool Any::morphismSpec(const Configuration &cCodom, std::vector<std::size_t> &perm) const {
	MOD_ABORT;
}

std::pair<std::string, bool> Any::asPrettyStringImpl(std::function<std::size_t(const EmbeddingEdge&)> getNeighbourId) const {
	return std::make_pair("\\modStereoShape{Any}", false);
}

} // namespace Stereo
} // namespace lib
} // namespace mod