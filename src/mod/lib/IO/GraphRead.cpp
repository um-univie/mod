#include "Graph.h"

#include <mod/lib/Chem/Smiles.h>
#include <mod/lib/Graph/DFSEncoding.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/IO/GMLUtils.h>
#include <mod/lib/IO/IO.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <gml/parser.hpp>
#include <gml/converter.hpp>
#include <gml/converter_edsl.hpp>

#include <boost/graph/copy.hpp>

namespace mod {
namespace lib {
namespace IO {
namespace Graph {
namespace Read {

Data::Data() { }

Data::Data(std::unique_ptr<lib::Graph::GraphType> graph, std::unique_ptr<lib::Graph::PropString> label) : graph(std::move(graph)), label(std::move(label)) {
	assert(this->graph);
	assert(this->label);
}

Data::Data(Data &&other) : graph(std::move(other.graph)), label(std::move(other.label)),
externalToInternalIds(std::move(other.externalToInternalIds)) { }

Data::~Data() {
	if(graph) MOD_ABORT;
	if(label) MOD_ABORT;
}

void Data::clear() {
	graph.reset();
	label.reset();
}

namespace {

Data parseGML(std::istream &s, std::ostream &err) {
	GML::Graph graph;
	{
		gml::ast::List ast;
		bool res = gml::parser::parse(s, ast, err);
		if(!res) return Data();
		using namespace gml::converter::edsl;
		auto cVertex = list<GML::Vertex>("node", &GML::Graph::vertices)
				(int_("id", &GML::Vertex::id), 1, 1)
				(string("label", &GML::Vertex::label), 1, 1);
		auto cEdge = list<GML::Edge>("edge", &GML::Graph::edges)
				(int_("source", &GML::Edge::source), 1, 1)
				(int_("target", &GML::Edge::target), 1, 1)
				(string("label", &GML::Edge::label), 1, 1);
		auto cGraph = list<Parent>("graph") (cVertex) (cEdge);
		auto iterBegin = ast.list.begin();
		res = gml::converter::convert(iterBegin, ast.list.end(), cGraph, err, graph);
		if(!res) return Data();
	}

	std::sort(begin(graph.vertices), end(graph.vertices), [](const GML::Vertex &v1, const GML::Vertex & v2) -> bool {
		return v1.id < v2.id;
	});
	auto gPtr = std::make_unique<lib::Graph::GraphType>();
	auto pStringPtr = std::make_unique<lib::Graph::PropString>(*gPtr);
	auto &g = *gPtr;
	auto &pString = *pStringPtr;
	using Vertex = lib::Graph::Vertex;
	std::unordered_map<int, Vertex> vMap;
	for(const auto &vGML : graph.vertices) {
		Vertex v = add_vertex(g);
		assert(vGML.label);
		pString.addVertex(v, *vGML.label);
		if(vMap.find(vGML.id) != end(vMap)) {
			err << "Error in graph GML. Vertex id " << vGML.id << " used multiple times." << std::endl;
			return Data();
		}
		vMap[vGML.id] = v;
	}
	for(const auto &eGML : graph.edges) {
		if(vMap.find(eGML.source) == end(vMap)) {
			err << "Error in graph GML. Source " << eGML.source << " does not exist in '" << eGML << "'" << std::endl;
			return Data();
		}
		if(vMap.find(eGML.target) == end(vMap)) {
			err << "Error in graph GML. Target " << eGML.target << " does not exist in '" << eGML << "'" << std::endl;
			return Data();
		}
		Vertex src = vMap[eGML.source];
		Vertex tar = vMap[eGML.target];
		auto e = edge(src, tar, g);
		if(e.second) {
			err << "Error in graph GML. Duplicate edge with source " << eGML.source << " and target " << eGML.target << "." << std::endl;
			return Data();
		}
		e = add_edge(src, tar, g);
		assert(eGML.label);
		pString.addEdge(e.first, *eGML.label);
	}
	auto data = Data(std::move(gPtr), std::move(pStringPtr));
	for(auto &&vp : vMap) {
		data.externalToInternalIds[vp.first] = get(boost::vertex_index_t(), g, vp.second);
	}
	return data;
}

} // namespace

Data gml(std::istream &s, std::ostream &err) {
	return parseGML(s, err);
}

Data dfs(const std::string &dfs, std::ostream &err) {
	return lib::Graph::DFSEncoding::parse(dfs, err);
}

Data smiles(const std::string &smiles, std::ostream &err) {
	return lib::Chem::readSmiles(smiles, err);
}

} // namespace Read
} // namespace Graph
} // namespace IO
} // namespace lib
} // namespace mod
