#include "Graph.h"

#include <mod/Error.h>
#include <mod/GraphGraphInterface.h>
#include <mod/GraphPrinter.h>
#include <mod/Misc.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/Graph/Properties/Depiction.h>
#include <mod/lib/Graph/Properties/Molecule.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/GraphPimpl.h>
#include <mod/lib/IO/Graph.h>

#include <boost/graph/connected_components.hpp>

#include <cassert>

namespace mod {

//------------------------------------------------------------------------------
// Graph
//------------------------------------------------------------------------------

struct Graph::Pimpl {

	Pimpl(std::unique_ptr<lib::Graph::Single> g) : g(std::move(g)) {
		assert(this->g);
	}
public:
	const std::unique_ptr<lib::Graph::Single> g;
	std::unique_ptr<const std::map<int, std::size_t> > externalToInternalIds;
};

Graph::Graph(std::unique_ptr<lib::Graph::Single> g) : p(new Pimpl(std::move(g))) { }

Graph::~Graph() { }

std::size_t Graph::getId() const {
	return p->g->getId();
}

std::ostream &operator<<(std::ostream &s, const Graph &g) {
	return s << "'" << g.getGraph().getName() << "'";
}

lib::Graph::Single &Graph::getGraph() const {
	return *p->g;
}

//------------------------------------------------------------------------------

std::size_t Graph::numVertices() const {
	return num_vertices(p->g->getGraph());
}

Graph::VertexRange Graph::vertices() const {
	return VertexRange(p->g->getAPIReference());
}

std::size_t Graph::numEdges() const {
	return num_edges(p->g->getGraph());
}

Graph::EdgeRange Graph::edges() const {
	return EdgeRange(p->g->getAPIReference());
}

//------------------------------------------------------------------------------

std::pair<std::string, std::string> Graph::print() const {
	GraphPrinter first;
	GraphPrinter second;
	second.setMolDefault();
	return print(first, second);
}

std::pair<std::string, std::string> Graph::print(const GraphPrinter &first, const GraphPrinter &second) const {
	return lib::IO::Graph::Write::summary(*p->g, first.getOptions(), second.getOptions());
}

std::string Graph::getGMLString(bool withCoords) const {
	if(withCoords && !getGraph().getDepictionData().getHasCoordinates())
		throw LogicError("Coordinates are not available for this graph (" + getName() + ").");
	std::stringstream ss;
	lib::IO::Graph::Write::gml(getGraph(), withCoords, ss);
	return ss.str();
}

std::string Graph::printGML(bool withCoords) const {
	if(withCoords && !getGraph().getDepictionData().getHasCoordinates())
		throw LogicError("Coordinates are not available for this graph (" + getName() + ").");
	return lib::IO::Graph::Write::gml(getGraph(), withCoords);
}

const std::string &Graph::getName() const {
	return p->g->getName();
}

void Graph::setName(std::string name) const {
	p->g->setName(name);
}

const std::string &Graph::getSmiles() const {
	return p->g->getSmiles();
}

const std::string &Graph::getGraphDFS() const {
	return p->g->getGraphDFS().first;
}

const std::string &Graph::getLinearEncoding() const {
	if(p->g->getMoleculeState().getIsMolecule()) return p->g->getSmiles();
	else return p->g->getGraphDFS().first;
}

bool Graph::getIsMolecule() const {
	return p->g->getMoleculeState().getIsMolecule();
}

double Graph::getEnergy() const {
	if(getIsMolecule()) return p->g->getMoleculeState().getEnergy();
	else return std::numeric_limits<double>::quiet_NaN();
}

void Graph::cacheEnergy(double value) const {
	if(getIsMolecule()) p->g->getMoleculeState().cacheEnergy(value);
	else throw LogicError("Graph::cacheEnergy: Caching of energy failed. '" + getName() + "' is not a molecule.\n");
}

double Graph::getMolarMass() const {
	if(getIsMolecule()) return p->g->getMoleculeState().getMolarMass();
	else return std::numeric_limits<double>::quiet_NaN();
}

unsigned int Graph::vLabelCount(const std::string &label) const {
	return p->g->getVertexLabelCount(label);
}

unsigned int Graph::eLabelCount(const std::string &label) const {
	return p->g->getEdgeLabelCount(label);
}

std::size_t Graph::isomorphism(std::shared_ptr<mod::Graph> g, std::size_t maxNumMatches) const {
	return lib::Graph::Single::isomorphismVF2(getGraph(), g->getGraph(), maxNumMatches);
}

std::size_t Graph::monomorphism(std::shared_ptr<mod::Graph> g, std::size_t maxNumMatches) const {
	return lib::Graph::Single::monomorphism(getGraph(), g->getGraph(), maxNumMatches);
}

std::shared_ptr<Graph> Graph::makePermutation() const {
	auto gPerm = makeGraph(std::make_unique<lib::Graph::Single>(lib::Graph::makePermutation(getGraph())));
	gPerm->setName(getName() + " perm");
	return gPerm;
}

void Graph::setImage(std::shared_ptr<mod::Function<std::string()> > image) {
	getGraph().getDepictionData().setImage(image);
}

std::shared_ptr<mod::Function<std::string()> > Graph::getImage() const {
	return getGraph().getDepictionData().getImage();
}

void Graph::setImageCommand(std::string cmd) {
	getGraph().getDepictionData().setImageCommand(cmd);
}

std::string Graph::getImageCommand() const {
	return getGraph().getDepictionData().getImageCommand();
}

Graph::Vertex Graph::getVertexFromExternalId(int id) const {
	if(!p->externalToInternalIds) return Vertex();
	auto iter = p->externalToInternalIds->find(id);
	if(iter == end(*p->externalToInternalIds)) return Vertex();
	return Vertex(p->g->getAPIReference(), iter->second);
}

//------------------------------------------------------------------------------
// Static
//------------------------------------------------------------------------------

namespace {

std::shared_ptr<Graph> handleLoadedGraph(lib::IO::Graph::Read::Data data, const std::string &source,
		std::ostringstream &err) {
	if(!data.graph) {
		throw InputError("Error in graph loading from " + source + ".\n" + err.str());
	}
	{ // check connectedness
		std::vector<std::size_t> cMap(num_vertices(*data.graph));
		auto numComponents = boost::connected_components(*data.graph, cMap.data());
		if(numComponents > 1) {
			data.clear();
			throw InputError("Error in graph loading from " + source
					+ ".\nThe graph is not connected (" + std::to_string(numComponents) + " components).");
		}
	}
	auto gInternal = std::make_unique<lib::Graph::Single>(std::move(data.graph), std::move(data.label));
	std::shared_ptr<Graph> g = Graph::makeGraph(std::move(gInternal), std::move(data.externalToInternalIds));
	return g;
}

} // namespace

std::shared_ptr<Graph> Graph::graphGMLString(const std::string &data) {
	std::istringstream ss(data);
	std::ostringstream err;
	auto gData = lib::IO::Graph::Read::gml(ss, err);
	return handleLoadedGraph(std::move(gData), "inline GML string", err);
}

std::shared_ptr<Graph> Graph::graphGML(const std::string &file) {
	std::string fullFilename = prefixFilename(file);
	std::ifstream ifs(fullFilename);
	std::ostringstream err;
	if(!ifs) {
		err << "Could not open graph GML file '" << file << "' ('" << fullFilename << "')." << std::endl;
		throw InputError(err.str());
	}
	auto gData = lib::IO::Graph::Read::gml(ifs, err);
	return handleLoadedGraph(std::move(gData), "file, '" + file + "'", err);
}

std::shared_ptr<Graph> Graph::graphDFS(const std::string &graphDFS) {
	std::ostringstream err;
	auto gData = lib::IO::Graph::Read::dfs(graphDFS, err);
	return handleLoadedGraph(std::move(gData), "graphDFS, '" + graphDFS + "'", err);
}

std::shared_ptr<Graph> Graph::smiles(const std::string &smiles) {
	std::ostringstream err;
	auto gData = lib::IO::Graph::Read::smiles(smiles, err);
	return handleLoadedGraph(std::move(gData), "smiles string, '" + smiles + "'", err);
}

std::shared_ptr<Graph> Graph::makeGraph(std::unique_ptr<lib::Graph::Single> g) {
	return makeGraph(std::move(g), { });
}

std::shared_ptr<Graph> Graph::makeGraph(std::unique_ptr<lib::Graph::Single> g, std::map<int, std::size_t> externalToInternalIds) {
	if(!g) MOD_ABORT;
	std::shared_ptr<Graph> wrapped = std::shared_ptr<Graph > (new Graph(std::move(g)));
	wrapped->p->g->setAPIReference(wrapped);
	if(!externalToInternalIds.empty()) {
		wrapped->p->externalToInternalIds = std::make_unique<std::map<int, std::size_t> >(std::move(externalToInternalIds));
	}
	return wrapped;
}

} // namespace mod