#ifndef MOD_LIB_IO_GMLUTILS_H
#define MOD_LIB_IO_GMLUTILS_H

#include <boost/optional/optional.hpp>
#include <boost/variant/variant.hpp>

#include <vector>

namespace mod {
namespace lib {
namespace IO {
namespace GML {

struct Vertex {
	int id;
	boost::optional<std::string> label;
};

struct Edge {
	friend std::ostream &operator<<(std::ostream &s, const Edge &e);
public:
	int source, target;
	boost::optional<std::string> label;
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
	Graph left, context, right;
	std::vector<MatchConstraint> matchConstraints;
};

} // namespace GML
} // namespace IO
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_IO_GMLUTILS_H */