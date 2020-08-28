#include "Graph.hpp"

#include <mod/lib/Chem/Smiles.hpp>
#include <mod/lib/Graph/DFSEncoding.hpp>
#include <mod/lib/Graph/Properties/String.hpp>
#include <mod/lib/Graph/Properties/Molecule.hpp>
#include <mod/lib/Graph/Properties/Stereo.hpp>
#include <mod/lib/IO/GMLUtils.hpp>
#include <mod/lib/IO/IO.hpp>
#include <mod/lib/Stereo/GeometryGraph.hpp>
#include <mod/lib/Stereo/Inference.hpp>

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

Data::Data() {}

Data::Data(Data &&other) : g(std::move(other.g)), pString(std::move(other.pString)), pStereo(std::move(other.pStereo)),
                           externalToInternalIds(std::move(other.externalToInternalIds)) {}

Data::~Data() {
	if(std::uncaught_exception()) return; // TODO: update to the plural version when C++17 is required
	if(g) MOD_ABORT;
	if(pString) MOD_ABORT;
	if(pStereo) MOD_ABORT;
}

namespace {

Data parseGML(std::istream &s, std::ostream &err) {
	GML::Graph gGML;
	{
		gml::ast::KeyValue ast;
		bool res = gml::parser::parse(s, ast, err);
		if(!res) return Data();
		using namespace gml::converter::edsl;
		auto cVertex = GML::makeVertexConverter(1);
		auto cEdge = GML::makeEdgeConverter(1);
		auto cGraph = list<Parent>("graph")(cVertex)(cEdge);
		auto iterBegin = &ast;
		auto iterEnd = iterBegin + 1;
		res = gml::converter::convert(iterBegin, iterEnd, cGraph, err, gGML);
		if(!res) return Data();
	}

	std::sort(begin(gGML.vertices), end(gGML.vertices), [](const GML::Vertex &v1, const GML::Vertex &v2) -> bool {
		return v1.id < v2.id;
	});
	auto gPtr = std::make_unique<lib::Graph::GraphType>();
	auto pStringPtr = std::make_unique<lib::Graph::PropString>(*gPtr);
	auto &g = *gPtr;
	auto &pString = *pStringPtr;
	using Vertex = lib::Graph::Vertex;
	using Edge = lib::Graph::Edge;
	std::unordered_map<int, Vertex> vMap;
	std::unordered_map<Vertex, int> idMap;
	for(const auto &vGML : gGML.vertices) {
		Vertex v = add_vertex(g);
		assert(vGML.label);
		pString.addVertex(v, *vGML.label);
		if(vMap.find(vGML.id) != end(vMap)) {
			err << "Error in graph GML. Vertex id " << vGML.id << " used multiple times." << std::endl;
			return Data();
		}
		vMap[vGML.id] = v;
		idMap[v] = vGML.id;
	}
	auto vFromVertexId = [&vMap](int id) {
		auto iter = vMap.find(id);
		assert(iter != end(vMap));
		return iter->second;
	};
	for(const auto &eGML : gGML.edges) {
		if(eGML.source == eGML.target) {
			err << "Error in graph GML. Loop edge (on " << eGML.source << ") is not allowed." << std::endl;
			return Data();
		}
		if(vMap.find(eGML.source) == end(vMap)) {
			err << "Error in graph GML. Source " << eGML.source << " does not exist in '" << eGML << "'" << std::endl;
			return Data();
		}
		if(vMap.find(eGML.target) == end(vMap)) {
			err << "Error in graph GML. Target " << eGML.target << " does not exist in '" << eGML << "'" << std::endl;
			return Data();
		}
		Vertex src = vFromVertexId(eGML.source);
		Vertex tar = vFromVertexId(eGML.target);
		auto e = edge(src, tar, g);
		if(e.second) {
			err << "Error in graph GML. Duplicate edge with source " << eGML.source << " and target " << eGML.target << "."
			    << std::endl;
			return Data();
		}
		e = add_edge(src, tar, g);
		assert(eGML.label);
		pString.addEdge(e.first, *eGML.label);
	}

	// Prepare data for the standard core part.
	Data data;
	data.g = std::move(gPtr);
	data.pString = std::move(pStringPtr);
	for(auto &&vp : vMap) {
		data.externalToInternalIds[vp.first] = get(boost::vertex_index_t(), g, vp.second);
	}

	auto atError = [&data]() {
		data.g.reset();
		data.pString.reset();
		data.pStereo.reset();
		return Data();
	};

	bool doStereo = false;
	for(const auto &vGML : gGML.vertices) doStereo = doStereo || vGML.stereo;
	for(const auto &eGML : gGML.edges) doStereo = doStereo || eGML.stereo;
	if(!doStereo) return data;
	// Stereo
	//============================================================================
	lib::Graph::PropMolecule molState(g, pString);
	auto stereoInference = lib::Stereo::makeInference(g, molState, false);
	std::stringstream ssErr;
	const auto &gGeometry = lib::Stereo::getGeometryGraph();
	// Set the explicitly defined edge categories.
	//----------------------------------------------------------------------------
	for(const auto &eGML : gGML.edges) {
		Vertex vSrc = vFromVertexId(eGML.source),
				vTar = vFromVertexId(eGML.target);
		auto ePair = edge(vSrc, vTar, g);
		assert(ePair.second);
		if(!eGML.stereo) continue;
		const std::string &s = *eGML.stereo;
		if(s.size() != 1) {
			err << "Error in stereo data for edge (" << eGML.source << ", " << eGML.target << "). ";
			err << "Parsing error in stereo data '" << s << "'.";
			return atError();
		}
		lib::Stereo::EdgeCategory cat;
		switch(s.front()) {
		case '*':
			cat = lib::Stereo::EdgeCategory::Any;
			break;
		default:
			err << "Error in stereo data for edge (" << eGML.source << ", " << eGML.target << "). ";
			err << "Parsing error in stereo data '" << s << "'.";
			return atError();
		}
		bool res = stereoInference.assignEdgeCategory(ePair.first, cat, ssErr);
		if(!res) {
			err << "Error in stereo data for edge (" << eGML.source << ", " << eGML.target << "). ";
			err << ssErr.str();
			return atError();
		}
	}
	// Set the explicitly stereo data.
	//----------------------------------------------------------------------------
	for(auto &vGML : gGML.vertices) {
		if(!vGML.stereo) continue;
		auto v = vFromVertexId(vGML.id);
		vGML.parsedEmbedding = lib::IO::Stereo::Read::parseEmbedding(vGML.stereo.get(), ssErr);
		if(!vGML.parsedEmbedding) {
			err << "Error in stereo data for vertex " << vGML.id << ". ";
			err << ssErr.str();
			return atError();
		}
		// Geometry
		//..........................................................................
		const auto &embGML = *vGML.parsedEmbedding;
		if(embGML.geometry) {
			auto vGeo = gGeometry.findGeometry(*embGML.geometry);
			if(vGeo == gGeometry.nullGeometry()) {
				err << "Error in stereo data for vertex " << vGML.id << ". Invalid gGeometry '" << *embGML.geometry << "'."
				    << std::endl;
				return atError();
			}
			bool res = stereoInference.assignGeometry(v, vGeo, ssErr);
			if(!res) {
				err << "Error in stereo data for vertex " << vGML.id << ". " << ssErr.str();
				return atError();
			}
		}
		// Edges
		//..........................................................................
		if(embGML.edges) {
			stereoInference.initEmbedding(v);
			for(const auto &e : *embGML.edges) {
				if(const int *idPtr = boost::get<int>(&e)) {
					int idNeighbour = *idPtr;
					if(vMap.find(idNeighbour) == end(vMap)) {
						err << "Error in graph GML. Neighbour vertex " << idNeighbour << " in stereo embedding for vertex "
						    << vGML.id << " does not exist." << std::endl;
						return atError();
					}
					auto ePair = edge(v, vFromVertexId(idNeighbour), g);
					if(!ePair.second) {
						err << "Error in graph GML. Vertex " << idNeighbour << " in stereo embedding for vertex " << vGML.id
						    << " is not a neighbour." << std::endl;
						return atError();
					}
					stereoInference.addEdge(v, ePair.first);
				} else if(const char *virtPtr = boost::get<char>(&e)) {
					switch(*virtPtr) {
					case 'e':
						stereoInference.addLonePair(v);
						break;
					case 'r':
						stereoInference.addRadical(v);
						break;
					default:
						MOD_ABORT; // the parser should know what is allowed
					}
				} else {
					MOD_ABORT; // the parser should know what is allowed
				}
			}
		}
		// Fixation
		//..........................................................................
		if(embGML.fixation) {
			// TODO: expand this when more complicated geometries are implemented
			bool isFixed = embGML.fixation.get();
			if(isFixed) stereoInference.fixSimpleGeometry(v);
		}
	} // end of explicit stereo data

	auto stereoResult = stereoInference.finalize(ssErr, [&idMap](Vertex v) {
		auto iter = idMap.find(v);
		assert(iter != idMap.end());
		return iter->second;
	});
	switch(stereoResult) {
	case lib::Stereo::DeductionResult::Success:
		break;
	case lib::Stereo::DeductionResult::Warning:
		if(!getConfig().stereo.silenceDeductionWarnings.get())
			IO::log() << ssErr.str();
		break;
	case lib::Stereo::DeductionResult::Error:
		err << ssErr.str();
		return atError();
	}

	data.pStereo = std::make_unique<lib::Graph::PropStereo>(*data.g, std::move(stereoInference));
	return data;
}

} // namespace

Data gml(std::istream &s, std::ostream &err) {
	return parseGML(s, err);
}

Data dfs(const std::string &dfs, std::ostream &err) {
	return lib::Graph::DFSEncoding::parse(dfs, err);
}

Data smiles(const std::string &smiles, std::ostream &err, const bool allowAbstract, SmilesClassPolicy classPolicy) {
	return lib::Chem::readSmiles(smiles, err, allowAbstract, classPolicy);
}

} // namespace Read
} // namespace Graph
} // namespace IO
} // namespace lib
} // namespace mod
