#include "EmbeddingEdge.hpp"

namespace mod {
namespace lib {
namespace Stereo {

EmbeddingEdge::EmbeddingEdge(std::size_t offset, Type type, EdgeCategory cat)
: offset(offset), type(type), cat(cat) {
	if(type != Type::Edge) assert(cat == EdgeCategory::Single);
}

} // namespace Stereo
} // namespace lib
} // namespace mod