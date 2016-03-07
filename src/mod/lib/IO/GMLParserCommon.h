#ifndef MOD_LIB_IO_GMLPARSERCOMMON_H
#define	MOD_LIB_IO_GMLPARSERCOMMON_H

#include <mod/lib/IO/ParserCommon.h>

namespace mod {
namespace lib {
namespace IO {
namespace Parser {
namespace GML {

struct Vertex {
	int id;
	std::string label;
};

struct Edge {
	int source, target;
	std::string label;
};

struct Graph {
	std::vector<Vertex> vertices;
	std::vector<Edge> edges;
};

struct MatchConstraint {
	int id;
	std::string op;
	unsigned int count;
	std::vector<std::string> nodeLabels, edgeLabels;
};

struct Rule {
	boost::optional<std::string> id;
	Graph left, context, right;
	std::vector<MatchConstraint> matchConstraints;
};

template<typename Iter> // manually instantiated in the cpp file
struct Skipper : public qi::grammar<Iter> {
	Skipper();
private:
	qi::rule<Iter> start;
};

template<typename Iter, template<typename> class Skipper> // manually instantiated in the cpp file
struct VertexAndEdgeList : public qi::grammar<Iter, Skipper<Iter>, Graph()> {
	VertexAndEdgeList();
private:
	qi::rule<Iter, Skipper<Iter>, Graph() > start;
	qi::rule<Iter, Skipper<Iter>, void(Graph&) > vertexOrEdges;
	qi::rule<Iter, Skipper<Iter>, void(Graph&) > vertexOrEdge;
	qi::rule<Iter, Skipper<Iter>, Vertex() > vertex;
	qi::rule<Iter, Skipper<Iter>, Edge() > edge;
	qi::rule<Iter, Skipper<Iter>, void() > vis2d;
	EscapedString<'"', '"', Iter> escapedString;
};

inline void pushVertex(Graph &g, Vertex v) {
	g.vertices.push_back(v);
}

inline void pushEdge(Graph &g, Edge e) {
	g.edges.push_back(e);
}

} // namespace GML
} // namespace Parser
} // namespace IO
} // namespace lib
} // namespace mod


BOOST_FUSION_ADAPT_STRUCT(mod::lib::IO::Parser::GML::Vertex,
		(int, id)(std::string, label))
BOOST_FUSION_ADAPT_STRUCT(mod::lib::IO::Parser::GML::Edge,
		(int, source)(int, target)(std::string, label))
BOOST_FUSION_ADAPT_STRUCT(mod::lib::IO::Parser::GML::MatchConstraint,
		(int, id)
		(std::string, op)
		(unsigned int, count)
		(std::vector<std::string>, nodeLabels)
		(std::vector<std::string>, edgeLabels))
BOOST_FUSION_ADAPT_STRUCT(mod::lib::IO::Parser::GML::Rule,
		(boost::optional<std::string>, id)
		(mod::lib::IO::Parser::GML::Graph, left)
		(mod::lib::IO::Parser::GML::Graph, context)
		(mod::lib::IO::Parser::GML::Graph, right)
		(std::vector<mod::lib::IO::Parser::GML::MatchConstraint>, matchConstraints))

#endif	/* MOD_LIB_IO_GMLPARSERCOMMON_H */