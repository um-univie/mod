#include "GraphInterface.h"

#include <mod/graph/Printer.h>
#include <mod/lib/DG/Hyper.h>
#include <mod/lib/GraphPimpl.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/IO/Derivation.h>
#include <mod/lib/Rules/Real.h>

namespace mod {
namespace dg {

//------------------------------------------------------------------------------
// Vertex
//------------------------------------------------------------------------------

MOD_GRAPHPIMPL_Define_Vertex(DG, dg, g->getHyper().getGraph(), g, DG)
MOD_GRAPHPIMPL_Define_Vertex_Directed(DG, dg, g->getHyper().getGraph(), g)

DG::InEdgeRange DG::Vertex::inEdges() const {
	if(!g) throw LogicError("Can not get in-edges on a null vertex.");
	return InEdgeRange(g, vId);
}

DG::OutEdgeRange DG::Vertex::outEdges() const {
	if(!g) throw LogicError("Can not get in-edges on a null vertex.");
	return OutEdgeRange(g, vId);
}

std::shared_ptr<graph::Graph> DG::Vertex::getGraph() const {
	using boost::vertices;
	auto iter = vertices(g->getHyper().getGraph()).first + vId;
	const auto &dg = g->getHyper().getGraph();
	assert(dg[*iter].kind == lib::DG::HyperVertexKind::Vertex);
	return dg[*iter].graph->getAPIReference();
}


//------------------------------------------------------------------------------
// HyperEdge
//------------------------------------------------------------------------------

DG::HyperEdge::HyperEdge(std::shared_ptr<DG> g, std::size_t eId) : g(g), eId(eId) {
	assert(g);
	if(eId >= num_vertices(g->getHyper().getGraph())) {
		this->g = nullptr;
		this->eId = 0;
	} else {
		const auto &dg = g->getHyper().getGraph();
		using boost::vertices;
		const auto v = *(vertices(dg).first + eId);
		assert(dg[v].kind == lib::DG::HyperVertexKind::Edge);
		(void) dg;
		(void) v;
	}
}

DG::HyperEdge::HyperEdge() : eId(0) { }

std::ostream &operator<<(std::ostream &s, const DG::HyperEdge &e) {
	s << "HyperEdge(";
	if(e.isNull()) s << "null";
	else {
		s << *e.getDG() << ", {";
		for(auto src : e.sources())
			s << " " << src.getId();
		s << " }, {";
		for(auto tar : e.targets())
			s << " " << tar.getId();
		s << "}";
	}
	return s << ")";
}

bool operator==(const DG::HyperEdge &e1, const DG::HyperEdge &e2) {
	return e1.g == e2.g && e1.eId == e2.eId;
}

bool operator!=(const DG::HyperEdge &e1, const DG::HyperEdge &e2) {
	return !(e1 == e2);
}

bool operator<(const DG::HyperEdge &e1, const DG::HyperEdge &e2) {
	return std::tie(e1.g, e1.eId) < std::tie(e2.g, e2.eId);
}

bool DG::HyperEdge::isNull() const {
	return *this == HyperEdge();
}

std::size_t DG::HyperEdge::getId() const {
	if(isNull()) throw LogicError("Can not get id on a null hyperedge.");
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto v = *(vertices(dg).first + eId);
	return get(boost::vertex_index_t(), dg, v);
}

std::shared_ptr<DG> DG::HyperEdge::getDG() const {
	if(isNull()) throw LogicError("Can not get derivation graph on a null hyperedge.");
	return g;
}

std::size_t DG::HyperEdge::numSources() const {
	if(isNull()) throw LogicError("Can not get number of sources on a null edge.");
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto v = *(vertices(dg).first + eId);
	return in_degree(v, dg);
}

DG::SourceRange DG::HyperEdge::sources() const {
	if(isNull()) throw LogicError("Can not get sources on a null edge.");
	return SourceRange(g, eId);
}

std::size_t DG::HyperEdge::numTargets() const {
	if(isNull()) throw LogicError("Can not get number of targets on a null edge.");
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto v = *(vertices(dg).first + eId);
	return out_degree(v, dg);
}

DG::TargetRange DG::HyperEdge::targets() const {
	if(isNull()) throw LogicError("Can not get targets on a null edge.");
	return TargetRange(g, eId);
}

DG::RuleRange DG::HyperEdge::rules() const {
	if(isNull()) throw LogicError("Can not get rules on a null edge.");
	return RuleRange(g, eId);
}

DG::HyperEdge DG::HyperEdge::getInverse() const {
	if(isNull()) throw LogicError("Can not get inverse of null edge.");
	const auto &dgHyper = g->getHyper();
	const auto &dg = dgHyper.getGraph();
	using boost::vertices;
	auto v = *(vertices(dg).first + eId);
	assert(dg[v].kind == lib::DG::HyperVertexKind::Edge);
	auto vInverse = dg[v].reverse;
	if(vInverse == dg.null_vertex()) return HyperEdge();
	return HyperEdge(g, get(boost::vertex_index_t(), dg, vInverse));
}

void DG::HyperEdge::print(const graph::Printer &printer, const std::string &nomatchColour, const std::string &matchColour) const {
	if(isNull()) throw LogicError("Can not print null edge.");
	const auto &dgHyper = g->getHyper();
	const auto &dg = dgHyper.getGraph();
	using boost::vertices;
	auto v = *(vertices(dg).first + eId);
	assert(dg[v].kind == lib::DG::HyperVertexKind::Edge);
	lib::IO::Derivation::Write::summary(g->getNonHyper(), v, printer.getOptions(), nomatchColour, matchColour);
}

void DG::HyperEdge::printTransitionState() const {
	graph::Printer printer;
	printTransitionState(printer);
}

void DG::HyperEdge::printTransitionState(const graph::Printer &printer) const {
	if(isNull()) throw LogicError("Can not print null edge.");
	const auto &dgHyper = g->getHyper();
	const auto &dg = dgHyper.getGraph();
	using boost::vertices;
	auto v = *(vertices(dg).first + eId);
	assert(dg[v].kind == lib::DG::HyperVertexKind::Edge);
	assert(dg[v].rules.size() == 1);
	lib::IO::Derivation::Write::summaryTransitionState(g->getNonHyper(), v, printer.getOptions());
}

//------------------------------------------------------------------------------
// VertexIterator
//------------------------------------------------------------------------------

DG::VertexIterator::VertexIterator(std::shared_ptr<DG> g) : g(g), vId(0) {
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto vs = vertices(dg);
	for(auto iter = vs.first + vId; iter != vs.second; ++iter) {
		if(dg[*iter].kind == lib::DG::HyperVertexKind::Vertex) {
			vId = iter - vs.first;
			return;
		}
	}
	this->g = nullptr;
	vId = 0;
}

DG::VertexIterator::VertexIterator() : vId(0) { }

DG::Vertex DG::VertexIterator::dereference() const {
	using boost::vertices;
	assert(g);
	const auto &dg = g->getHyper();
	assert(num_vertices(dg.getGraph()) > 0);
	const auto v = *(vertices(dg.getGraph()).first + vId);
	assert(dg.getGraph()[v].kind == lib::DG::HyperVertexKind::Vertex);
	return dg.getInterfaceVertex(v);
}

bool DG::VertexIterator::equal(const VertexIterator &iter) const {
	if(g) return g == iter.g && vId == iter.vId;
	else return g == iter.g;
}

void DG::VertexIterator::increment() {
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto vs = vertices(dg);
	for(auto iter = vs.first + vId + 1; iter != vs.second; ++iter) {
		if(dg[*iter].kind == lib::DG::HyperVertexKind::Vertex) {
			vId = iter - vs.first;
			return;
		}
	}
	g = nullptr;
	vId = 0;
}

//------------------------------------------------------------------------------
// EdgeIterator
//------------------------------------------------------------------------------

DG::EdgeIterator::EdgeIterator(std::shared_ptr<DG> g) : g(g), eId(0) {
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto vs = vertices(dg);
	for(auto iter = vs.first + eId; iter != vs.second; ++iter) {
		if(dg[*iter].kind == lib::DG::HyperVertexKind::Edge) {
			eId = iter - vs.first;
			return;
		}
	}
	this->g = nullptr;
	eId = 0;
}

DG::EdgeIterator::EdgeIterator() : eId(0) { }

DG::HyperEdge DG::EdgeIterator::dereference() const {
	using boost::vertices;
	const auto &dg = g->getHyper();
	const auto v = *(vertices(dg.getGraph()).first + eId);
	assert(dg.getGraph()[v].kind == lib::DG::HyperVertexKind::Edge);
	return dg.getInterfaceEdge(v);
}

bool DG::EdgeIterator::equal(const EdgeIterator &iter) const {
	if(g) return g == iter.g && eId == iter.eId;
	else return g == iter.g;
}

void DG::EdgeIterator::increment() {
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto vs = vertices(dg);
	for(auto iter = vs.first + eId + 1; iter != vs.second; ++iter) {
		if(dg[*iter].kind == lib::DG::HyperVertexKind::Edge) {
			eId = iter - vs.first;
			return;
		}
	}
	g = nullptr;
	eId = 0;
}

//------------------------------------------------------------------------------
// InEdgeIterator
//------------------------------------------------------------------------------

DG::InEdgeIterator::InEdgeIterator(std::shared_ptr<DG> g, std::size_t vId) : g(g), vId(vId), eId(0) {
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto v = vertices(dg).first[vId];
	if(eId >= in_degree(v, dg)) {
		this->g = nullptr;
		this->vId = 0;
	}
}

DG::InEdgeIterator::InEdgeIterator() : vId(0), eId(0) { }

DG::HyperEdge DG::InEdgeIterator::dereference() const {
	const auto &dg = g->getHyper();
	using boost::vertices;
	const auto v = vertices(dg.getGraph()).first[vId];
	const auto e = inv_adjacent_vertices(v, dg.getGraph()).first[eId];
	return dg.getInterfaceEdge(e);
}

bool DG::InEdgeIterator::equal(const InEdgeIterator &iter) const {
	return g == iter.g && vId == iter.vId && eId == iter.eId;
}

void DG::InEdgeIterator::increment() {
	++eId;
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto v = vertices(dg).first[vId];
	if(eId >= in_degree(v, dg)) {
		g = nullptr;
		vId = 0;
		eId = 0;
	}
}

//------------------------------------------------------------------------------
// OutEdgeIterator
//------------------------------------------------------------------------------

DG::OutEdgeIterator::OutEdgeIterator(std::shared_ptr<DG> g, std::size_t vId) : g(g), vId(vId), eId(0) {
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto v = vertices(dg).first[vId];
	if(eId >= out_degree(v, dg)) {
		this->g = nullptr;
		this->vId = 0;
	}
}

DG::OutEdgeIterator::OutEdgeIterator() : vId(0), eId(0) { }

DG::HyperEdge DG::OutEdgeIterator::dereference() const {
	const auto &dg = g->getHyper();
	using boost::vertices;
	const auto v = vertices(dg.getGraph()).first[vId];
	const auto e = adjacent_vertices(v, dg.getGraph()).first[eId];
	return dg.getInterfaceEdge(e);
}

bool DG::OutEdgeIterator::equal(const OutEdgeIterator &iter) const {
	return g == iter.g && vId == iter.vId && eId == iter.eId;
}

void DG::OutEdgeIterator::increment() {
	++eId;
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto v = vertices(dg).first[vId];
	if(eId >= out_degree(v, dg)) {
		g = nullptr;
		vId = 0;
		eId = 0;
	}
}

//------------------------------------------------------------------------------
// SourceIterator
//------------------------------------------------------------------------------

DG::SourceIterator::SourceIterator(std::shared_ptr<DG> g, std::size_t eId) : g(g), eId(eId), vId(0) { }

DG::SourceIterator::SourceIterator() : eId(0), vId(0) { }

DG::Vertex DG::SourceIterator::dereference() const {
	using boost::vertices;
	const auto &dg = g->getHyper();
	const auto e = vertices(dg.getGraph()).first[eId];
	const auto v = inv_adjacent_vertices(e, dg.getGraph()).first[vId];
	return dg.getInterfaceVertex(v);
}

bool DG::SourceIterator::equal(const SourceIterator &iter) const {
	return g == iter.g && eId == iter.eId && vId == iter.vId;
}

void DG::SourceIterator::increment() {
	++vId;
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto e = vertices(dg).first[eId];
	if(vId >= in_degree(e, dg)) {
		g = nullptr;
		eId = 0;
		vId = 0;
	}
}

//------------------------------------------------------------------------------
// TargetIterator
//------------------------------------------------------------------------------

DG::TargetIterator::TargetIterator(std::shared_ptr<DG> g, std::size_t eId) : g(g), eId(eId), vId(0) { }

DG::TargetIterator::TargetIterator() : eId(0), vId(0) { }

DG::Vertex DG::TargetIterator::dereference() const {
	using boost::vertices;
	const auto &dg = g->getHyper();
	const auto e = vertices(dg.getGraph()).first[eId];
	const auto v = adjacent_vertices(e, dg.getGraph()).first[vId];
	return dg.getInterfaceVertex(v);
}

bool DG::TargetIterator::equal(const TargetIterator &iter) const {
	return g == iter.g && eId == iter.eId && vId == iter.vId;
}

void DG::TargetIterator::increment() {
	++vId;
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto e = vertices(dg).first[eId];
	if(vId >= out_degree(e, dg)) {
		g = nullptr;
		eId = 0;
		vId = 0;
	}
}

//------------------------------------------------------------------------------
// RuleIterator
//------------------------------------------------------------------------------

DG::RuleIterator::RuleIterator(std::shared_ptr<DG> g, std::size_t eId) : g(g), eId(eId), i(0) {
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	assert(g);
	assert(num_vertices(g->getHyper().getGraph()) > 0);
	assert(g->getHyper().getGraph()[*(vertices(g->getHyper().getGraph()).first + eId)].kind == lib::DG::HyperVertexKind::Edge);
	auto e = vertices(dg).first[eId];
	const auto &rs = dg[e].rules;
	if(rs.empty()) {
		this->eId = 0;
		this->g = nullptr;
	}
}

DG::RuleIterator::RuleIterator() : eId(0), i(0) { }

std::shared_ptr<rule::Rule> DG::RuleIterator::dereference() const {
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	assert(g);
	assert(num_vertices(g->getHyper().getGraph()) > 0);
	assert(g->getHyper().getGraph()[*(vertices(g->getHyper().getGraph()).first + eId)].kind == lib::DG::HyperVertexKind::Edge);
	auto e = vertices(dg).first[eId];
	const auto &rs = dg[e].rules;
	assert(i < rs.size());
	return rs[i]->getAPIReference();
}

bool DG::RuleIterator::equal(const RuleIterator &iter) const {
	return g == iter.g && eId == iter.eId && i == iter.i;
}

void DG::RuleIterator::increment() {
	++i;
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto e = vertices(dg).first[eId];
	const auto &rs = dg[e].rules;
	if(i >= rs.size()) {
		g = nullptr;
		eId = 0;
		i = 0;
	}
}

std::size_t DG::RuleRange::size() const {
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto e = vertices(dg).first[eId];
	const auto &rs = dg[e].rules;
	return rs.size();
}

} // namespace dg
} // namespace mod