#ifndef MOD_LIB_STEREO_READ_HPP
#define MOD_LIB_STEREO_READ_HPP

#include <mod/lib/IO/Result.hpp>

#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace mod::lib::Stereo::Read {

using ParsedEmbeddingEdge = std::variant<int, char>;

struct ParsedEmbedding {
	std::optional<std::string> geometry;
	std::optional<std::vector<ParsedEmbeddingEdge>> edges;
	std::optional<bool> fixation;
};

IO::Result<ParsedEmbedding> parseEmbedding(const std::string &str);

} // namespace mod::lib::Stereo::Read

#endif // MOD_LIB_STEREO_READ_HPP