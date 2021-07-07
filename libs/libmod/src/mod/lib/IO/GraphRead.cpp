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

#include <iostream>

namespace mod::lib::IO::Graph::Read {

Data::Data() {}

Data::Data(Data &&other) : g(std::move(other.g)), pString(std::move(other.pString)), pStereo(std::move(other.pStereo)),
                           externalToInternalIds(std::move(other.externalToInternalIds)) {}

Data::~Data() {
	if(std::uncaught_exceptions() != 0) return;
	if(g) MOD_ABORT;
	if(pString) MOD_ABORT;
	if(pStereo) MOD_ABORT;
}

void Data::reset() {
	pStereo.reset();
	pString.reset();
	g.reset();
}

Result<std::vector<Data>> gml(lib::IO::Warnings &warnings, std::string_view src) {
	GML::Graph gGML;
	{
		gml::ast::KeyValue ast;
		try {
			ast = gml::parser::parse(src);
		} catch(const gml::parser::error &e) {
			return Result<>::Error(e.what());
		}
		using namespace gml::converter::edsl;
		auto cVertex = GML::makeVertexConverter(1);
		auto cEdge = GML::makeEdgeConverter(1);
		auto cGraph = list<Parent>("graph")(cVertex)(cEdge);
		auto iterBegin = &ast;
		auto iterEnd = iterBegin + 1;
		try {
			gml::converter::convert(iterBegin, iterEnd, cGraph, gGML);
		} catch(const gml::converter::error &e) {
			return Result<>::Error(e.what());
		}
	}

	std::sort(begin(gGML.vertices), end(gGML.vertices), [](const GML::Vertex &v1, const GML::Vertex &v2) -> bool {
		return v1.id < v2.id;
	});

	// Check that is mathematically is a graph

	std::unordered_map<int, int> globalIddFromExtID;
	{
		int i = 0;
		for(const auto &vGML : gGML.vertices) {
			if(globalIddFromExtID.find(vGML.id) != end(globalIddFromExtID))
				return Result<>::Error("Vertex id " + std::to_string(vGML.id) + " used multiple times.");
			globalIddFromExtID.emplace(vGML.id, i++);
		}
	}

	for(const auto &eGML : gGML.edges) {
		if(eGML.source == eGML.target)
			return Result<>::Error("Loop edge (on " + std::to_string(eGML.source) + ") is not allowed.");
		if(globalIddFromExtID.find(eGML.source) == end(globalIddFromExtID))
			return Result<>::Error("Source " + std::to_string(eGML.source) + " does not exist in '"
			                       + boost::lexical_cast<std::string>(eGML) + "'.");
		if(globalIddFromExtID.find(eGML.target) == end(globalIddFromExtID))
			return Result<>::Error("Target " + std::to_string(eGML.target) + " does not exist in '"
			                       + boost::lexical_cast<std::string>(eGML) + "'.");
	}

	// Now we can calculate connected components and start converting data

	ConnectedComponents components(gGML.vertices.size());
	for(const auto &e : gGML.edges) {
		const auto iterSrc = globalIddFromExtID.find(e.source);
		const auto iterTar = globalIddFromExtID.find(e.target);
		components.join(iterSrc->second, iterTar->second);
	}
	const auto numComponents = components.finalize();

	std::vector<Data> datas(numComponents);
	std::vector<std::unordered_map<lib::Graph::Vertex, int>> extIDFromVertex(numComponents);
	for(auto &d : datas) {
		d.g = std::make_unique<lib::Graph::GraphType>();
		d.pString = std::make_unique<lib::Graph::PropString>(*d.g);
	}

	const auto atError = [&datas](std::string msg) -> Result<> {
		for(auto &d : datas) d.reset();
		return Result<>::Error(std::move(msg));
	};

	for(const auto &vGML : gGML.vertices) {
		const auto comp = components[globalIddFromExtID.find(vGML.id)->second];
		auto &g = *datas[comp].g;
		const auto v = add_vertex(g);
		assert(vGML.label);
		datas[comp].pString->addVertex(v, *vGML.label);
		datas[comp].externalToInternalIds.emplace(vGML.id, get(boost::vertex_index_t(), g, v));
		extIDFromVertex[comp].emplace(v, vGML.id);
	}
	const auto vFromVertexId = [&](int id) {
		const auto globalIDIter = globalIddFromExtID.find(id);
		assert(globalIDIter != end(globalIddFromExtID));
		const auto comp = components[globalIDIter->second];
		const auto vIdIter = datas[comp].externalToInternalIds.find(id);
		assert(vIdIter != end(datas[comp].externalToInternalIds));
		return std::pair(comp, vertex(vIdIter->second, *datas[comp].g));
	};
	for(const auto &eGML : gGML.edges) {
		const auto[comp, vSrc] = vFromVertexId(eGML.source);
		const auto[compTar, vTar] = vFromVertexId(eGML.target);
		assert(comp == compTar);
		auto &g = *datas[comp].g;
		const auto eQuery = edge(vSrc, vTar, g);
		if(eQuery.second)
			return atError("Duplicate edge with source " + std::to_string(eGML.source)
			               + " and target " + std::to_string(eGML.target) + ".");
		const auto e = add_edge(vSrc, vTar, g);
		assert(eGML.label);
		datas[comp].pString->addEdge(e.first, *eGML.label);
	}

	bool doStereo = false;
	for(const auto &vGML : gGML.vertices) doStereo = doStereo || vGML.stereo;
	for(const auto &eGML : gGML.edges) doStereo = doStereo || eGML.stereo;
	if(!doStereo) return std::move(datas); // TODO: remove std::move when C++20/P1825R0 is available
	// Stereo
	//============================================================================
	std::vector<lib::Graph::PropMolecule> molStates;
	std::vector<lib::Stereo::Inference<lib::Graph::GraphType, lib::Graph::PropMolecule>> stereoInferences;
	molStates.reserve(numComponents);
	stereoInferences.reserve(numComponents);
	for(int i = 0; i != numComponents; ++i) {
		const auto &g = *datas[i].g;
		molStates.emplace_back(g, *datas[i].pString);
		stereoInferences.push_back(lib::Stereo::makeInference(g, molStates.back(), false));
	}

	const auto &gGeometry = lib::Stereo::getGeometryGraph();
	// Set the explicitly defined edge categories.
	//----------------------------------------------------------------------------
	for(const auto &eGML : gGML.edges) {
		const auto[comp, vSrc] = vFromVertexId(eGML.source);
		const auto[compTar, vTar] = vFromVertexId(eGML.target);
		assert(comp == compTar);
		const auto &g = *datas[comp].g;
		const auto ePair = edge(vSrc, vTar, g);
		assert(ePair.second);
		if(!eGML.stereo) continue;
		const std::string &s = *eGML.stereo;
		if(s.size() != 1)
			return atError(
					"Error in stereo data for edge (" + std::to_string(eGML.source)
					+ ", " + std::to_string(eGML.target) + "). Parsing error in stereo data '" + s + "'.");
		lib::Stereo::EdgeCategory cat;
		switch(s.front()) {
		case '*':
			cat = lib::Stereo::EdgeCategory::Any;
			break;
		default:
			return atError("Error in stereo data for edge (" + std::to_string(eGML.source)
			               + ", " + std::to_string(eGML.target) + "). Parsing error in stereo data '" + s + "'.");
		}
		if(auto res = stereoInferences[comp].assignEdgeCategory(ePair.first, cat); !res)
			return atError("Error in stereo data for edge (" + std::to_string(eGML.source)
			               + ", " + std::to_string(eGML.target) + "). "
			               + res.extractError());
	}
	// Set the explicitly stereo data.
	//----------------------------------------------------------------------------
	for(auto &vGML : gGML.vertices) {
		if(!vGML.stereo) continue;
		const auto[comp, v] = vFromVertexId(vGML.id);
		if(auto res = lib::IO::Stereo::Read::parseEmbedding(*vGML.stereo)) {
			vGML.parsedEmbedding = std::move(*res);
		} else {
			return atError("Error in stereo data for vertex " + std::to_string(vGML.id) + ". "
			               + res.extractError());
		}
		// Geometry
		//..........................................................................
		const auto &embGML = *vGML.parsedEmbedding;
		if(embGML.geometry) {
			const auto vGeo = gGeometry.findGeometry(*embGML.geometry);
			if(vGeo == gGeometry.nullGeometry())
				return atError("Error in stereo data for vertex " + std::to_string(vGML.id)
				               + ". Invalid gGeometry '" + *embGML.geometry + "'.");
			if(auto res = stereoInferences[comp].assignGeometry(v, vGeo); !res)
				return atError("Error in stereo data for vertex " + std::to_string(vGML.id) + ". "
				               + res.extractError());
		}
		// Edges
		//..........................................................................
		if(embGML.edges) {
			stereoInferences[comp].initEmbedding(v);
			for(const auto &e : *embGML.edges) {
				if(const int *idPtr = std::get_if<int>(&e)) {
					const auto extIDNeighbour = *idPtr;
					if(globalIddFromExtID.find(extIDNeighbour) == end(globalIddFromExtID))
						return atError("Neighbour vertex " + std::to_string(extIDNeighbour)
						               + " in stereo embedding for vertex " + std::to_string(vGML.id) + " does not exist.");
					const auto[compNeighbour, vNeighbour] = vFromVertexId(extIDNeighbour);
					const auto ePair = edge(v, vNeighbour, *datas[comp].g);
					if(!ePair.second)
						return atError("Error in graph GML. Vertex " + std::to_string(extIDNeighbour) +
						               " in stereo embedding for vertex " + std::to_string(vGML.id) + " is not a neighbour.");
					assert(compNeighbour == comp);
					stereoInferences[comp].addEdge(v, ePair.first);
				} else if(const char *virtPtr = std::get_if<char>(&e)) {
					switch(*virtPtr) {
					case 'e':
						stereoInferences[comp].addLonePair(v);
						break;
					case 'r':
						stereoInferences[comp].addRadical(v);
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
			const bool isFixed = *embGML.fixation;
			if(isFixed) stereoInferences[comp].fixSimpleGeometry(v);
		}
	} // end of explicit stereo data

	for(int comp = 0; comp != numComponents; ++comp) {
		// TODO: the warning should only be printed once, instead of for each connected component
		lib::IO::Warnings stereoWarnings;
		auto stereoResult = stereoInferences[comp].finalize(
				stereoWarnings, [comp, &extIDFromVertex](lib::Graph::Vertex v) {
					const auto iter = extIDFromVertex[comp].find(v);
					assert(iter != extIDFromVertex[comp].end());
					return std::to_string(iter->second);
				});
		warnings.addFrom(std::move(stereoWarnings), !getConfig().stereo.silenceDeductionWarnings.get());
		if(!stereoResult)
			return atError(stereoResult.extractError());
		datas[comp].pStereo = std::make_unique<lib::Graph::PropStereo>(*datas[comp].g, std::move(stereoInferences[comp]));
	}
	return std::move(datas); // TODO: remove std::move when C++20/P1825R0 is available
}

Result<Data> dfs(const std::string &dfs) {
	return lib::Graph::DFSEncoding::parse(dfs);
}

Result<std::vector<Data>>
smiles(lib::IO::Warnings &warnings, const std::string &smiles, const bool allowAbstract, SmilesClassPolicy classPolicy) {
	return lib::Chem::readSmiles(warnings, smiles, allowAbstract, classPolicy);
}

} // namespace mod::lib::IO::Graph::Read