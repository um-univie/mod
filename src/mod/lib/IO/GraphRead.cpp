#include "Graph.h"

#include <mod/lib/Chem/Smiles.h>
#include <mod/lib/Graph/DFSEncoding.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/IO/GMLParserCommon.h>
#include <mod/lib/IO/IO.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>
#include <jla_boost/Memory.hpp>

#include <boost/graph/copy.hpp>

#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_eps.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_nonterminal.hpp>

#include <unordered_map>

using namespace mod::lib::IO::Parser;

namespace mod {
namespace lib {
namespace IO {
namespace Graph {
namespace Read {

// The GML parser is based on the graph GML parser in GGL

namespace {

template<typename Iter, template<typename> class Skipper>
struct Parser : public qi::grammar<Iter, Skipper<Iter>, GML::Graph()> {

	Parser(std::ostream &err) : Parser::base_type(start), errorHandler(err) {
		start = qi::eps > qi::lit("graph") > vertexAndEdgeList;
		start.name("graphStatement");
		qi::on_error<qi::fail>(start, errorHandler(qi::_1, qi::_2, qi::_3, qi::_4));
	}
private:
	qi::rule<Iter, Skipper<Iter>, GML::Graph() > start;
	GML::VertexAndEdgeList<Iter, Skipper> vertexAndEdgeList;
	phx::function<ErrorHandler<Iter> > errorHandler;
};

std::pair<std::unique_ptr<lib::Graph::GraphType>, std::unique_ptr<lib::Graph::PropStringType> >
parseGML(std::istream &s, std::ostream &err) {

	struct FlagsHolder {

		FlagsHolder(std::istream &s) : s(s), flags(s.flags()) { }

		~FlagsHolder() {
			s.flags(flags);
		}
	private:
		std::istream &s;
		std::ios::fmtflags flags;
	} flagsHolder(s);
	s.unsetf(std::ios::skipws);
	IteratorType iterStart = spirit::make_default_multi_pass(BaseIteratorType(s));
	IteratorType iterEnd;
	Parser<IteratorType, GML::Skipper> parser(err);
	GML::Graph graph;

	bool res = lib::IO::Parser::phrase_parse(err, iterStart, iterEnd, parser, GML::Skipper<IteratorType>(), graph);
	if(!res) return std::make_pair(nullptr, nullptr);

	std::sort(begin(graph.vertices), end(graph.vertices), [](const GML::Vertex &v1, const GML::Vertex & v2) -> bool {
		return v1.id < v2.id;
	});
	auto gPtr = make_unique<lib::Graph::GraphType>();
	auto pStringPtr = make_unique<lib::Graph::PropStringType>(*gPtr);
	auto &g = *gPtr;
	auto &pString = *pStringPtr;
	using Vertex = lib::Graph::Vertex;
	std::unordered_map<int, Vertex> vMap;
	for(const auto &vertex : graph.vertices) {
		Vertex v = add_vertex(g);
		pString.addVertex(v, vertex.label);
		if(vMap.find(vertex.id) != end(vMap)) {
			err << "Error in graph GML. Vertex id " << vertex.id << " used multiple times." << std::endl;
			return std::make_pair(nullptr, nullptr);
		}
		vMap[vertex.id] = v;
	}
	for(const auto &edge : graph.edges) {
		if(vMap.find(edge.source) == end(vMap)) {
			err << "Error in graph GML. Source " << edge.source << " does not exist in edge [ source "
					<< edge.source << " target " << edge.target << " label \"" << edge.label << "\" ]" << std::endl;
			return std::make_pair(nullptr, nullptr);
		}
		if(vMap.find(edge.target) == end(vMap)) {
			err << "Error in graph GML. Target " << edge.target << " does not exist in edge [ source "
					<< edge.source << " target " << edge.target << " label \"" << edge.label << "\" ]" << std::endl;
			return std::make_pair(nullptr, nullptr);
		}
		Vertex src = vMap[edge.source];
		Vertex tar = vMap[edge.target];
		auto e = boost::edge(src, tar, g);
		if(e.second) {
			err << "Error in graph GML. Duplicate edge [ source "
					<< edge.source << " target " << edge.target << " label \"" << edge.label << "\" ]" << std::endl;
			return std::make_pair(nullptr, nullptr);
		}
		e = add_edge(src, tar, g);
		pString.addEdge(e.first, edge.label);
	}
	return std::make_pair(std::move(gPtr), std::move(pStringPtr));
}

} // namespace

std::pair<std::unique_ptr<lib::Graph::GraphType>, std::unique_ptr<lib::Graph::PropStringType> > gml(std::istream &s, std::ostream &err) {
	return parseGML(s, err);
}

std::pair<std::unique_ptr<lib::Graph::GraphType>, std::unique_ptr<lib::Graph::PropStringType> > dfs(const std::string &dfs, std::ostream &err) {
	return lib::Graph::DFSEncoding::parse(dfs, err);
}

std::pair<std::unique_ptr<lib::Graph::GraphType>, std::unique_ptr<lib::Graph::PropStringType> > smiles(const std::string &smiles, std::ostream &err) {
	return lib::Chem::readSmiles(smiles, err);
}

} // namespace Read
} // namespace Graph
} // namespace IO
} // namespace lib
} // namespace mod
