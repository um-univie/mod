#ifndef MOD_LIB_IO_STEREO_H
#define MOD_LIB_IO_STEREO_H

#include <mod/lib/IO/Graph.h>
#include <mod/lib/Rules/Real.h>

#include <boost/variant/variant.hpp>

#include <string>

namespace mod {
namespace lib {
namespace Rules {
struct PropStereoCore;
} // namespace Rules
namespace Stereo {
struct Configuration;
struct GeometryGraph;
} // namespace Stereo
namespace IO {
namespace Stereo {
namespace Read {
using ParsedEmbeddingEdge = boost::variant<int, char>;

struct ParsedEmbedding {
	boost::optional<std::string> geometry;
	boost::optional<std::vector<ParsedEmbeddingEdge> > edges;
	boost::optional<bool> fixation;
};

boost::optional<ParsedEmbedding> parseEmbedding(const std::string &str, std::ostream &err);

} // namesapce Read
namespace Write {

std::string summary(const lib::Graph::Single &g, lib::Graph::Vertex v, const lib::Stereo::Configuration &conf, const IO::Graph::Write::Options &options);
std::string summary(const lib::Rules::Real &r, lib::Rules::Vertex v, lib::Rules::Membership m, const IO::Graph::Write::Options &options);

// old/new delimiter

void summary(const lib::Stereo::GeometryGraph &g);

} // namesapce Write
} // namesapce Stereo
} // namesapce IO
} // namesapce lib
} // namesapce mod

#endif /* MOD_LIB_IO_STEREO_H */