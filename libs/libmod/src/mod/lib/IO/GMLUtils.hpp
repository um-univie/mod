#ifndef MOD_LIB_IO_GMLUTILS_H
#define MOD_LIB_IO_GMLUTILS_H

#include <mod/lib/IO/Stereo.hpp>

#include <boost/optional/optional.hpp>
#include <boost/variant/variant.hpp>

#include <vector>

#include <gml/converter_edsl.hpp>

#include <string>

namespace mod {
namespace lib {
namespace IO {
namespace GML {

struct Vertex {
	int id;
	boost::optional<std::string> label;
	boost::optional<std::string> stereo;
public:
	boost::optional<lib::IO::Stereo::Read::ParsedEmbedding> parsedEmbedding;
};

struct Edge {
	friend std::ostream &operator<<(std::ostream &s, const Edge &e);
public:
	int source, target;
	boost::optional<std::string> label;
	boost::optional<std::string> stereo;
};

struct Graph {
	std::vector<Vertex> vertices;
	std::vector<Edge> edges;
};

struct AdjacencyConstraint {
	int id;
	std::string op;
	unsigned int count;
	std::vector<std::string> nodeLabels, edgeLabels;
};

struct ShortestPathConstraint {
	int source, target;
	std::string op;
	int length;
};

using MatchConstraint = boost::variant<AdjacencyConstraint, ShortestPathConstraint>;

struct Rule {
	boost::optional<std::string> id;
	boost::optional<std::string> labelType;
	Graph left, context, right;
	std::vector<MatchConstraint> matchConstraints;
};

//------------------------------------------------------------------------------

static const auto makeVertexConverter = [](std::size_t lowerBound) {
	using namespace gml::converter::edsl;
	auto cStereo = string("stereo", &Vertex::stereo);
	return list<Vertex>("node", &Graph::vertices)
			(int_("id", &Vertex::id), 1, 1)
			(string("label", &Vertex::label), lowerBound, 1)
			(cStereo, 0, 1)
			;
};

static const auto makeEdgeConverter = [](std::size_t lowerBound) {
	using namespace gml::converter::edsl;
	auto cStereo = string("stereo", &Edge::stereo);
	return list<Edge>("edge", &Graph::edges)
			(int_("source", &Edge::source), 1, 1)
			(int_("target", &Edge::target), 1, 1)
			(string("label", &Edge::label), lowerBound, 1)
			(cStereo, 0, 1)
			;
};

} // namespace GML
} // namespace IO
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_IO_GMLUTILS_H */