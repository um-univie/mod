#include "Single.hpp"

#include <mod/Misc.hpp>
#include <mod/graph/Graph.hpp>
#include <mod/lib/Chem/MoleculeUtil.hpp>
#include <mod/lib/Chem/Smiles.hpp>
#include <mod/lib/Graph/Canonicalisation.hpp>
#include <mod/lib/Graph/DFSEncoding.hpp>
#include <mod/lib/Graph/Properties/Depiction.hpp>
#include <mod/lib/Graph/Properties/Molecule.hpp>
#include <mod/lib/Graph/Properties/Stereo.hpp>
#include <mod/lib/Graph/Properties/String.hpp>
#include <mod/lib/Graph/Properties/Term.hpp>
#include <mod/lib/GraphMorphism/LabelledMorphism.hpp>
#include <mod/lib/GraphMorphism/VF2Finder.hpp>
#include <mod/lib/IO/IO.hpp>
#include <mod/lib/IO/Graph.hpp>
#include <mod/lib/LabelledGraph.hpp>
#include <mod/lib/Random.hpp>
#include <mod/lib/Term/WAM.hpp>

#include <jla_boost/graph/morphism/callbacks/Limit.hpp>

#include <boost/graph/connected_components.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/lexical_cast.hpp>

namespace mod::lib::Graph {
BOOST_CONCEPT_ASSERT((LabelledGraphConcept<LabelledGraph>));

namespace {
std::size_t nextGraphNum = 0;

const std::string getGraphName(unsigned int id) {
	return "g_{" + boost::lexical_cast<std::string>(id) + "}";
}

bool sanityCheck(const GraphType &g, const PropString &pString, std::ostream &s) {
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
				  << "') and " << get(boost::vertex_index_t(), g, e.second) << " ('" << pString[e.second] << "')"
				  << std::endl;
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

Single::Single(std::unique_ptr<GraphType> g, std::unique_ptr<PropString> pString, std::unique_ptr<PropStereo> pStereo)
		: g(std::move(g), std::move(pString), std::move(pStereo)),
		  id(nextGraphNum++), name(getGraphName(id)) {
	if(!sanityCheck(getGraph(), getStringState(), std::cout)) {
		std::cout << "Graph::sanityCheck\tfailed in graph '" << getName() << "'" << std::endl;
		MOD_ABORT;
	}
}

Single::~Single() {}

const LabelledGraph &Single::getLabelledGraph() const {
	return g;
}

std::size_t Single::getId() const {
	return id;
}

std::shared_ptr<graph::Graph> Single::getAPIReference() const {
	if(apiReference.use_count() > 0) return std::shared_ptr<graph::Graph>(apiReference);
	else std::abort();
}

void Single::setAPIReference(std::shared_ptr<graph::Graph> g) {
	assert(apiReference.use_count() == 0);
	apiReference = g;
	assert(&g->getGraph() == this);
}

const std::string &Single::getName() const {
	return name;
}

void Single::setName(std::string name) {
	this->name = name;
}

const std::pair<const std::string &, bool> Single::getGraphDFS() const {
	if(!dfs) std::tie(dfs, dfsHasNonSmilesRingClosure) = DFSEncoding::write(getGraph(), getStringState(), false);
	return std::pair<const std::string &, bool>(*dfs, dfsHasNonSmilesRingClosure);
}

const std::string &Single::getGraphDFSWithIds() const {
	if(!dfsWithIds) dfsWithIds = DFSEncoding::write(getGraph(), getStringState(), true).first;
	return *dfsWithIds;
}

const std::string &Single::getSmiles() const {
	if(getMoleculeState().getIsMolecule()) {
		if(!smiles) {
			if(getConfig().graph.useWrongSmilesCanonAlg.get()) {
				smiles = Chem::getSmiles(getGraph(), getMoleculeState(), nullptr, false);
			} else {
				getCanonForm(LabelType::String, false); // TODO: make the withStereo a parameter
				smiles = Chem::getSmiles(getGraph(), getMoleculeState(), &canon_perm_string, false);
			}
		}
		return *smiles;
	} else {
		std::string text;
		text += "Graph " + boost::lexical_cast<std::string>(getId()) + " with name '" + getName() +
		        "' is not a molecule.\n";
		text += "Can not generate SMILES string. GraphDFS is\n\t" + getGraphDFS().first + "\n";
		throw LogicError(std::move(text));
	}
}

const std::string &Single::getSmilesWithIds() const {
	if(getMoleculeState().getIsMolecule()) {
		if(!smilesWithIds) {
			if(getConfig().graph.useWrongSmilesCanonAlg.get()) {
				smilesWithIds = Chem::getSmiles(getGraph(), getMoleculeState(), nullptr, true);
			} else {
				getCanonForm(LabelType::String, false); // TODO: make the withStereo a parameter
				smilesWithIds = Chem::getSmiles(getGraph(), getMoleculeState(), &canon_perm_string, true);
			}
		}
		return *smilesWithIds;
	} else {
		std::string text;
		text += "Graph " + boost::lexical_cast<std::string>(getId()) + " with name '" + getName() +
		        "' is not a molecule.\n";
		text += "Can not generate SMILES string. GraphDFS is\n\t" + getGraphDFS().first + "\n";
		throw LogicError(std::move(text));
	}
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

DepictionData &Single::getDepictionData() {
	if(!depictionData) depictionData.reset(new DepictionData(getLabelledGraph()));
	return *depictionData;
}

const DepictionData &Single::getDepictionData() const {
	if(!depictionData) depictionData.reset(new DepictionData(getLabelledGraph()));
	return *depictionData;
}

// Labelled Graph Interface
//------------------------------------------------------------------------------

const GraphType &Single::getGraph() const {
	return get_graph(g);
}

const PropString &Single::getStringState() const {
	return get_string(g);
}

const PropMolecule &Single::getMoleculeState() const {
	return get_molecule(g);
}

const Single::CanonForm &Single::getCanonForm(LabelType labelType, bool withStereo) const {
	if(labelType != LabelType::String)
		throw LogicError("Can only canonicalise with label type string.");
	// TODO: when Terms are supported, remember to check if the state is valid, else throw TermParsingError
	if(withStereo)
		throw LogicError("Can not canonicalise stereo.");
	if(!canon_form_string) {
		assert(!aut_group_string);
		std::tie(canon_perm_string, canon_form_string, aut_group_string) = lib::Graph::getCanonForm(*this, labelType,
		                                                                                            withStereo);
	}
	assert(canon_form_string);
	assert(aut_group_string);
	return *canon_form_string;
}

const Single::AutGroup &Single::getAutGroup(LabelType labelType, bool withStereo) const {
	getCanonForm(labelType, withStereo);
	assert(aut_group_string);
	return *aut_group_string;
}

//------------------------------------------------------------------------------
// Static
//------------------------------------------------------------------------------

namespace {
namespace GM = jla_boost::GraphMorphism;
namespace GM_MOD = lib::GraphMorphism;

template<typename Finder>
std::size_t
morphism(const Single &gDomain, const Single &gCodomain, std::size_t maxNumMatches, LabelSettings labelSettings,
         Finder finder) {
	auto mr = GM::makeLimit(maxNumMatches);
	lib::GraphMorphism::morphismSelectByLabelSettings(gDomain.getLabelledGraph(), gCodomain.getLabelledGraph(),
	                                                  labelSettings, finder, std::ref(mr));
	return mr.getNumHits();
}

std::size_t isomorphismSmilesOrCanonOrVF2(const Single &gDom, const Single &gCodom, LabelSettings labelSettings) {
	const auto &ggDom = gDom.getLabelledGraph();
	const auto &ggCodom = gCodom.getLabelledGraph();
	// first try if we can compare canonical SMILES strings
	if(get_molecule(ggDom).getIsMolecule() && get_molecule(ggCodom).getIsMolecule() &&
	   !getConfig().graph.useWrongSmilesCanonAlg.get())
		return gDom.getSmiles() == gCodom.getSmiles() ? 1 : 0;

	// otherwise maybe we can still do canonical form comparison
	if(labelSettings.type == LabelType::String && !labelSettings.withStereo) {
		return canonicalCompare(gDom, gCodom, labelSettings.type, labelSettings.withStereo) ? 1 : 0;
	}

	// otherwise, we have no choice but to use VF2
	return Single::isomorphismVF2(gDom, gCodom, 1, labelSettings);
}

} // namespace

std::size_t Single::isomorphismVF2(const Single &gDom, const Single &gCodom, std::size_t maxNumMatches,
                                   LabelSettings labelSettings) {
	return morphism(gDom, gCodom, maxNumMatches, labelSettings, GM_MOD::VF2Isomorphism());
}

bool Single::isomorphic(const Single &gDom, const Single &gCodom, LabelSettings labelSettings) {
	++getConfig().graph.numIsomorphismCalls();
	const auto nDom = num_vertices(gDom.getGraph());
	const auto nCodom = num_vertices(gCodom.getGraph());
	if(nDom != nCodom) return false; // early bail-out
	// this hax with name comparing is basically to make abstract derivation graphs
	// TODO: remove it, so the name truly doesn't matter
	if(nDom == 0)
		return gDom.getName() == gCodom.getName();
	if(&gDom == &gCodom) return true;
	switch(getConfig().graph.isomorphismAlg.get()) {
	case Config::IsomorphismAlg::SmilesCanonVF2:
		return isomorphismSmilesOrCanonOrVF2(gDom, gCodom, labelSettings);
	case Config::IsomorphismAlg::VF2:
		return isomorphismVF2(gDom, gCodom, 1, labelSettings);
	case Config::IsomorphismAlg::Canon:
		if(labelSettings.relation != LabelRelation::Isomorphism)
			throw LogicError("Can only do isomorphism via canonicalisation with the isomorphism relation.");
		if(labelSettings.withStereo && labelSettings.stereoRelation != LabelRelation::Isomorphism)
			throw LogicError("Can only do isomorphism via canonicalisation with the isomorphism stereo relation.");
		return canonicalCompare(gDom, gCodom, labelSettings.type, labelSettings.withStereo);
	}
	MOD_ABORT;
}

std::size_t
Single::isomorphism(const Single &gDom, const Single &gCodom, std::size_t maxNumMatches, LabelSettings labelSettings) {
	++getConfig().graph.numIsomorphismCalls();
	if(maxNumMatches == 1)
		return isomorphic(gDom, gCodom, labelSettings) ? 1 : 0;
	// this hax with name comparing is basically to make abstract derivation graphs
	// TODO: remove it, so the name truly doesn't matter
	const auto nDom = num_vertices(gDom.getGraph());
	const auto nCodom = num_vertices(gCodom.getGraph());
	if(nDom == 0 && nCodom == 0)
		return gDom.getName() == gCodom.getName() ? 1 : 0;
	// we only have VF2 for doing multiple morphisms
	return isomorphismVF2(gDom, gCodom, maxNumMatches, labelSettings);
}

std::size_t
Single::monomorphism(const Single &gDom, const Single &gCodom, std::size_t maxNumMatches, LabelSettings labelSettings) {
	return morphism(gDom, gCodom, maxNumMatches, labelSettings, GM_MOD::VF2Monomorphism());
}

bool Single::nameLess(const Single *g1, const Single *g2) {
	if(g1->getName().size() != g2->getName().size())
		return g1->getName().size() < g2->getName().size();
	else
		return g1->getName() < g2->getName();
}

bool Single::canonicalCompare(const Single &g1, const Single &g2, LabelType labelType, bool withStereo) {
	return lib::Graph::canonicalCompare(g1, g2, labelType, withStereo);
}

Single makePermutation(const Single &g) {
	if(has_stereo(g.getLabelledGraph()))
		throw mod::FatalError("Can not (yet) permute graphs with stereo information.");
	std::unique_ptr<PropString> pString;
	auto gBoost = lib::makePermutedGraph(g.getGraph(),
	                                     [&pString](GraphType &gNew) {
		                                     pString.reset(new PropString(gNew));
	                                     },
	                                     [&g, &pString](Vertex vOld, const GraphType &gOld, Vertex vNew,
	                                                    GraphType &gNew) {
		                                     pString->addVertex(vNew, g.getStringState()[vOld]);
	                                     },
	                                     [&g, &pString](Edge eOld, const GraphType &gOld, Edge eNew, GraphType &gNew) {
		                                     pString->addEdge(eNew, g.getStringState()[eOld]);
	                                     }
	);
	Single gPerm(std::move(gBoost), std::move(pString), nullptr);
	if(getConfig().graph.checkIsoInPermutation.get()) {
		const bool iso = 1 == Single::isomorphismVF2(g, gPerm, 1,
		                                             {LabelType::String, LabelRelation::Isomorphism, false,
		                                              LabelRelation::Isomorphism});
		if(!iso) {
			IO::Graph::Write::Options graphLike, molLike;
			graphLike.EdgesAsBonds(true).RaiseCharges(true).CollapseHydrogens(true).WithIndex(true);
			molLike.CollapseHydrogens(true).EdgesAsBonds(true).RaiseCharges(true).SimpleCarbons(true).WithColour(
					true).WithIndex(true);
			IO::Graph::Write::summary(g, graphLike, molLike);
			IO::Graph::Write::summary(gPerm, graphLike, molLike);
			IO::Graph::Write::gml(g, false);
			IO::Graph::Write::gml(gPerm, false);
			std::cout << "g:     " << g.getSmiles() << std::endl;
			std::cout << "gPerm: " << gPerm.getSmiles() << std::endl;
			MOD_ABORT;
		}
	}
	return gPerm;
}

} // namespace mod::lib::Graph