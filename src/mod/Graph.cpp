#include "Graph.h"

#include <mod/Chem.h>
#include <mod/Config.h>
#include <mod/Error.h>
#include <mod/Misc.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/Graph/Properties/Depiction.h>
#include <mod/lib/GraphPimpl.h>
#include <mod/lib/IO/Graph.h>
#include <mod/lib/IO/IO.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>
#include <jla_boost/Memory.hpp>

#include <boost/concept_check.hpp>
#include <boost/graph/connected_components.hpp>

#include <limits>
#include <sstream>

namespace mod {

//------------------------------------------------------------------------------
// Vertex
//------------------------------------------------------------------------------

MOD_GRAPHPIMPL_Define_Vertex(Graph, graph, g->getGraph().getGraph())
MOD_GRAPHPIMPL_Define_Vertex_Undirected(Graph, graph, g->getGraph().getGraph())

const std::string &Graph::Vertex::getStringLabel() const {
	if(!g) throw LogicError("Can not get string label on a null vertex.");
	const auto &graph = g->getGraph().getGraph();
	using boost::vertices;
	auto v = *(vertices(graph).first + vId);
	return g->getGraph().getStringState()[v];
}

AtomId Graph::Vertex::getAtomId() const {
	if(!g) throw LogicError("Can not get atom id on a null vertex.");
	const auto &graph = g->getGraph().getGraph();
	using boost::vertices;
	auto v = *(vertices(graph).first + vId);
	return g->getGraph().getMoleculeState()[v].getAtomId();
}

Charge Graph::Vertex::getCharge() const {
	if(!g) throw LogicError("Can not get charge on a null vertex.");
	const auto &graph = g->getGraph().getGraph();
	using boost::vertices;
	auto v = *(vertices(graph).first + vId);
	return g->getGraph().getMoleculeState()[v].getCharge();
}

//------------------------------------------------------------------------------
// Edge
//------------------------------------------------------------------------------

MOD_GRAPHPIMPL_Define_Indices(Graph, graph, g->getGraph().getGraph())

BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Graph::VertexIterator>));
BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Graph::EdgeIterator>));
BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Graph::IncidentEdgeIterator>));

const std::string &Graph::Edge::getStringLabel() const {
	if(!g) throw LogicError("Can not get string label on a null edge.");
	const auto &graph = g->getGraph().getGraph();
	using boost::vertices;
	auto v = *(vertices(graph).first + vId);
	auto e = *(out_edges(v, graph).first + eId);
	return g->getGraph().getStringState()[e];
}

BondType Graph::Edge::getBondType() const {
	if(!g) throw LogicError("Can not get bond type on a null edge.");
	const auto &graph = g->getGraph().getGraph();
	using boost::vertices;
	auto v = *(vertices(graph).first + vId);
	auto e = *(out_edges(v, graph).first + eId);
	return g->getGraph().getMoleculeState()[e];
}

//------------------------------------------------------------------------------
// Graph
//------------------------------------------------------------------------------

struct Graph::Pimpl {
	Pimpl(std::unique_ptr<lib::Graph::Single> g);
public:
	const std::unique_ptr<lib::Graph::Single> g;
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
	auto gPerm = makeGraph(make_unique<lib::Graph::Single>(lib::Graph::makePermutation(getGraph())));
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

//------------------------------------------------------------------------------
// Pimpl impl
//------------------------------------------------------------------------------

Graph::Pimpl::Pimpl(std::unique_ptr<lib::Graph::Single> g) : g(std::move(g)) {
	assert(this->g);
}

//------------------------------------------------------------------------------
// Static
//------------------------------------------------------------------------------

namespace {

std::shared_ptr<Graph> handleLoadedGraph(
		std::unique_ptr<lib::Graph::GraphType> gBoost,
		std::unique_ptr<lib::Graph::PropStringType> pString, const std::string &source,
		std::ostringstream &err) {
	if(!gBoost || !pString)
		throw InputError("Error in graph loading from " + source + ".\n" + err.str());
	{ // check connectedness
		std::vector<std::size_t> cMap(num_vertices(*gBoost));
		auto numComponents = boost::connected_components(*gBoost, cMap.data());
		if(numComponents > 1)
			throw InputError("Error in graph loading from " + source
				+ ".\nThe graph is not connected (" + std::to_string(numComponents) + " components).");
	}
	auto gInternal = make_unique<lib::Graph::Single>(std::move(gBoost), std::move(pString));
	std::shared_ptr<Graph> g = Graph::makeGraph(std::move(gInternal));
	return g;
}

} // namespace

std::shared_ptr<Graph> Graph::graphGMLString(const std::string &data) {
	std::istringstream ss(data);
	std::ostringstream err;
	auto gData = lib::IO::Graph::Read::gml(ss, err);
	return handleLoadedGraph(std::move(gData.first), std::move(gData.second), "inline GML string", err);
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
	return handleLoadedGraph(std::move(gData.first), std::move(gData.second), "file, '" + file + "'", err);
}

std::shared_ptr<Graph> Graph::graphDFS(const std::string &graphDFS) {
	std::ostringstream err;
	auto gData = lib::IO::Graph::Read::dfs(graphDFS, err);
	return handleLoadedGraph(std::move(gData.first), std::move(gData.second), "graphDFS, '" + graphDFS + "'", err);
}

std::shared_ptr<Graph> Graph::smiles(const std::string &smiles) {
	std::ostringstream err;
	auto gData = lib::IO::Graph::Read::smiles(smiles, err);
	return handleLoadedGraph(std::move(gData.first), std::move(gData.second), "smiles string, '" + smiles + "'", err);
}

std::shared_ptr<Graph> Graph::makeGraph(std::unique_ptr<lib::Graph::Single> g) {
	if(!g) MOD_ABORT;
	std::shared_ptr<Graph> wrapped = std::shared_ptr<Graph > (new Graph(std::move(g)));
	wrapped->p->g->setAPIReference(wrapped);
	return wrapped;
}

//------------------------------------------------------------------------------
// GraphPrinter
//------------------------------------------------------------------------------

GraphPrinter::GraphPrinter() : options(new lib::IO::Graph::Write::Options()) {
	options->EdgesAsBonds(true).RaiseCharges(true);
}

GraphPrinter::~GraphPrinter() { }

const lib::IO::Graph::Write::Options &GraphPrinter::getOptions() const {
	return *options;
}

void GraphPrinter::setMolDefault() {
	options->All().Thick(false).WithIndex(false);
}

void GraphPrinter::setReactionDefault() {
	options->All().Thick(false).WithIndex(false).SimpleCarbons(false);
}

void GraphPrinter::disableAll() {
	options->Non();
}

void GraphPrinter::enableAll() {
	options->All();
}

void GraphPrinter::setEdgesAsBonds(bool value) {
	options->EdgesAsBonds(value);
}

bool GraphPrinter::getEdgesAsBonds() const {
	return options->edgesAsBonds;
}

void GraphPrinter::setCollapseHydrogens(bool value) {
	options->CollapseHydrogens(value);
}

bool GraphPrinter::getCollapseHydrogens() const {
	return options->collapseHydrogens;
}

void GraphPrinter::setRaiseCharges(bool value) {
	options->RaiseCharges(value);
}

bool GraphPrinter::getRaiseCharges() const {
	return options->raiseCharges;
}

void GraphPrinter::setSimpleCarbons(bool value) {
	options->SimpleCarbons(value);
}

bool GraphPrinter::getSimpleCarbons() const {
	return options->simpleCarbons;
}

void GraphPrinter::setThick(bool value) {
	options->Thick(value);
}

bool GraphPrinter::getThick() const {
	return options->thick;
}

void GraphPrinter::setWithColour(bool value) {
	options->WithColour(value);
}

bool GraphPrinter::getWithColour() const {
	return options->withColour;
}

void GraphPrinter::setWithIndex(bool value) {
	options->WithIndex(value);
}

bool GraphPrinter::getWithIndex() const {
	return options->withIndex;
}

void GraphPrinter::setWithTexttt(bool value) {
	options->WithTexttt(value);
}

bool GraphPrinter::getWithTexttt() const {
	return options->withTexttt;
}

} // namespace mod
