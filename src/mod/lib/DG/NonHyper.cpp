#include "NonHyper.h"

#include <mod/Config.h>
#include <mod/DG.h>
#include <mod/DGGraphInterface.h>
#include <mod/Error.h>
#include <mod/Graph.h>
#include <mod/Rule.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/DG/Hyper.h>
#include <mod/lib/Graph/Base.h>
#include <mod/lib/Graph/Merge.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/Graph/Properties/Molecule.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/IO/DG.h>
#include <mod/lib/IO/IO.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

#include <unordered_set>

namespace mod {
namespace lib {
namespace DG {
namespace {
std::size_t nextDGNum = 0;
}// namespace 

NonHyper::NonHyper(const std::vector<std::shared_ptr<mod::Graph> > &graphDatabase)
: id(nextDGNum++), hyperCreator(nullptr), hasCalculated(false),
productNum(0) {
	for(std::shared_ptr<mod::Graph> g : graphDatabase) this->graphDatabase.insert(g);
}

NonHyper::~NonHyper() {
	for(const lib::Graph::Merge *g : mergedGraphs) delete g;
}

std::size_t NonHyper::getId() const {
	return id;
}

std::shared_ptr<mod::DG> NonHyper::getAPIReference() const {
	if(apiReference.use_count() > 0) return std::shared_ptr<mod::DG>(apiReference);
	else std::abort();
}

void NonHyper::setAPIReference(std::shared_ptr<mod::DG> dg) {
	assert(apiReference.use_count() == 0);
	apiReference = dg;
	assert(&dg->getNonHyper() == this);
}

void NonHyper::calculate() {
	if(getHasCalculated()) MOD_ABORT;
	auto p = Hyper::makeHyper(*this);
	assert(!hyperCreator);
	hyper = std::move(p.first);
	hyperCreator = &p.second;
	calculateImpl();
	// annotate the graph with reversible pairs
	findReversiblePairs();
	hyperCreator = nullptr;
	hasCalculated = true;
	// make a nicer hyper graph
	//	p.first = std::move(hyper);
	//	hyper.reset(new Hyper(*this, 0));
	//	HyperCreator(std::move(p.second)); // just to lock it before compare, remove with it
	//	Hyper::temp_compare(*p.first, *hyper);
}

bool NonHyper::getHasCalculated() const {
	return hasCalculated;
}

bool NonHyper::addGraph(std::shared_ptr<mod::Graph> g) {
	if(getHasCalculated()) std::abort();
	return graphDatabase.insert(g).second;
}

bool NonHyper::addGraphAsVertex(std::shared_ptr<mod::Graph> g) {
	if(getHasCalculated()) std::abort();
	bool inserted = addGraph(g);
	getVertex(&g->getGraph());
	return inserted;
}

std::pair<std::shared_ptr<mod::Graph>, bool> NonHyper::checkIfNew(std::unique_ptr<lib::Graph::GraphType> gBoost, std::unique_ptr<lib::Graph::PropString> pString) const {
	assert(gBoost);
	assert(pString);
	auto gCand = std::make_unique<lib::Graph::Single>(std::move(gBoost), std::move(pString));
	for(std::shared_ptr<mod::Graph> g : graphDatabase) {
		bool isEqual = 1 == lib::Graph::Single::isomorphism(*gCand, g->getGraph(), 1);
		if(isEqual) return std::make_pair(g, false);
	}
	if(gCand->getMoleculeState().getIsMolecule()) {
		// maybe the stupid SMKLES canon was used
		// TODO: remove this when SMILES canon has been fixed
		for(std::shared_ptr<mod::Graph> g : graphDatabase) {
			if(g->getGraph().getMoleculeState().getIsMolecule()) {
				bool realIsomorphic = 1 == lib::Graph::Single::isomorphismVF2(*gCand, g->getGraph(), 1);
				if(realIsomorphic) {
					lib::IO::log() << "DG isomorphism, special case" << std::endl;
					return std::make_pair(g, false);
				}
			}
		}
	}
	std::shared_ptr<mod::Graph> g = mod::Graph::makeGraph(std::move(gCand));
	return std::make_pair(g, true);
}

void NonHyper::giveProductStatus(std::shared_ptr<mod::Graph> g) {
	assert(std::find(begin(graphDatabase), end(graphDatabase), g) != end(graphDatabase));

	std::string name = "p_{";
	name += boost::lexical_cast<std::string>(getId());
	name += ",";
	name += boost::lexical_cast<std::string>(productNum++);
	name += "}";
	g->setName(name);

	if(productNum % getConfig().dg.printGraphProduction.get() == 0) {
		IO::log() << "DG(" << products.size() << " p)\tnew graph\t";
		IO::log() << g->getGraph().getName();
		if(g->getGraph().getMoleculeState().getIsMolecule()) IO::log() << "\t" << g->getGraph().getSmiles();
		IO::log() << std::endl;
	}

	assert(std::find(begin(products), end(products), g) == end(products));
	products.push_back(g);
}

bool NonHyper::addProduct(std::shared_ptr<mod::Graph> g) {
	assert(g);
	bool isNewGraph = addGraph(g);
	if(isNewGraph) giveProductStatus(g);
	return isNewGraph;
}

const lib::Graph::Merge *NonHyper::addToMergeStore(const lib::Graph::Merge *g) {
	assert(!g->getSingles().empty());
	std::pair < MergeStore::const_iterator, bool> p = mergedGraphs.insert(g);
	if(!p.second) {
		delete g;
	}
	return *p.first;
}

std::pair < NonHyper::Edge, bool> NonHyper::isDerivation(const lib::Graph::Base *left, const lib::Graph::Base *right, const lib::Rules::Real *r) const {
	std::map<const Graph::Base*, Vertex>::const_iterator iterLeft = graphToVertex.find(left);
	if(iterLeft == end(graphToVertex)) return std::make_pair(Edge(), false);
	std::map<const Graph::Base*, Vertex>::const_iterator iterRight = graphToVertex.find(right);
	if(iterRight == end(graphToVertex)) return std::make_pair(Edge(), false);
	return edge(iterLeft->second, iterRight->second, dg);
}

std::pair<NonHyper::Edge, bool> NonHyper::suggestDerivation(const lib::Graph::Base *gSrc, const lib::Graph::Base *gTar, const lib::Rules::Real* r) {
	// make vertices for to and from
	Vertex vSrc = getVertex(gSrc), vTar = getVertex(gTar);
	std::pair<Edge, bool> e = edge(vSrc, vTar, dg);
	if(!e.second) {
		e = add_edge(vSrc, vTar, dg);
		if(r) dg[e.first].rules.push_back(r);
		hyperCreator->addEdge(e.first);
	} else {
		e.second = false;
		if(r) {
			auto &rules = dg[e.first].rules;
			auto iter = std::find(rules.begin(), rules.end(), r);
			if(iter == rules.end()) {
				rules.push_back(r);
				// TODO: hyperCreator->addRuleToEdge(e.first)
			}
		}
	}
	return e;
}

const NonHyper::GraphType &NonHyper::getGraphDuringCalculation() const {
	return dg;
}

NonHyper::Vertex NonHyper::getVertex(const lib::Graph::Base *g) {
	std::map<const Graph::Base*, Vertex>::iterator iter = graphToVertex.find(g);
	if(iter == graphToVertex.end()) {
		assert(hyperCreator);
		Vertex v = add_vertex(dg);
		dg[v].graph = g;
		graphToVertex[g] = v;
		for(auto *gSub : g->getSingles())
			hyperCreator->addVertex(gSub);
		return v;
	} else return iter->second;
}

void NonHyper::findReversiblePairs() {
	for(Edge e : asRange(edges(dg))) {
		Vertex vSource = source(e, dg);
		Vertex vTarget = target(e, dg);
		for(Edge eBack : asRange(out_edges(vTarget, dg))) {
			if(target(eBack, dg) == vSource) {
				dg[e].reverse = eBack;
				hyperCreator->setReverse(e, eBack);
			}
		}
	}
}

void NonHyper::list(std::ostream &s) const {
	listImpl(s);
}

const NonHyper::GraphType &NonHyper::getGraph() const {
	if(!getHasCalculated()) MOD_ABORT;
	return dg;
}

Hyper &NonHyper::getHyper() {
	if(!getHasCalculated()) MOD_ABORT;
	assert(hyper);
	return *hyper;
}

const Hyper &NonHyper::getHyper() const {
	if(!hyper) MOD_ABORT;
	return *hyper;
}

const NonHyper::StdGraphSet &NonHyper::getGraphDatabase() const {
	return graphDatabase;
}

const std::vector<std::shared_ptr<mod::Graph> > &NonHyper::getProducts() const {
	return products;
}

void NonHyper::print() const {
	if(!getHasCalculated()) std::abort();
	std::string fileNoExt = IO::DG::Write::pdfNonHyper(*this);
	IO::post() << "summaryDGNonHyper \"dg_" << getId() << "\" \"" << fileNoExt << "\"" << std::endl;
}

mod::DerivationRef NonHyper::getDerivationRef(const std::vector<Hyper::Vertex> &sources, const std::vector<Hyper::Vertex> &targets) const {
	const auto &dg = getHyper().getGraph();
	if(!getHasCalculated()) std::abort();
	for(auto v : sources) assert(dg[v].kind == HyperVertexKind::Vertex);
	for(auto v : targets) assert(dg[v].kind == HyperVertexKind::Vertex);

	const lib::Graph::Base *educt, *product;
	if(sources.size() == 1) educt = dg[sources.front()].graph;
	else {
		lib::Graph::Merge eductMerge;
		for(auto v : sources) eductMerge.mergeWith(*dg[v].graph);
		eductMerge.lock();
		MergeStore::const_iterator iter = mergedGraphs.find(&eductMerge);
		if(iter == end(mergedGraphs)) {
			return DerivationRef();
		}
		educt = *iter;
	}
	if(targets.size() == 1) product = dg[targets.front()].graph;
	else {
		Graph::Merge productMerge;
		for(auto v : targets) productMerge.mergeWith(*dg[v].graph);
		productMerge.lock();
		MergeStore::const_iterator iter = mergedGraphs.find(&productMerge);
		if(iter == end(mergedGraphs)) {
			return DerivationRef();
		}
		product = *iter;
	}
	std::map<const Graph::Base*, Vertex>::const_iterator iterEduct, iterProduct;
	iterEduct = graphToVertex.find(educt);
	assert(iterEduct != end(graphToVertex));
	iterProduct = graphToVertex.find(product);
	assert(iterProduct != end(graphToVertex));
	Vertex vSrc = iterEduct->second, vTar = iterProduct->second;
	std::pair<Edge, bool> p = edge(vSrc, vTar, getGraph());
	if(!p.second) {
		return DerivationRef();
	}
	const HyperGraphType &dgHyper = getHyper().getGraph();

	for(HyperVertex v : asRange(vertices(dgHyper))) {
		if(dgHyper[v].kind != HyperVertexKind::Edge) continue;
		if(dgHyper[v].inVertex == vSrc && dgHyper[v].outVertex == vTar)
			return getHyper().getDerivationRef(v);
	}
	// the edge must be there by now
	MOD_ABORT;
}

std::vector<mod::DerivationRef> NonHyper::getAllDerivationRefs() const {
	std::vector<mod::DerivationRef> result;
	const HyperGraphType &dgHyper = getHyper().getGraph();

	for(HyperVertex v : asRange(vertices(dgHyper))) {
		if(dgHyper[v].kind != HyperVertexKind::Edge) continue;
		result.push_back(getHyper().getDerivationRef(v));
		assert(getHyper().getDerivationRef(v).isValid());
	}
	return result;
}

void NonHyper::diff(const NonHyper &dg1, const NonHyper &dg2) {
	std::ostream &s = IO::log();
	bool headerPrinted = false;
	auto printHeader = [&headerPrinted, &dg1, &dg2, &s] () -> std::ostream& {
		if(headerPrinted) return s;
		else headerPrinted = true;
		s << "--- DG " << dg1.getId() << std::endl;
		s << "+++ DG " << dg2.getId() << std::endl;
		return s;
	};
	// diff vertices
	std::unordered_set<std::shared_ptr<mod::Graph> > vertexGraphsUnique;
	for(auto vertex : dg1.getAPIReference()->vertices()) vertexGraphsUnique.insert(vertex.getGraph());
	for(auto vertex : dg2.getAPIReference()->vertices()) vertexGraphsUnique.insert(vertex.getGraph());
	std::vector<std::shared_ptr<mod::Graph> > vertexGraphs(begin(vertexGraphsUnique), end(vertexGraphsUnique));
	std::sort(begin(vertexGraphs), end(vertexGraphs), [] (std::shared_ptr<mod::Graph> g1, std::shared_ptr<mod::Graph> g2) {
		return g1->getName() < g2->getName();
	});
	for(auto g : vertexGraphs) {
		bool in1 = dg1.getHyper().isVertexGraph(g);
		bool in2 = dg2.getHyper().isVertexGraph(g);
		assert(in1 || in2);
		if(!in1) printHeader() << "-";
		if(!in2) printHeader() << "+";
		if(!in1 || !in2) s << *g << std::endl;
	}
	// diff edges
	std::vector<Derivation> ders1, ders2;
	auto order = [] (Derivation d) -> Derivation {
		std::sort(begin(d.left), end(d.left), GraphLess());
		std::sort(begin(d.right), end(d.right), GraphLess());
		return d;
	};
	for(auto dRef : dg1.getAllDerivationRefs()) ders1.push_back(order(*dRef));
	for(auto dRef : dg2.getAllDerivationRefs()) ders2.push_back(order(*dRef));
	auto derLess = [] (const Derivation &d1, const Derivation & d2) {
		if(std::lexicographical_compare(begin(d1.left), end(d1.left), begin(d2.left), end(d2.left), GraphLess())) return true;
		else if(std::lexicographical_compare(begin(d2.left), end(d2.left), begin(d1.left), end(d1.left), GraphLess())) return false;
		if(std::lexicographical_compare(begin(d1.right), end(d1.right), begin(d2.right), end(d2.right), GraphLess())) return true;
		else if(std::lexicographical_compare(begin(d2.right), end(d2.right), begin(d1.right), end(d1.right), GraphLess())) return false;
		return d1.rule->getId() < d2.rule->getId();
	};
	std::sort(begin(ders1), end(ders1), derLess);
	std::sort(begin(ders2), end(ders2), derLess);
	auto iter1 = begin(ders1), iter2 = begin(ders2);
	while(true) {
		if(iter1 == end(ders1)) {
			for(; iter2 != end(ders2); iter2++) printHeader() << "+" << *iter2 << std::endl;
			break;
		}
		if(iter2 == end(ders2)) {
			for(; iter1 != end(ders1); iter1++) printHeader() << "-" << *iter1 << std::endl;
			break;
		}
		if(derLess(*iter1, *iter2)) {
			printHeader() << "-" << *iter1 << std::endl;
			iter1++;
		} else if(derLess(*iter2, *iter1)) {
			printHeader() << "+" << *iter2 << std::endl;
			iter2++;
		} else {
			iter1++;
			iter2++;
		}
	}
}

} // namespace DG
} // namespace lib
} // namespace mod
