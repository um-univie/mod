#include "Hyper.h"

#include <mod/DG.h>
#include <mod/Config.h>
#include <mod/Error.h>
#include <mod/Graph.h>
#include <mod/lib/DG/Matrix.h>
#include <mod/lib/DG/NonHyper.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/IO/DG.h>
#include <mod/lib/IO/Graph.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/Rule/Base.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>
#include <jla_boost/Memory.hpp>

#include <boost/graph/adjacency_list.hpp>

namespace mod {
namespace lib {
namespace DG {

//------------------------------------------------------------------------------
// HyperCreator
//------------------------------------------------------------------------------

struct HyperCreator::Pimpl {
	std::map<NonHyper::Edge, Hyper::Vertex> edgeToHyper;
};

HyperCreator::HyperCreator(Hyper &hyper) : owner(hyper), valid(true), pimpl(new Pimpl()) { }

HyperCreator::HyperCreator(HyperCreator &&other)
: owner(other.owner), valid(other.valid), pimpl(std::move(other.pimpl)) {
	other.valid = false;
}

HyperCreator::~HyperCreator() {
	if(valid) owner.hasCalculated = true;
}

void HyperCreator::addVertex(const lib::Graph::Single *g) {
	owner.addVertex(g);
}

void HyperCreator::addEdge(NonHyper::Edge eNon) {
	const auto &dg = owner.nonHyper.getGraphDuringCalculation();
	auto &hyper = owner.hyper;
	NonHyper::Vertex vSrcNon = source(eNon, dg), vTarNon = target(eNon, dg);
	Hyper::Vertex v = add_vertex(hyper);
	hyper[v].kind = HyperVertexKind::Edge;
	hyper[v].graph = nullptr;
	hyper[v].rules = dg[eNon].rules;
	assert(dg[eNon].rules.size() == 1); // TODO: do we actually need do something special here?
	hyper[v].inVertex = vSrcNon;
	hyper[v].outVertex = vTarNon;
	hyper[v].reverse = hyper.null_vertex();
	pimpl->edgeToHyper[eNon] = v;

	{ // source edges
		for(const lib::Graph::Single *g : dg[vSrcNon].graph->getSingles()) {
			Hyper::Vertex vSrc = owner.getVertexFromGraph(g);
			add_edge(vSrc, v, hyper);
		}
	}

	{ // target edges
		for(const lib::Graph::Single *g : dg[vTarNon].graph->getSingles()) {
			Hyper::Vertex vTar = owner.getVertexFromGraph(g);
			add_edge(v, vTar, hyper);
		}
	}
}

void HyperCreator::setReverse(NonHyper::Edge e, NonHyper::Edge eBack) {
	const auto &dg = owner.nonHyper.getGraphDuringCalculation();
	auto &hyper = owner.hyper;
	assert(dg[e].reverse);
	const auto &edgeToHyper = pimpl->edgeToHyper;
	auto eIter = edgeToHyper.find(e);
	auto eBackIter = edgeToHyper.find(eBack);
	assert(eIter != end(edgeToHyper));
	assert(eBackIter != end(edgeToHyper));
	hyper[eIter->second].reverse = eBackIter->second;
}

//------------------------------------------------------------------------------
// Hyper
//------------------------------------------------------------------------------

Hyper::Hyper(const NonHyper &dg)
: hasCalculated(false), nonHyper(dg), matrixSparse(nullptr) { }

std::pair<std::unique_ptr<Hyper>, HyperCreator> Hyper::makeHyper(const NonHyper &dg) {
	auto hyper = std::unique_ptr<Hyper>(new Hyper(dg));
	auto &hyperRef = *hyper;
	return std::make_pair(std::move(hyper), HyperCreator(hyperRef));
}

//Hyper::Hyper(const NonHyper &dgClass, int dummy)
//: hasCalculated(true), nonHyper(dgClass), vpGraph(nullptr), matrixSparse(nullptr) {
//	const NonHyperGraphType &dg = dgClass.getGraph();
//
//	// first go through each vertex in dg, and create the needed vertices in dgHyper representing graphs
//
//	for(Vertex v : asRange(vertices(dg))) {
//		const lib::Graph::Base *g = dg[v].graph;
//		assert(g);
//		for(const lib::Graph::Single *gSingle : g->getSingles()) addVertex(gSingle);
//	}
//
//	// then go through all edge and add vertices for them, and edges
//	std::map<NonHyperEdge, Vertex> edgeToHyper;
//
//	for(NonHyperEdge e : asRange(edges(dg))) {
//		const lib::Rule::Base *r = dg[e].rule;
//		Vertex from = source(e, dg);
//		const lib::Graph::Base *gFrom = dg[from].graph;
//		Vertex to = target(e, dg);
//		const lib::Graph::Base *gTo = dg[to].graph;
//		// it's a hyper edge, so first add the vertex implemening the edge
//		Hyper::Vertex v = add_vertex(hyper);
//		hyper[v].kind = VertexKind::Edge;
//		hyper[v].graph = nullptr;
//		hyper[v].rule = r;
//		hyper[v].reverse = hyper.null_vertex();
//		hyper[v].inVertex = from;
//		hyper[v].outVertex = to;
//		// insert to map
//		edgeToHyper[e] = v;
//		// fix reverse
//		boost::optional<NonHyperEdge> reverse = dg[e].reverse;
//		if(reverse) {
//			auto iter = edgeToHyper.find(reverse.get());
//			if(iter != end(edgeToHyper)) {
//				// if both edge have been added, then fix it
//				Vertex ourReverse = iter->second;
//				hyper[v].reverse = ourReverse;
//				hyper[ourReverse].reverse = v;
//			}
//		}
//
//		// add in edges to the rule
//		{ // from
//			for(const lib::Graph::Single *g : gFrom->getSingles()) {
//				// get the vertex descriptor representing this graph
//				Hyper::Vertex vFrom = getVertexFromGraph(g);
//				add_edge(vFrom, v, hyper);
//			}
//		}
//
//		// add out edges from the rule
//		{ // to
//			for(const lib::Graph::Single *g : gTo->getSingles()) {
//				// get the vertex descriptor representing this graph
//				Hyper::Vertex vTo = getVertexFromGraph(g);
//				add_edge(v, vTo, hyper);
//			}
//		}
//	}
//}

void Hyper::addVertex(const lib::Graph::Single *g) {
	{ // sanity check
#ifndef NDEBUG
		std::shared_ptr<mod::Graph> gAPI = g->getAPIReference();
		NonHyper::StdGraphSet::const_iterator iter = nonHyper.getGraphDatabase().find(gAPI);
		assert(iter != nonHyper.getGraphDatabase().end());
#endif
	}
	std::map<const lib::Graph::Single*, Vertex>::iterator idIter = graphToHyperVertex.find(g);
	if(idIter == graphToHyperVertex.end()) { // create the vertex
		Vertex vNew = add_vertex(hyper);
		hyper[vNew].kind = VertexKind::Vertex;
		hyper[vNew].graph = g;
		graphToHyperVertex[g] = vNew;
		vertexGraphs.push_back(g->getAPIReference());
	}
}

Hyper::~Hyper() {
	delete matrixSparse;
}

const NonHyper &Hyper::getNonHyper() const {
	return nonHyper;
}

const Hyper::GraphType &Hyper::getGraph() const {
	return hyper;
}

const Matrix &Hyper::getMatrixSparse() const {
	if(!hasCalculated) std::abort();
	if(!matrixSparse) matrixSparse = new Matrix(*this);
	return *matrixSparse;
}

void Hyper::printStats(std::ostream &s) const {
	// statistics stuff
	unsigned int numVerts = 0, numIn = 0, numOut = 0, numReverse = 0;
	unsigned int numEdges = 0, numEdgePairs = 0;
	std::map<unsigned int, unsigned int> countNumIn, countNumOut;
	double avgInReverseRatio = 0, avgInReverseRatioWithVirtual = 0;
	double avgOutReverseRatio = 0, avgOutReverseRatioWithVirtual = 0;
	unsigned int projectedNumTransit = 0;

	unsigned int numUniqueIn = 0, numUniqueOut = 0;
	unsigned int projectedNumTransitTotal = 0;
	unsigned int numUniqueInOnlyReverse = 0, numUniqueOutOnlyReverse = 0;
	unsigned int projectedNumTransitCollapseNonReverse = 0;

	for(Vertex v : asRange(vertices(hyper))) {
		if(hyper[v].kind != VertexKind::Vertex) {
			numEdges++;
			if(hyper[v].reverse != hyper.null_vertex()) numEdgePairs++;
			continue;
		}
		numVerts++;
		numIn += in_degree(v, hyper);
		countNumIn[in_degree(v, hyper)]++;
		numOut += out_degree(v, hyper);
		countNumOut[out_degree(v, hyper)]++;
		for(Edge e : asRange(out_edges(v, hyper))) if(hyper[target(e, hyper)].reverse != hyper.null_vertex()) numReverse++;
		unsigned int numInReverse = 0;
		for(Edge e : asRange(in_edges(v, hyper))) if(hyper[source(e, hyper)].reverse != hyper.null_vertex()) numInReverse++;
		unsigned int numOutReverse = 0;
		for(Edge e : asRange(out_edges(v, hyper))) if(hyper[target(e, hyper)].reverse != hyper.null_vertex()) numOutReverse++;
		if(in_degree(v, hyper) > 0) {
			avgInReverseRatio += numInReverse / (double) in_degree(v, hyper);
		}
		avgInReverseRatioWithVirtual += (numInReverse + 1) / (double) (in_degree(v, hyper) + 1);
		if(out_degree(v, hyper)) {
			avgOutReverseRatio += numOutReverse / (double) out_degree(v, hyper);
		}
		avgOutReverseRatioWithVirtual += (numOutReverse + 1) / (double) (out_degree(v, hyper) + 1);
		if(in_degree(v, hyper) >= 10 || out_degree(v, hyper) >= 10) {
			s << "DG\t|in| = " << in_degree(v, hyper) << ", |out| = " << out_degree(v, hyper)
					<< "\t" << hyper[v].graph->getName() << std::endl;
			s << "\t" << numInReverse << " inReverse, " << numOutReverse << " outReverse" << std::endl;
		}
		projectedNumTransit += (numInReverse + 2)*(numOutReverse + 2);

		auto invAdjIters = asRange(inv_adjacent_vertices(v, hyper));
		std::unordered_set<Vertex> uniqueIn(begin(invAdjIters), end(invAdjIters));
		auto adjIters = asRange(adjacent_vertices(v, hyper));
		std::unordered_set<Vertex> uniqueOut(begin(adjIters), end(adjIters));

		unsigned int localNumUniqueInOnlyReverse = 1; // +1 from the virtual input/output edges
		unsigned int localNumUniqueOutOnlyReverse = 1;
		for(Vertex eIn : asRange(inv_adjacent_vertices(v, hyper))) {
			if(hyper[eIn].reverse != hyper.null_vertex()) localNumUniqueInOnlyReverse++;
		}
		for(Vertex eOut : asRange(adjacent_vertices(v, hyper))) {
			if(hyper[eOut].reverse != hyper.null_vertex()) localNumUniqueOutOnlyReverse++;
		}
		assert(localNumUniqueInOnlyReverse == localNumUniqueOutOnlyReverse); // right?
		numUniqueInOnlyReverse += localNumUniqueInOnlyReverse;
		numUniqueOutOnlyReverse += localNumUniqueOutOnlyReverse;

		// we only add these now because we implicitly had them in local-only-reverse-calculation
		uniqueIn.insert(hyper.null_vertex());
		uniqueOut.insert(hyper.null_vertex());

		numUniqueIn += uniqueIn.size();
		numUniqueOut += uniqueOut.size();
		projectedNumTransitTotal +=
				uniqueIn.size() * uniqueOut.size()
				- localNumUniqueInOnlyReverse; // we subtract these because they would be zero-constrained anyway

		// we add 1 because the catch all would also make a sub-vertex
		projectedNumTransitCollapseNonReverse +=
				(localNumUniqueInOnlyReverse + 1) * (localNumUniqueOutOnlyReverse + 1)
				- localNumUniqueInOnlyReverse; // we subtract these because they would be zero-constrained anyway
	}
	numEdgePairs /= 2;
	avgInReverseRatio /= numVerts;
	avgInReverseRatioWithVirtual /= numVerts;
	avgOutReverseRatio /= numVerts;
	avgOutReverseRatioWithVirtual /= numVerts;

	s << "Stat--------------------------------------------------------------" << std::endl;
	s << "numVerts:\t" << numVerts << std::endl << "numIn:\t\t" << numIn << std::endl
			<< "numOut:\t\t" << numOut << std::endl << "numReverse:\t" << numReverse << std::endl
			<< "numEdges:\t" << numEdges << std::endl << "numEdgePairs:\t" << numEdgePairs << std::endl;
	s << "out/vert:\t" << (numOut / ((double) numVerts)) << std::endl;
	s << "in/vert:\t" << (numIn / ((double) numVerts)) << std::endl;
	s << "reverse/vert:\t" << (numReverse / ((double) numVerts)) << std::endl;
	s << "pairs/edge:\t" << (numEdgePairs / ((double) numEdges)) << std::endl;
	s << "------------------------------------------------------------------" << std::endl;
	typedef std::pair<unsigned int, unsigned int> P;
	s << "numIn histogram:" << std::endl;
	for(const P &p : countNumIn) s << p.first << "\t" << p.second << std::endl;
	s << "------------------------------------------------------------------" << std::endl;
	s << "numOut histogram:" << std::endl;
	for(const P &p : countNumIn) s << p.first << "\t" << p.second << std::endl;
	s << "------------------------------------------------------------------" << std::endl;
	s << "avgInReverseRatio\t" << avgInReverseRatio << std::endl;
	s << "avgInReverseRatioWithVirtual\t" << avgInReverseRatioWithVirtual << std::endl;
	s << "avgOutReverseRatio\t" << avgOutReverseRatio << std::endl;
	s << "avgOutReverseRatioWithVirtual\t" << avgOutReverseRatioWithVirtual << std::endl;
	s << "------------------------------------------------------------------" << std::endl;
	s << "projected numTransit:\t" << projectedNumTransit << std::endl;
	s << "------------------------------------------------------------------" << std::endl;
	s << "------------------------------------------------------------------" << std::endl;
	s << "numUniqueIn:              " << numUniqueIn << std::endl;
	s << "numUniqueOut:             " << numUniqueOut << std::endl;
	s << "projectedNumTransitTotal: " << projectedNumTransitTotal << std::endl;
	s << "------------------------------------------------------------------" << std::endl;
	s << "numUniqueInOnlyReverse:                " << numUniqueInOnlyReverse << std::endl;
	s << "numUniqueOutOnlyReverse:               " << numUniqueOutOnlyReverse << std::endl;
	s << "projectedNumTransitCollapseNonReverse: " << projectedNumTransitCollapseNonReverse << std::endl;
	s << "------------------------------------------------------------------" << std::endl;
}

void Hyper::printIncidence(std::ostream &s, const std::shared_ptr<mod::Graph> g) const {
	if(!isVertexGraph(g)) {
		s << "The graph '" << g->getName() << "' is not part of the derivation graph." << std::endl;
		return;
	}
	Vertex v = getVertexFromGraph(&g->getGraph());
	{// first the in transformations
		s << "In:" << std::endl;

		for(Edge eIn : asRange(in_edges(v, hyper))) {
			Vertex vRule = source(eIn, hyper);
			NonHyperVertex vIn = hyper[vRule].inVertex;
			NonHyperVertex vOut = hyper[vRule].outVertex;

			assert(hyper[vRule].kind == VertexKind::Edge);

			int ruleId = get(boost::vertex_index_t(), hyper, vRule);
			const lib::Graph::Base *gIn = nonHyper.getGraph()[vIn].graph;
			const lib::Graph::Base *gOut = nonHyper.getGraph()[vOut].graph;
			s << ruleId << ":\t";
			gIn->printName(s);
			s << "\t->{";
			for(auto *r : hyper[vRule].rules) s << " " << r->getName();
			s << " } (" << get(boost::vertex_index_t(), hyper, hyper[vRule].reverse) << ")" << "\t";
			gOut->printName(s);
			s << std::endl;
		}
	}
	{ // and the out transformations
		s << "Out:" << std::endl;

		for(Edge eOut : asRange(out_edges(v, hyper))) {
			Vertex vRule = target(eOut, hyper);
			NonHyperVertex vIn = hyper[vRule].inVertex;
			NonHyperVertex vOut = hyper[vRule].outVertex;

			assert(hyper[vRule].kind == VertexKind::Edge);

			int ruleId = get(boost::vertex_index_t(), hyper, vRule);
			const Graph::Base *gIn = nonHyper.getGraph()[vIn].graph;
			const Graph::Base *gOut = nonHyper.getGraph()[vOut].graph;
			s << ruleId << ":\t";
			gIn->printName(s);
			s << "\t->{";
			for(auto *r : hyper[vRule].rules) s << " " << r->getName();
			s << " } (" << get(boost::vertex_index_t(), hyper, hyper[vRule].reverse) << ")" << "\t";
			gOut->printName(s);
			s << std::endl;
		}
	}
	s << "Notice: end of incident hyper edges." << std::endl;
}

Hyper::Vertex Hyper::getVertexFromDerivationRef(mod::DerivationRef dRef) const {
	assert(dRef.isValid());
	auto vIterPair = vertices(getGraph());
	auto vIter = vIterPair.first;
	assert(vIterPair.second - vIterPair.first > dRef.getId());
	auto v = *(vIter + dRef.getId());
	assert(hyper[v].kind == VertexKind::Edge);
	return v;
}

Hyper::Vertex Hyper::getVertexFromGraph(const lib::Graph::Single *g) const {
	std::map<const lib::Graph::Single*, Hyper::Vertex>::const_iterator iter = graphToHyperVertex.find(g);
	assert(iter != graphToHyperVertex.end());
	return iter->second;
}

bool Hyper::isVertexGraph(std::shared_ptr<mod::Graph> g) const {
	std::map<const Graph::Single*, Vertex>::const_iterator iter = graphToHyperVertex.find(&g->getGraph());
	return iter != graphToHyperVertex.end();
}

const std::vector<std::shared_ptr<mod::Graph> > &Hyper::getVertexGraphs() const {
	return vertexGraphs;
}

mod::DerivationRef Hyper::getDerivationRef(Vertex v) const {
	if(v == hyper.null_vertex()) return DerivationRef();
	if(hyper[v].kind != VertexKind::Edge) return DerivationRef();
	return DerivationRef(get(boost::vertex_index_t(), hyper, v), getNonHyper().getAPIReference());
}

mod::DerivationRef Hyper::getDerivationRefValid(Vertex v) const {
	auto dRef = getDerivationRef(v);
	if(dRef.isValid()) return dRef;
	else MOD_ABORT;
}

mod::Derivation Hyper::getDerivation(Vertex v) const {
	assert(v != hyper.null_vertex());
	assert(hyper[v].kind == VertexKind::Edge);
	Derivation d;
	for(Vertex vIn : asRange(inv_adjacent_vertices(v, hyper)))
		d.left.push_back(hyper[vIn].graph->getAPIReference());
	assert(hyper[v].rules.size() == 1);
	d.rule = hyper[v].rules.front()->getAPIReference();
	for(Vertex vOut : asRange(adjacent_vertices(v, hyper)))
		d.right.push_back(hyper[vOut].graph->getAPIReference());
	return d;
}

unsigned int Hyper::getFirstDerivation() const {
	for(Vertex v : asRange(vertices(getGraph()))) {
		if(getGraph()[v].kind == VertexKind::Edge) return get(boost::vertex_index_t(), getGraph(), v);
	}
	return num_vertices(getGraph());
}

unsigned int Hyper::getEndDerivation() const {
	return num_vertices(getGraph());
}

unsigned int Hyper::getNextDerivation(unsigned int id) const {
	auto vIterPair = vertices(getGraph());
	auto vIter = vIterPair.first;
	assert(vIterPair.second - vIterPair.first > id);
	for(vIter += id + 1; vIter != vIterPair.second; vIter++) {
		Vertex v = *vIter;
		if(getGraph()[v].kind == VertexKind::Edge) return get(boost::vertex_index_t(), getGraph(), v);
	}
	return num_vertices(getGraph());
}

mod::DerivationRef Hyper::dereferenceDerivation(unsigned int id) const {
	auto vIterPair = vertices(getGraph());
	auto vIter = vIterPair.first;
	assert(vIterPair.second - vIterPair.first > id);
	Vertex v = *(vIter + id);
	return getDerivationRef(v);
}

unsigned int Hyper::getFirstInDerivation(Vertex v) const {
	assert(getGraph()[v].kind == VertexKind::Vertex);
	return 0;
}

unsigned int Hyper::getEndInDerivation(Vertex v) const {
	assert(getGraph()[v].kind == VertexKind::Vertex);
	return in_degree(v, getGraph());
}

unsigned int Hyper::getNextInDerivation(unsigned int id, Vertex v) const {
	assert(getGraph()[v].kind == VertexKind::Vertex);
	assert(id < in_degree(v, getGraph()));
	return id + 1;
}

mod::DerivationRef Hyper::dereferenceInDerivation(unsigned int id, Vertex v) const {
	assert(getGraph()[v].kind == VertexKind::Vertex);
	auto vIterPair = inv_adjacent_vertices(v, getGraph());
	auto vIter = vIterPair.first;
	assert(vIterPair.second - vIterPair.first > id);
	Vertex vAdj = *(vIter + id);
	return getDerivationRef(vAdj);
}

unsigned int Hyper::getFirstOutDerivation(Vertex v) const {
	assert(getGraph()[v].kind == VertexKind::Vertex);
	return 0;
}

unsigned int Hyper::getEndOutDerivation(Vertex v) const {
	assert(getGraph()[v].kind == VertexKind::Vertex);
	return out_degree(v, getGraph());
}

unsigned int Hyper::getNextOutDerivation(unsigned int id, Vertex v) const {
	assert(getGraph()[v].kind == VertexKind::Vertex);
	assert(id < out_degree(v, getGraph()));
	return id + 1;
}

mod::DerivationRef Hyper::dereferenceOutDerivation(unsigned int id, Vertex v) const {
	assert(getGraph()[v].kind == VertexKind::Vertex);
	auto vIterPair = adjacent_vertices(v, getGraph());
	auto vIter = vIterPair.first;
	assert(vIterPair.second - vIterPair.first > id);
	Vertex vAdj = *(vIter + id);
	return getDerivationRef(vAdj);
}

void Hyper::temp_compare(const Hyper &a, const Hyper &b) {
	const auto &dgHyperA = a.getGraph();
	const auto &dgHyperB = b.getGraph();
	if(num_vertices(dgHyperA) != num_vertices(dgHyperB)) {
		MOD_ABORT;
	}
	std::vector<Vertex> verticesA(vertices(dgHyperA).first, vertices(dgHyperA).second);
	std::vector<Vertex> verticesB(vertices(dgHyperB).first, vertices(dgHyperB).second);
	std::stable_partition(begin(verticesA), end(verticesA), [&dgHyperA](Vertex v) {
		return dgHyperA[v].kind == VertexKind::Vertex;
	});
	std::stable_partition(begin(verticesB), end(verticesB), [&dgHyperB](Vertex v) {
		return dgHyperB[v].kind == VertexKind::Vertex;
	});
	std::map<Vertex, size_t> vMapA, vMapB;
	for(std::size_t i = 0; i < verticesA.size(); i++) vMapA[verticesA[i]] = i;
	for(std::size_t i = 0; i < verticesB.size(); i++) vMapB[verticesB[i]] = i;
	auto vIterA = begin(verticesA);
	auto vIterB = begin(verticesB);
	for(; vIterA != end(verticesA); vIterA++, vIterB++) {
		Vertex vA = *vIterA, vB = *vIterB;
		const auto &pA = dgHyperA[vA];
		const auto &pB = dgHyperB[vB];
		if(pA.kind != pB.kind) {
			MOD_ABORT;
		}
		if(pA.graph != pB.graph) {
			MOD_ABORT;
		}
		assert(pA.rules.size() == 1);
		assert(pB.rules.size() == 1);
		if(pA.rules.front() != pB.rules.front()) {
			MOD_ABORT;
		}
		if(pA.kind == VertexKind::Edge) {
			if(pA.inVertex != pB.inVertex) {
				MOD_ABORT;
			}
			if(pA.outVertex != pB.outVertex) {
				MOD_ABORT;
			}
			if(vMapA[pA.reverse] != vMapB[pB.reverse]) {
				IO::log() << pA.reverse << " -> " << vMapA[pA.reverse] << " != " << vMapB[pB.reverse] << " <- " << pB.reverse << std::endl;
				MOD_ABORT;
			}
		}
	}
	if(num_edges(dgHyperA) != num_edges(dgHyperB)) {
		MOD_ABORT;
	}
	auto eIterA = edges(dgHyperA);
	auto eIterB = edges(dgHyperB);
	for(; eIterA.first != eIterA.second; eIterA.first++, eIterB.first++) {
		Edge eA = *eIterA.first, eB = *eIterB.first;
		if(vMapA[source(eA, dgHyperA)] != vMapB[source(eB, dgHyperB)]) {
			MOD_ABORT;
		}
		if(vMapA[target(eA, dgHyperA)] != vMapB[target(eB, dgHyperB)]) {
			MOD_ABORT;
		}
	}
}

} // namespace DG
} // namespace lib
} // namespace mod
