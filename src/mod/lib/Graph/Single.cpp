#include "Single.h"

#include <mod/Graph.h>
#include <mod/Misc.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/Chem/OBabel.h>
#include <mod/lib/Chem/Smiles.h>
#include <mod/lib/Graph/DFSEncoding.h>
#include <mod/lib/Graph/Properties/Depiction.h>
#include <mod/lib/GraphMorphism/LabelledMorphism.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/IO/Graph.h>
#include <mod/lib/LabelledGraph.h>
#include <mod/lib/Random.h>
#include <mod/lib/Rule/Real.h>

#include <jla_boost/Memory.hpp>
#include <jla_boost/graph/morphism/VF2Finder.hpp>

#include <boost/graph/connected_components.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/lexical_cast.hpp>

namespace mod {
namespace lib {
namespace Graph {
BOOST_CONCEPT_ASSERT((LabelledGraphConcept<LabelledGraph>));

namespace {
std::size_t nextGraphNum = 0;

const std::string getGraphName(unsigned int id) {
	return "g_{" + boost::lexical_cast<std::string>(id) + "}";
}

bool sanityCheck(const GraphType &g, const PropStringType &pString, std::ostream &s) {
	std::vector<std::pair<Vertex, Vertex> > edgesSorted;
	edgesSorted.reserve(num_edges(g));
	for(Edge e : asRange(edges(g))) {
		Vertex v1 = source(e, g), v2 = target(e, g);
		if(get(boost::vertex_index_t(), g, v2) > get(boost::vertex_index_t(), g, v1)) std::swap(v1, v2);
		edgesSorted.emplace_back(v1, v2);
	}
	std::sort(begin(edgesSorted), end(edgesSorted));
	for(std::size_t i = 0; i < edgesSorted.size(); i++) {
		auto e = edgesSorted[i];
		// check loop
		if(e.first == e.second) {
			s << "Graph::sanityCheck:\tloop edge found on vertex "
					<< get(boost::vertex_index_t(), g, e.first) << "('" << pString[e.first] << "')" << std::endl;
			return false;
		}
		// check parallelness
		if(i > 0) {
			auto ep = edgesSorted[i - 1];
			if(e == ep) {
				s << "Graph::sanityCheck:\tparallel edges found between "
						<< get(boost::vertex_index_t(), g, e.first) << "('" << pString[e.first]
						<< "') and " << get(boost::vertex_index_t(), g, e.second) << " ('" << pString[e.second] << "')" << std::endl;
				return false;
			}
		}
	}
	// check connectedness
	std::vector<GraphType::vertices_size_type> component(num_vertices(g));
	GraphType::vertices_size_type numComponents = boost::connected_components(g, &component[0]);
	if(numComponents > 1) {
		s << "Graph::sanityCheck:\tthe graph contains " << numComponents << " > 1 connected components." << std::endl;
		return false;
	}
	return true;
}

} // namespace 

Single::Single(std::unique_ptr<GraphType> g, std::unique_ptr<PropStringType> pString)
: g(std::move(g), std::move(pString)),
id(nextGraphNum++), name(getGraphName(id)) {
	if(!sanityCheck(getGraph(), getStringState(), IO::log())) {
		IO::log() << "Graph::sanityCheck\tfailed in graph '" << getName() << "'" << std::endl;
		MOD_ABORT;
	}
	singles.insert(this);
}

Single::~Single() { }

const LabelledGraph &Single::getLabelledGraph() const {
	return g;
}

std::size_t Single::getId() const {
	return id;
}

std::shared_ptr<mod::Graph> Single::getAPIReference() const {
	if(apiReference.use_count() > 0) return std::shared_ptr<mod::Graph>(apiReference);
	else std::abort();
}

void Single::setAPIReference(std::shared_ptr<mod::Graph> g) {
	assert(apiReference.use_count() == 0);
	apiReference = g;
	assert(&g->getGraph() == this);
}

void Single::printName(std::ostream& s) const {
	s << getName();
}

const std::string &Single::getName() const {
	return name;
}

void Single::setName(std::string name) {
	this->name = name;
}

const std::pair<const std::string&, bool> Single::getGraphDFS() const {
	if(!dfs) std::tie(dfs, dfsHasNonSmilesRingClosure) = DFSEncoding::write(getGraph(), getStringState());
	return std::pair<const std::string&, bool>(*dfs, dfsHasNonSmilesRingClosure);
}

const std::string &Single::getSmiles() const {
	if(getMoleculeState().getIsMolecule()) {
		if(!smiles) smiles.reset(Chem::getSmiles(getGraph(), getMoleculeState()));
		return *smiles;
	} else {
		std::string text;
		text += "Graph " + boost::lexical_cast<std::string>(getId()) + " with name '" + getName() + "' is not a molecule.\n";
		text += "Can not generate SMILES string. GraphDFS is\n\t" + getGraphDFS().first + "\n";
		throw FatalError(std::move(text));
	}
}

std::shared_ptr<mod::Rule> Single::getBindRule() const {
	if(!bindRule) bindRule = Rule::Real::createSide(getGraph(), getStringState(), Rule::Membership::Right, getName());
	return bindRule;
}

std::shared_ptr<mod::Rule> Single::getIdRule() const {
	if(!idRule) idRule = Rule::Real::createSide(getGraph(), getStringState(), Rule::Membership::Context, getName());
	return idRule;
}

std::shared_ptr<mod::Rule> Single::getUnbindRule() const {
	if(!unbindRule) unbindRule = Rule::Real::createSide(getGraph(), getStringState(), Rule::Membership::Left, getName());
	return unbindRule;
}

unsigned int Single::getVertexLabelCount(const std::string &label) const {
	unsigned int count = 0;

	for(Vertex v : asRange(vertices(getGraph()))) {
		const std::string &vLabel = getStringState()[v];
		if(vLabel == label) count++;
	}
	return count;
}

unsigned int Single::getEdgeLabelCount(const std::string &label) const {
	unsigned int count = 0;

	for(Edge e : asRange(edges(getGraph()))) {
		const std::string &eLabel = getStringState()[e];
		if(eLabel == label) count++;
	}
	return count;
}

const PropMolecule &Single::getMoleculeState() const {
	if(!moleculeState) moleculeState.reset(new PropMolecule(get_graph(g), get_string(g)));
	return *moleculeState;
}

DepictionData &Single::getDepictionData() {
	if(!depictionData) depictionData.reset(new DepictionData(getGraph(), getStringState(), getMoleculeState()));
	return *depictionData;
}

const DepictionData &Single::getDepictionData() const {
	if(!depictionData) depictionData.reset(new DepictionData(getGraph(), getStringState(), getMoleculeState()));
	return *depictionData;
}

// Labelled Graph Interface
//------------------------------------------------------------------------------

const GraphType &Single::getGraph() const {
	return get_graph(g);
}

const PropStringType &Single::getStringState() const {
	return get_string(g);
}

//------------------------------------------------------------------------------
// Static
//------------------------------------------------------------------------------

namespace {
namespace GM = jla_boost::GraphMorphism;

template<typename Finder>
std::size_t morphism(const Single &gDomain, const Single &gCodomain, std::size_t maxNumMatches, Finder finder) {
	auto mr = GM::makeLimit(maxNumMatches);
	lib::GraphMorphism::morphismSelectByLabelSettings(gDomain.getLabelledGraph(), gCodomain.getLabelledGraph(), finder, std::ref(mr));
	return mr.getNumHits();
}

} // namespace

std::size_t Single::isomorphismVF2(const Single &gDom, const Single &gCodom, std::size_t maxNumMatches) {
	return morphism(gDom, gCodom, maxNumMatches, GM::VF2Isomorphism());
}

bool Single::isomorphismBrokenSmilesAndVF2(const Single &gDom, const Single &gCodom) {
	const auto &g1 = gDom.getGraph();
	const auto &g2 = gCodom.getGraph();
	if(num_vertices(g1) != num_vertices(g2)) return false;
	if(num_edges(g1) != num_edges(g2)) return false;
	if(
			gDom.getMoleculeState().getIsMolecule() && gCodom.getMoleculeState().getIsMolecule()) {
		return gDom.getSmiles() == gCodom.getSmiles();
	}
	return 1 == isomorphismVF2(gDom, gCodom, 1);
}

std::size_t Single::isomorphism(const Single &gDom, const Single &gCodom, std::size_t maxNumMatches) {
	// this hax with name comparing is basically to make abstract derivation graphs
	// TODO: remove it, so the name truly doesn't matter
	if(num_vertices(gDom.getGraph()) == 0 && num_vertices(gCodom.getGraph()) == 0)
		return gDom.getName() == gCodom.getName() ? 1 : 0;
	// we only have VF2 for doing multiple morphisms
	if(maxNumMatches > 1)
		return isomorphismVF2(gDom, gCodom, maxNumMatches);
	// so now it's just the decision question
	if(&gDom == &gCodom)
		return 1;
	return isomorphismBrokenSmilesAndVF2(gDom, gCodom) ? 1 : 0;
}

std::size_t Single::monomorphism(const Single &gDom, const Single &gCodom, std::size_t maxNumMatches) {
	return morphism(gDom, gCodom, maxNumMatches, GM::VF2Monomorphism());
}

bool Single::nameLess(const Single *g1, const Single *g2) {
	if(g1->getName().size() != g2->getName().size())
		return g1->getName().size() < g2->getName().size();
	else
		return g1->getName() < g2->getName();
}

Single makePermutation(const Single &g) {
	std::unique_ptr<PropStringType> pString;
	auto gBoost = lib::makePermutedGraph(g.getGraph(),
			[&pString](GraphType & gNew) {
				pString.reset(new PropStringType(gNew));
			},
	[&g, &pString](Vertex vOld, const GraphType &gOld, Vertex vNew, GraphType & gNew) {
		pString->addVertex(vNew, g.getStringState()[vOld]);
	},
	[&g, &pString](Edge eOld, const GraphType &gOld, Edge eNew, GraphType & gNew) {
		pString->addEdge(eNew, g.getStringState()[eOld]);
	}
	);
	Single gPerm(std::move(gBoost), std::move(pString));
	bool iso = 1 == Single::isomorphismVF2(g, gPerm, 1);
	if(!iso) {
		IO::Graph::Write::Options graphLike, molLike;
		graphLike.EdgesAsBonds(true).RaiseCharges(true).CollapseHydrogens(true).WithIndex(true);
		molLike.CollapseHydrogens(true).EdgesAsBonds(true).RaiseCharges(true).SimpleCarbons(true).WithColour(true).WithIndex(true);
		IO::Graph::Write::summary(g, graphLike, molLike);
		IO::Graph::Write::summary(gPerm, graphLike, molLike);
		IO::Graph::Write::gml(g, false);
		IO::Graph::Write::gml(gPerm, false);
		IO::log() << "g:     " << g.getSmiles() << std::endl;
		IO::log() << "gPerm: " << gPerm.getSmiles() << std::endl;
		MOD_ABORT;
	}
	return gPerm;
}

//------------------------------------------------------------------------------
// Other
//------------------------------------------------------------------------------

bool Less::operator()(const Single *g1, const Single *g2) const {
	return g1->getId() < g2->getId();
}

} // namespace Graph
} // namespace lib
} // namespace mod
