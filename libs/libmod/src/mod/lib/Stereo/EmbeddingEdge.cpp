#include "EmbeddingEdge.hpp"

namespace mod::lib::Stereo {

EmbeddingEdge::EmbeddingEdge(std::size_t offset, Type type, EdgeCategory cat)
		: offset(offset), type(type), cat(cat) {
	if(type != Type::Edge) assert(cat == EdgeCategory::Single);
}

} // namespace mod::lib::Stereo