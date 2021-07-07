#include "Graph.hpp"

#include <mod/Error.hpp>
#include <mod/graph/Automorphism.hpp>
#include <mod/graph/GraphInterface.hpp>
#include <mod/graph/Printer.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/Graph/Properties/Depiction.hpp>
#include <mod/lib/Graph/Properties/Molecule.hpp>
#include <mod/lib/Graph/Properties/Stereo.hpp>
#include <mod/lib/Graph/Properties/String.hpp>
#include <mod/lib/Graph/Properties/Term.hpp>
#include <mod/lib/IO/Graph.hpp>

#include <boost/graph/connected_components.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#include <cassert>
#include <fstream>
#include <iostream>

namespace mod::graph {

// Graph
//------------------------------------------------------------------------------

struct Graph::ExternalData {
	std::map<int, std::size_t> externalToInternalIds;
	std::vector<std::pair<std::string, bool>> warnings;
};

Graph::Graph(std::unique_ptr<lib::Graph::Single> g) : g(std::move(g)) {}

Graph::~Graph() = default;

std::size_t Graph::getId() const {
	return g->getId();
}

std::ostream &operator<<(std::ostream &s, const Graph &g) {
	return s << "'" << g.g->getName() << "'";
}

lib::Graph::Single &Graph::getGraph() const {
	return *g;
}

//------------------------------------------------------------------------------

std::size_t Graph::numVertices() const {
	return num_vertices(g->getGraph());
}

Graph::VertexRange Graph::vertices() const {
	return VertexRange(g->getAPIReference());
}

std::size_t Graph::numEdges() const {
	return num_edges(g->getGraph());
}

Graph::EdgeRange Graph::edges() const {
	return EdgeRange(g->getAPIReference());
}

//------------------------------------------------------------------------------

Graph::AutGroup Graph::aut(LabelSettings labelSettings) const {
	return AutGroup(g->getAPIReference(), labelSettings);
}

//------------------------------------------------------------------------------

std::pair<std::string, std::string> Graph::print() const {
	Printer first;
	Printer second;
	second.setMolDefault();
	return print(first, second);
}

std::pair<std::string, std::string> Graph::print(const Printer &first, const Printer &second) const {
	return lib::IO::Graph::Write::summary(*g, first.getOptions(), second.getOptions());
}

void Graph::printTermState() const {
	lib::IO::Graph::Write::termState(*g);
}

std::string Graph::getGMLString(bool withCoords) const {
	if(withCoords && !g->getDepictionData().getHasCoordinates())
		throw LogicError("Coordinates are not available for this graph (" + getName() + ").");
	std::stringstream ss;
	lib::IO::Graph::Write::gml(*g, withCoords, ss);
	return ss.str();
}

std::string Graph::printGML(bool withCoords) const {
	if(withCoords && !g->getDepictionData().getHasCoordinates())
		throw LogicError("Coordinates are not available for this graph (" + getName() + ").");
	return lib::IO::Graph::Write::gml(*g, withCoords);
}

const std::string &Graph::getName() const {
	return g->getName();
}

void Graph::setName(std::string name) const {
	g->setName(name);
}

const std::string &Graph::getSmiles() const {
	return g->getSmiles();
}

const std::string &Graph::getSmilesWithIds() const {
	return g->getSmilesWithIds();
}

const std::string &Graph::getGraphDFS() const {
	return g->getGraphDFS().first;
}

const std::string &Graph::getGraphDFSWithIds() const {
	return g->getGraphDFSWithIds();
}

const std::string &Graph::getLinearEncoding() const {
	if(g->getMoleculeState().getIsMolecule()) return g->getSmiles();
	else return g->getGraphDFS().first;
}

bool Graph::getIsMolecule() const {
	return g->getMoleculeState().getIsMolecule();
}

double Graph::getEnergy() const {
	if(getIsMolecule()) return g->getMoleculeState().getEnergy();
	else return std::numeric_limits<double>::quiet_NaN();
}

void Graph::cacheEnergy(double value) const {
	if(getIsMolecule()) g->getMoleculeState().cacheEnergy(value);
	else throw LogicError("Graph::cacheEnergy: Caching of energy failed. '" + getName() + "' is not a molecule.\n");
}

double Graph::getExactMass() const {
	if(!getIsMolecule()) throw LogicError("Can not get exact mass of a non-molecule.");
	return g->getMoleculeState().getExactMass();
}

unsigned int Graph::vLabelCount(const std::string &label) const {
	return g->getVertexLabelCount(label);
}

unsigned int Graph::eLabelCount(const std::string &label) const {
	return g->getEdgeLabelCount(label);
}

namespace {

void checkTermParsing(const lib::Graph::Single &g, LabelSettings ls) {
	if(ls.type == LabelType::Term) {
		const auto &term = get_term(g.getLabelledGraph());
		if(!isValid(term)) {
			std::string msg = "Parsing failed for graph '" + g.getName() + "'. " + term.getParsingError();
			throw TermParsingError(std::move(msg));
		}
	}
}

} // namespace

std::size_t
Graph::isomorphism(std::shared_ptr<graph::Graph> host, std::size_t maxNumMatches, LabelSettings labelSettings) const {
	checkTermParsing(*g, labelSettings);
	checkTermParsing(*host->g, labelSettings);
	return lib::Graph::Single::isomorphism(*g, *host->g, maxNumMatches, labelSettings);
}

std::size_t
Graph::monomorphism(std::shared_ptr<graph::Graph> host, std::size_t maxNumMatches, LabelSettings labelSettings) const {
	checkTermParsing(*g, labelSettings);
	checkTermParsing(*host->g, labelSettings);
	return lib::Graph::Single::monomorphism(*g, *host->g, maxNumMatches, labelSettings);
}

std::shared_ptr<Graph> Graph::makePermutation() const {
	auto gPerm = create(std::make_unique<lib::Graph::Single>(lib::Graph::makePermutation(*g)));
	gPerm->setName(getName() + " perm");
	return gPerm;
}

void Graph::setImage(std::shared_ptr<mod::Function<std::string()>> image) {
	g->getDepictionData().setImage(image);
}

std::shared_ptr<mod::Function<std::string()> > Graph::getImage() const {
	return g->getDepictionData().getImage();
}

void Graph::setImageCommand(std::string cmd) {
	g->getDepictionData().setImageCommand(cmd);
}

std::string Graph::getImageCommand() const {
	return g->getDepictionData().getImageCommand();
}

void Graph::instantiateStereo() const {
	get_stereo(g->getLabelledGraph());
}

Graph::Vertex Graph::getVertexFromExternalId(int id) const {
	if(!externalData || externalData->externalToInternalIds.empty()) return Vertex();
	const auto iter = externalData->externalToInternalIds.find(id);
	if(iter == end(externalData->externalToInternalIds)) return Vertex();
	return Vertex(g->getAPIReference(), iter->second);
}

int Graph::getMinExternalId() const {
	if(!externalData || externalData->externalToInternalIds.empty()) return 0;
	return externalData->externalToInternalIds.begin()->first;
}

int Graph::getMaxExternalId() const {
	if(!externalData || externalData->externalToInternalIds.empty()) return 0;
	return (--externalData->externalToInternalIds.end())->first;
}

std::vector<std::pair<std::string, bool>> Graph::getLoadingWarnings() const {
	if(!externalData) throw LogicError("Can not get loading warnings. No data from external loading stored.");
	return externalData->warnings;
}

//------------------------------------------------------------------------------
// Static
//------------------------------------------------------------------------------

namespace {

std::shared_ptr<Graph>
makeGraphFromData(lib::IO::Graph::Read::Data data, std::vector<std::pair<std::string, bool>> warnings) {
	auto gInternal = std::make_unique<lib::Graph::Single>(
			std::move(data.g), std::move(data.pString), std::move(data.pStereo));
	std::shared_ptr<Graph> g = Graph::create(std::move(gInternal),
	                                         std::move(data.externalToInternalIds),
	                                         std::move(warnings));
	return g;
}

std::shared_ptr<Graph>
handleLoadedGraph(lib::IO::Result<std::vector<lib::IO::Graph::Read::Data>> dataRes, lib::IO::Warnings warnings,
                  const std::string &source) {
	std::cout << warnings << std::flush;
	if(!dataRes) throw InputError("Error in graph loading from " + source + ".\n" + dataRes.extractError());
	auto data = std::move(*dataRes);
	if(data.size() != 1)
		throw InputError("Error in graph loading from " + source
		                 + ".\nThe graph is not connected (" + std::to_string(data.size()) + " components).");
	return makeGraphFromData(std::move(data.front()), warnings.extractWarnings());
}

std::vector<std::shared_ptr<Graph>>
handleLoadedGraphs(lib::IO::Result<std::vector<lib::IO::Graph::Read::Data>> dataRes, lib::IO::Warnings warnings,
                   const std::string &source) {
	std::cout << warnings << std::flush;
	if(!dataRes) throw InputError("Error in graph loading from " + source + ".\n" + dataRes.extractError());
	auto data = std::move(*dataRes);
	// the warnings are copied into each graph
	const auto warningList = warnings.extractWarnings();
	std::vector<std::shared_ptr<Graph>> res;
	res.reserve(data.size());
	for(auto &d : data)
		res.push_back(makeGraphFromData(std::move(d), warningList));
	return res;
}

} // namespace

std::shared_ptr<Graph> Graph::fromGMLString(const std::string &data) {
	lib::IO::Warnings warnings;
	auto res = lib::IO::Graph::Read::gml(warnings, data);
	return handleLoadedGraph(std::move(res), std::move(warnings), "inline GML string");
}

std::shared_ptr<Graph> Graph::fromGMLFile(const std::string &file) {
	boost::iostreams::mapped_file_source ifs;
	try {
		ifs.open(file);
	} catch(const BOOST_IOSTREAMS_FAILURE &e) {
		throw InputError("Could not open graph GML file '" + file + "':\n" + e.what());
	}
	if(!ifs) throw InputError("Could not open graph GML file '" + file + "'.\n");
	lib::IO::Warnings warnings;
	auto res = lib::IO::Graph::Read::gml(warnings, {ifs.begin(), ifs.size()});
	return handleLoadedGraph(std::move(res), std::move(warnings), "file, '" + file + "'");
}

std::vector<std::shared_ptr<Graph>> Graph::fromGMLStringMulti(const std::string &data) {
	lib::IO::Warnings warnings;
	auto res = lib::IO::Graph::Read::gml(warnings, data);
	return handleLoadedGraphs(std::move(res), std::move(warnings), "inline GML string");
}

std::vector<std::shared_ptr<Graph>> Graph::fromGMLFileMulti(const std::string &file) {
	boost::iostreams::mapped_file_source ifs;
	try {
		ifs.open(file);
	} catch(const BOOST_IOSTREAMS_FAILURE &e) {
		throw InputError("Could not open graph GML file '" + file + "':\n" + e.what());
	}
	if(!ifs) throw InputError("Could not open graph GML file '" + file + "'.\n");
	lib::IO::Warnings warnings;
	auto res = lib::IO::Graph::Read::gml(warnings, {ifs.begin(), ifs.size()});
	return handleLoadedGraphs(std::move(res), std::move(warnings), "file, '" + file + "'");
}

std::shared_ptr<Graph> Graph::fromDFS(const std::string &graphDFS) {
	auto data = lib::IO::Graph::Read::dfs(graphDFS);
	if(!data) throw InputError("Error in graph loading from graphDFS, '" + graphDFS + "'.\n" + data.extractError());
	return makeGraphFromData(std::move(*data), {});
}

std::shared_ptr<Graph> Graph::fromSMILES(const std::string &smiles) {
	return Graph::fromSMILES(smiles, false, SmilesClassPolicy::NoneOnDuplicate);
}

std::shared_ptr<Graph>
Graph::fromSMILES(const std::string &smiles, const bool allowAbstract, SmilesClassPolicy classPolicy) {
	lib::IO::Warnings warnings;
	auto res = lib::IO::Graph::Read::smiles(warnings, smiles, allowAbstract, classPolicy);
	return handleLoadedGraph(std::move(res), std::move(warnings), "smiles string, '" + smiles + "'");
}

std::vector<std::shared_ptr<Graph>> Graph::fromSMILESMulti(const std::string &smiles) {
	return fromSMILESMulti(smiles, false, SmilesClassPolicy::NoneOnDuplicate);
}

std::vector<std::shared_ptr<Graph>>
Graph::fromSMILESMulti(const std::string &smiles, bool allowAbstract, SmilesClassPolicy classPolicy) {
	lib::IO::Warnings warnings;
	auto res = lib::IO::Graph::Read::smiles(warnings, smiles, allowAbstract, classPolicy);
	return handleLoadedGraphs(std::move(res), std::move(warnings), "smiles string, '" + smiles + "'");
}

std::shared_ptr<Graph> Graph::create(std::unique_ptr<lib::Graph::Single> g) {
	if(!g) MOD_ABORT;
	std::shared_ptr<Graph> wrapped = std::shared_ptr<Graph>(new Graph(std::move(g)));
	wrapped->g->setAPIReference(wrapped);
	return wrapped;
}

std::shared_ptr<Graph> Graph::create(std::unique_ptr<lib::Graph::Single> g,
                                     std::map<int, std::size_t> externalToInternalIds,
                                     std::vector<std::pair<std::string, bool>> warnings) {
	auto wrapped = create(std::move(g));
	auto externalData = std::make_unique<ExternalData>();
	externalData->externalToInternalIds = std::move(externalToInternalIds);
	externalData->warnings = std::move(warnings);
	wrapped->externalData = std::move(externalData);
	return wrapped;
}

} // namespace mod::graph