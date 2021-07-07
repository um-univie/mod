#ifndef MOD_LIB_IO_STEREO_HPP
#define MOD_LIB_IO_STEREO_HPP

#include <mod/lib/IO/Graph.hpp>
#include <mod/lib/Rules/Real.hpp>

#include <optional>
#include <string>
#include <variant>

namespace mod::lib::Rules {
struct PropStereoCore;
} // namespace mod::lib::Rules
namespace mod::lib::Stereo {
struct Configuration;
struct GeometryGraph;
} // namespace mod::lib::Stereo
namespace mod::lib::IO::Stereo::Read {
using ParsedEmbeddingEdge = std::variant<int, char>;

struct ParsedEmbedding {
	std::optional<std::string> geometry;
	std::optional<std::vector<ParsedEmbeddingEdge> > edges;
	std::optional<bool> fixation;
};

lib::IO::Result<ParsedEmbedding> parseEmbedding(const std::string &str);

} // namesapce mod::lib::IO::Stereo::Read
namespace mod::lib::IO::Stereo::Write {

std::string summary(const lib::Graph::Single &g, lib::Graph::Vertex v, const lib::Stereo::Configuration &conf,
                    const IO::Graph::Write::Options &options, int shownIdOffset, const std::string &nameSuffix);
std::string summary(const lib::Rules::Real &r, lib::Rules::Vertex v, lib::Rules::Membership m,
                    const IO::Graph::Write::Options &options);

// old/new delimiter

void summary(const lib::Stereo::GeometryGraph &g);

} // namesapce mod::lib::IO::Stereo::Write

#endif // MOD_LIB_IO_STEREO_HPP