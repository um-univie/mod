#include "NonHyper.h"

// for debugging
#include <mod/Misc.h>
#include <mod/graph/GraphInterface.h>
// end for debugging

#include <mod/Config.h>
#include <mod/Error.h>
#include <mod/dg/DG.h>
#include <mod/dg/GraphInterface.h>
#include <mod/graph/Graph.h>
#include <mod/rule/Rule.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/DG/Hyper.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/Graph/Properties/Molecule.h>
#include <mod/lib/Graph/Properties/Stereo.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/Graph/Properties/Term.h>
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

NonHyper::NonHyper(const std::vector<std::shared_ptr<graph::Graph> > &graphDatabase, LabelSettings labelSettings)
: id(nextDGNum++), labelSettings(labelSettings), hyperCreator(nullptr), hasCalculated(false),
productNum(0) {
	if(getConfig().dg.skipInitialGraphIsomorphismCheck.get()) {
		for(std::shared_ptr<graph::Graph> gCand : graphDatabase) this->graphDatabase.insert(gCand);
	} else {
		const auto ls = LabelSettings{this->labelSettings.type, LabelRelation::Isomorphism, this->labelSettings.withStereo, LabelRelation::Isomorphism};
		for(std::shared_ptr<graph::Graph> gCand : graphDatabase) {
			for(const auto &g : this->graphDatabase) {
				if(g == gCand) break;
				const bool equal = gCand->isomorphism(g, 1, ls);
				if(equal) {
					std::string msg = "Isomorphic graphs '" + g->getName() + "' and '" + gCand->getName() + "' in initial graph database.";
					throw LogicError(std::move(msg));
				}
			}
			if(ls.type == LabelType::Term) {
				const auto &term = get_term(gCand->getGraph().getLabelledGraph());
				if(!isValid(term)) {
					std::string msg = "Parsing failed for graph '" + gCand->getName() + "' in graph database. " + term.getParsingError();
					throw TermParsingError(std::move(msg));
				}
			}
			this->graphDatabase.insert(gCand);
		}
	}
}

NonHyper::~NonHyper() { }

std::size_t NonHyper::getId() const {
	return id;
}

std::shared_ptr<dg::DG> NonHyper::getAPIReference() const {
	if(apiReference.use_count() > 0) return std::shared_ptr<dg::DG>(apiReference);
	else std::abort();
}

void NonHyper::setAPIReference(std::shared_ptr<dg::DG> dg) {
	assert(apiReference.use_count() == 0);
	apiReference = dg;
	assert(&dg->getNonHyper() == this);
}

LabelSettings NonHyper::getLabelSettings() const {
	return labelSettings;
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

bool NonHyper::addGraph(std::shared_ptr<graph::Graph> g) {
	if(getHasCalculated()) std::abort();
	return graphDatabase.insert(g).second;
}

bool NonHyper::addGraphAsVertex(std::shared_ptr<graph::Graph> g) {
	if(getHasCalculated()) std::abort();
	bool inserted = addGraph(g);
	getVertex(GraphMultiset(&g->getGraph()));
	return inserted;
}

std::pair<std::shared_ptr<graph::Graph>, bool> NonHyper::checkIfNew(std::unique_ptr<lib::Graph::Single> gCand) const {
	assert(gCand);
	const auto labelSettings = LabelSettings{this->labelSettings.type, LabelRelation::Isomorphism, this->labelSettings.withStereo, LabelRelation::Isomorphism};
	for(std::shared_ptr<graph::Graph> g : graphDatabase) {
		bool isEqual = 1 == lib::Graph::Single::isomorphism(*gCand, g->getGraph(), 1, labelSettings);
		if(isEqual) {
			//			if(getConfig().dg.calculateDetailsVerbose.get()) {
			//				IO::log() << "Discarding product " << gCand->getName() << ", isomorphic to " << g->getName() << std::endl;
			//				IO::log() << "\tLabelSettings: withStereo=" << std::boolalpha << this->labelSettings.withStereo << std::endl;
			//				mod::postSection("Discarded");
			//				mod::lib::IO::Graph::Write::Options opts, optsGraph;
			//				opts.edgesAsBonds = opts.withIndex = true;
			//				optsGraph.collapseHydrogens = optsGraph.edgesAsBonds = optsGraph.raiseCharges = true;
			//				optsGraph.simpleCarbons = optsGraph.withColour = optsGraph.withIndex = true;
			//				mod::lib::IO::Graph::Write::summary(*gCand, optsGraph, optsGraph);
			//				std::shared_ptr<graph::Graph> gCandWrapped = graph::Graph::makeGraph(std::move(gCand));
			//				for(auto v : gCandWrapped->vertices()) {
			//					if(v.getStringLabel() == "C")
			//						v.printStereo();
			//				}
			//			}
			return std::make_pair(g, false);
		}
	}
	std::shared_ptr<graph::Graph> g = graph::Graph::makeGraph(std::move(gCand));
	return std::make_pair(g, true);
}

void NonHyper::giveProductStatus(std::shared_ptr<graph::Graph> g) {
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

bool NonHyper::addProduct(std::shared_ptr<graph::Graph> g) {
	assert(g);
	bool isNewGraph = addGraph(g);
	if(isNewGraph) giveProductStatus(g);
	return isNewGraph;
}

std::pair < NonHyper::Edge, bool> NonHyper::isDerivation(const GraphMultiset &gmsSrc, const GraphMultiset &gmsTar, const lib::Rules::Real *r) const {
	const auto iterLeft = multisetToVertex.find(gmsSrc);
	if(iterLeft == end(multisetToVertex)) return std::make_pair(Edge(), false);
	const auto iterRight = multisetToVertex.find(gmsTar);
	if(iterRight == end(multisetToVertex)) return std::make_pair(Edge(), false);
	return edge(iterLeft->second, iterRight->second, dg);
}

std::pair<NonHyper::Edge, bool> NonHyper::suggestDerivation(const GraphMultiset &gmsSrc, const GraphMultiset &gmsTar, const lib::Rules::Real *r) {
	// make vertices for to and from
	Vertex vSrc = getVertex(gmsSrc), vTar = getVertex(gmsTar);
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
				hyperCreator->addRuleToEdge(e.first, r);
			}
		}
	}
	return e;
}

const NonHyper::GraphType &NonHyper::getGraphDuringCalculation() const {
	return dg;
}

NonHyper::Vertex NonHyper::getVertex(const GraphMultiset &gms) {
	const auto iter = multisetToVertex.find(gms);
	if(iter != multisetToVertex.end()) return iter->second;
	assert(hyperCreator);
	Vertex v = add_vertex(dg);
	dg[v].graphs = gms;
	multisetToVertex[gms] = v;
	for(auto *gSub : gms) hyperCreator->addVertex(gSub);
	return v;
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

const std::vector<std::shared_ptr<graph::Graph> > &NonHyper::getProducts() const {
	return products;
}

void NonHyper::print() const {
	if(!getHasCalculated()) std::abort();
	std::string fileNoExt = IO::DG::Write::pdfNonHyper(*this);
	IO::post() << "summaryDGNonHyper \"dg_" << getId() << "\" \"" << fileNoExt << "\"" << std::endl;
}

HyperVertex NonHyper::findHyperEdge(const std::vector<Hyper::Vertex> &sources, const std::vector<Hyper::Vertex> &targets) const {
	const auto &dg = getHyper().getGraph();
	if(!getHasCalculated()) std::abort();
	for(auto v : sources) assert(dg[v].kind == HyperVertexKind::Vertex);
	for(auto v : targets) assert(dg[v].kind == HyperVertexKind::Vertex);

	std::vector<const lib::Graph::Single*> srcGraphs, tarGraphs;
	srcGraphs.reserve(sources.size());
	tarGraphs.reserve(targets.size());
	for(const auto v : sources) srcGraphs.push_back(dg[v].graph);
	for(const auto v : targets) tarGraphs.push_back(dg[v].graph);
	GraphMultiset gmsSrc(std::move(srcGraphs)), gmsTar(std::move(tarGraphs));
	const auto iterSrc = multisetToVertex.find(gmsSrc);
	if(iterSrc == end(multisetToVertex)) return boost::graph_traits<HyperGraphType>::null_vertex();
	const auto iterTar = multisetToVertex.find(gmsTar);
	if(iterTar == end(multisetToVertex)) return boost::graph_traits<HyperGraphType>::null_vertex();
	const auto vSrc = iterSrc->second, vTar = iterTar->second;

	const std::pair<Edge, bool> p = edge(vSrc, vTar, getGraph());
	if(!p.second) return boost::graph_traits<HyperGraphType>::null_vertex();
	const HyperGraphType &dgHyper = getHyper().getGraph();
	for(const auto v : asRange(vertices(dgHyper))) {
		if(dgHyper[v].kind != HyperVertexKind::Edge) continue;
		if(dgHyper[v].inVertex == vSrc && dgHyper[v].outVertex == vTar) {
			return v;
		}
	}
	// the edge must be there by now
	MOD_ABORT;
}

std::vector<dg::DG::HyperEdge> NonHyper::getAllHyperEdges() const {
	std::vector<dg::DG::HyperEdge> result;
	const HyperGraphType &dgHyper = getHyper().getGraph();

	for(HyperVertex v : asRange(vertices(dgHyper))) {
		if(dgHyper[v].kind != HyperVertexKind::Edge) continue;
		result.push_back(getHyper().getInterfaceEdge(v));
		assert(!result.back().isNull());
	}
	return result;
}

void NonHyper::diff(const NonHyper &dg1, const NonHyper &dg2) {
	std::ostream &s = IO::log();
	bool headerPrinted = false;
	const auto printHeader = [&headerPrinted, &dg1, &dg2, &s] () -> std::ostream& {
		if(headerPrinted) return s;
		else headerPrinted = true;
		s << "--- DG " << dg1.getId() << std::endl;
		s << "+++ DG " << dg2.getId() << std::endl;
		return s;
	};
	// diff vertices
	std::unordered_set<std::shared_ptr<graph::Graph> > vertexGraphsUnique;
	for(const auto v : dg1.getAPIReference()->vertices()) vertexGraphsUnique.insert(v.getGraph());
	for(const auto v : dg2.getAPIReference()->vertices()) vertexGraphsUnique.insert(v.getGraph());
	std::vector<std::shared_ptr<graph::Graph> > vertexGraphs(begin(vertexGraphsUnique), end(vertexGraphsUnique));
	std::sort(begin(vertexGraphs), end(vertexGraphs), [] (std::shared_ptr<graph::Graph> g1, std::shared_ptr<graph::Graph> g2) {
		return g1->getName() < g2->getName();
	});
	for(const auto g : vertexGraphs) {
		const bool in1 = dg1.getHyper().isVertexGraph(&g->getGraph());
		const bool in2 = dg2.getHyper().isVertexGraph(&g->getGraph());
		assert(in1 || in2);
		if(!in1) printHeader() << "-";
		if(!in2) printHeader() << "+";
		if(!in1 || !in2) s << *g << std::endl;
	}
	// diff edges
	const auto makeDers = [](const auto &dg) {
		const auto order = [](Derivation d) -> Derivation {
			std::sort(begin(d.left), end(d.left), graph::GraphLess());
			std::sort(begin(d.right), end(d.right), graph::GraphLess());
			return d;
		};
		std::vector<Derivation> ders;
		for(const auto &e : dg.edges()) {
			for(const auto &r : e.rules()) {
				Derivation d;
				for(const auto &v : e.sources())
					d.left.push_back(v.getGraph());
				d.r = r;
				for(const auto &v : e.targets())
					d.right.push_back(v.getGraph());
				ders.push_back(order(std::move(d)));
			}
		}
		return ders;
	};
	std::vector<Derivation>
			ders1 = makeDers(*dg1.getAPIReference()),
			ders2 = makeDers(*dg2.getAPIReference());
	const auto derLess = [](const Derivation &d1, const Derivation & d2) {
		if(std::lexicographical_compare(begin(d1.left), end(d1.left), begin(d2.left), end(d2.left), graph::GraphLess())) return true;
		else if(std::lexicographical_compare(begin(d2.left), end(d2.left), begin(d1.left), end(d1.left), graph::GraphLess())) return false;
		if(std::lexicographical_compare(begin(d1.right), end(d1.right), begin(d2.right), end(d2.right), graph::GraphLess())) return true;
		else if(std::lexicographical_compare(begin(d2.right), end(d2.right), begin(d1.right), end(d1.right), graph::GraphLess())) return false;
		return d1.r->getId() < d2.r->getId();
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
