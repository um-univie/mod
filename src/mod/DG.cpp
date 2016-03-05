#include "DG.h"

#include <mod/Config.h>
#include <mod/Derivation.h>
#include <mod/DG.h>
#include <mod/Error.h>
#include <mod/Function.h>
#include <mod/Misc.h>
#include <mod/lib/GraphPimpl.h>
#include <mod/lib/DG/Hyper.h>
#include <mod/lib/DG/Matrix.h>
#include <mod/lib/DG/NonHyperDerivations.h>
#include <mod/lib/DG/NonHyperRuleComp.h>
#include <mod/lib/DG/Strategies/GraphState.h>
#include <mod/lib/DG/Strategies/Strategy.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/IO/Derivation.h>
#include <mod/lib/IO/DG.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/Rule/Base.h>

#include <jla_boost/Memory.hpp>

#include <boost/lexical_cast.hpp>

#include <functional>
#include <fstream>
#include <sstream>

namespace mod {

//------------------------------------------------------------------------------
// Vertex
//------------------------------------------------------------------------------

MOD_GRAPHPIMPL_Define_Vertex(DG, dg, g->getHyper().getGraph())
MOD_GRAPHPIMPL_Define_Vertex_Directed(DG, dg, g->getHyper().getGraph())

DG::InEdgeRange DG::Vertex::inEdges() const {
	if(!g) throw LogicError("Can not get in-edges on a null vertex.");
	return InEdgeRange(g, vId);
}

DG::OutEdgeRange DG::Vertex::outEdges() const {
	if(!g) throw LogicError("Can not get in-edges on a null vertex.");
	return OutEdgeRange(g, vId);
}

std::shared_ptr<Graph> DG::Vertex::getGraph() const {
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
	}
}

DG::HyperEdge::HyperEdge() : eId(0) { }

std::ostream &operator<<(std::ostream &s, const DG::HyperEdge &e) {
	s << "HyperEdge(";
	if(!e.getDG()) s << "null";
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

std::size_t DG::HyperEdge::getId() const {
	if(!g) throw LogicError("Can not get id on a null hyperedge.");
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto v = *(vertices(dg).first + eId);
	return get(boost::vertex_index_t(), dg, v);
}

std::shared_ptr<DG> DG::HyperEdge::getDG() const {
	if(!g) throw LogicError("Can not get derivation graph on a null hyperedge.");
	return g;
}

std::size_t DG::HyperEdge::numSources() const {
	if(!g) throw LogicError("Can not get number of sources on a null edge.");
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto v = *(vertices(dg).first + eId);
	return in_degree(v, dg);
}

DG::SourceRange DG::HyperEdge::sources() const {
	if(!g) throw LogicError("Can not get sources on a null edge.");
	return SourceRange(g, eId);
}

std::size_t DG::HyperEdge::numTargets() const {
	if(!g) throw LogicError("Can not get number of targets on a null edge.");
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto v = *(vertices(dg).first + eId);
	return out_degree(v, dg);
}

DG::TargetRange DG::HyperEdge::targets() const {
	if(!g) throw LogicError("Can not get targets on a null edge.");
	return TargetRange(g, eId);
}

DG::RuleRange DG::HyperEdge::rules() const {
	if(!g) throw LogicError("Can not get rules on a null edge.");
	return RuleRange(g, eId);
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
	g = nullptr;
	vId = 0;
}

DG::VertexIterator::VertexIterator() : vId(0) { }

DG::Vertex DG::VertexIterator::dereference() const {
	using boost::vertices;
	assert(g->getHyper().getGraph()[*(vertices(g->getHyper().getGraph()).first + vId)].kind == lib::DG::HyperVertexKind::Vertex);
	return DG::Vertex(g, vId);
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
	g = nullptr;
	eId = 0;
}

DG::EdgeIterator::EdgeIterator() : eId(0) { }

DG::HyperEdge DG::EdgeIterator::dereference() const {
	using boost::vertices;
	assert(g->getHyper().getGraph()[*(vertices(g->getHyper().getGraph()).first + eId)].kind == lib::DG::HyperVertexKind::Edge);
	return DG::HyperEdge(g, eId);
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
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto v = vertices(dg).first[vId];
	auto e = inv_adjacent_vertices(v, dg).first[eId];
	return HyperEdge(g, get(boost::vertex_index_t(), dg, e));
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
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto v = vertices(dg).first[vId];
	auto e = adjacent_vertices(v, dg).first[eId];
	return HyperEdge(g, get(boost::vertex_index_t(), dg, e));
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
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto e = vertices(dg).first[eId];
	auto v = inv_adjacent_vertices(e, dg).first[vId];
	return Vertex(g, get(boost::vertex_index_t(), dg, v));
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
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto e = vertices(dg).first[eId];
	auto v = adjacent_vertices(e, dg).first[vId];
	return Vertex(g, get(boost::vertex_index_t(), dg, v));
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

DG::RuleIterator::RuleIterator(std::shared_ptr<DG> g, std::size_t eId) : g(g), eId(eId), i(0) { }

DG::RuleIterator::RuleIterator() : eId(0), i(0) { }

std::shared_ptr<Rule> DG::RuleIterator::dereference() const {
	const auto &dg = g->getHyper().getGraph();
	using boost::vertices;
	auto e = vertices(dg).first[eId];
	const auto &rs = dg[e].rules;
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

//------------------------------------------------------------------------------
// DerivationRefRange stuff
//------------------------------------------------------------------------------

DG::DerivationRefRange::const_iterator::const_iterator(const DerivationRefRange &range, unsigned int id)
: range(range), id(id) { }

bool DG::DerivationRefRange::const_iterator::operator!=(const const_iterator &other) const {
	return id != other.id;
}

bool DG::DerivationRefRange::const_iterator::operator==(const const_iterator &other) const {
	return id == other.id;
}

DG::DerivationRefRange::const_iterator DG::DerivationRefRange::const_iterator::operator++() {
	assert(*this != range.end());
	id = range.next(id);
	return *this;
}

DG::DerivationRefRange::const_iterator DG::DerivationRefRange::const_iterator::operator++(int) {
	assert(*this != range.end());
	const_iterator old = *this;
	++(*this);
	return old;
}

DerivationRef DG::DerivationRefRange::const_iterator::operator*() const {
	return range.dereference(id);
}

DG::DerivationRefRange::const_iterator DG::DerivationRefRange::begin() const {
	return const_iterator(*this, firstId);
}

DG::DerivationRefRange::const_iterator DG::DerivationRefRange::end() const {
	return const_iterator(*this, endId);
}

//------------------------------------------------------------------------------
// DG
//------------------------------------------------------------------------------

struct DG::Pimpl {

	Pimpl(std::unique_ptr<lib::DG::NonHyper> dg) : dg(std::move(dg)) { }
public:
	const std::unique_ptr<lib::DG::NonHyper> dg;
};

DG::DG(std::unique_ptr<lib::DG::NonHyper> dg) : p(new Pimpl(std::move(dg))) { }

DG::~DG() { }

unsigned int DG::getId() const {
	return p->dg->getId();
}

std::ostream &operator<<(std::ostream &s, const DG &dg) {
	return s << dg.p->dg->getType() << "(" << dg.p->dg->getId() << ")";
}

const lib::DG::NonHyper &DG::getNonHyper() const {
	return *p->dg;
}

const lib::DG::Hyper &DG::getHyper() const {
	return getNonHyper().getHyper();
}

//------------------------------------------------------------------------------

std::size_t DG::numVertices() const {
	using boost::vertices;
	auto vs = vertices(getHyper().getGraph());
	return std::count_if(vs.first, vs.second, [this](lib::DG::HyperVertex v) {
		return getHyper().getGraph()[v].kind == lib::DG::HyperVertexKind::Vertex;
	});
}

DG::VertexRange DG::vertices() const {
	return VertexRange(p->dg->getAPIReference());
}

std::size_t DG::numEdges() const {
	return num_edges(getHyper().getGraph());
}

DG::EdgeRange DG::edges() const {
	return EdgeRange(getNonHyper().getAPIReference());
}

//------------------------------------------------------------------------------

void DG::calc() {
	if(p->dg->getHasCalculated()) {
		lib::IO::log() << "Notice: no effect of calculation for derivation graph. Already done." << std::endl;
		return;
	}
	p->dg->calculate();
	lib::IO::log() << "Notice: Calculation of derivation graph done." << std::endl;
}

const std::set<std::shared_ptr<Graph>, GraphLess> &DG::getGraphDatabase() const {
	return p->dg->getGraphDatabase();
}

const std::vector<std::shared_ptr<Graph> > &DG::getVertexGraphs() const {
	if(!p->dg->getHasCalculated()) {
		lib::IO::log() << "Notice: The set of vertex graphs is empty as the calculation has not been done." << std::endl;
		static const std::vector<std::shared_ptr<Graph> > empty;
		return empty;
	}
	return p->dg->getHyper().getVertexGraphs();
}

const std::vector<std::shared_ptr<Graph> > &DG::getProducts() const {
	if(!p->dg->getHasCalculated()) {
		lib::IO::log() << "Notice: The set of product graphs is empty as the calculation has not been done." << std::endl;
		static const std::vector<std::shared_ptr<Graph> > empty;
		return empty;
	}
	return getNonHyper().getProducts();
}

void DG::print(const DGPrintData &data, const DGPrinter &printer) const {
	if(data.getDG() != getNonHyper().getAPIReference()) {
		std::ostringstream err;
		err << "DGPrintData is for another derivation graph (id=" << data.getDG()->getId()
				<< ") than this (id=" << getId() << ")" << std::endl;
		throw LogicError(err.str());
	}
	lib::IO::DG::Write::summary(data.getData(), printer.getPrinter(), printer.getGraphPrinter().getOptions());
}

std::string DG::dump() const {
	if(!p->dg->getHasCalculated()) throw LogicError("No dump can be done before the derivation graph it has been calculated.\n");
	else return lib::IO::DG::Write::dump(*p->dg);
}

void DG::printMatrix() const {
	if(!p->dg->getHasCalculated()) throw LogicError("No matrix printing can be done before the derivation graph it has been calculated.\n");
	else p->dg->getHyper().getMatrixSparse().print();
}

DerivationRef DG::getDerivationRef(const std::vector<std::shared_ptr<mod::Graph> > &educts,
		const std::vector<std::shared_ptr<mod::Graph> > &products) const {
	if(!p->dg->getHasCalculated()) throw LogicError("Derivations are not available before the derivation graph has been calculated.\n");
	else return p->dg->getDerivationRef(educts, products, true);
}

DG::DerivationRefRange DG::derivations() const {
	if(!p->dg->getHasCalculated()) throw LogicError("Derivations are not available before the derivation graph has been calculated.\n");
	return DerivationRefRange(p->dg->getHyper().getFirstDerivation(), p->dg->getHyper().getEndDerivation(),
			[this](unsigned int id) {
				return p->dg->getHyper().getNextDerivation(id);
			},
	[this](unsigned int id) {
		return p->dg->getHyper().dereferenceDerivation(id);
	});
}

DG::DerivationRefRange DG::inDerivations(std::shared_ptr<mod::Graph> g) const {
	if(!p->dg->getHasCalculated()) throw LogicError("Derivations are not available before the derivation graph has been calculated.\n");
	lib::DG::HyperVertex v = p->dg->getHyper().getVertexFromGraph(&g->getGraph());
	return DerivationRefRange(p->dg->getHyper().getFirstInDerivation(v), p->dg->getHyper().getEndInDerivation(v),
			[this, v](unsigned int id) {
				return p->dg->getHyper().getNextInDerivation(id, v);
			},
	[this, v](unsigned int id) {
		return p->dg->getHyper().dereferenceInDerivation(id, v);
	});
}

DG::DerivationRefRange DG::outDerivations(std::shared_ptr<mod::Graph> g) const {
	if(!p->dg->getHasCalculated()) throw LogicError("Derivations are not available before the derivation graph has been calculated.\n");
	lib::DG::HyperVertex v = p->dg->getHyper().getVertexFromGraph(&g->getGraph());
	return DerivationRefRange(p->dg->getHyper().getFirstOutDerivation(v), p->dg->getHyper().getEndOutDerivation(v),
			[this, v](unsigned int id) {
				return p->dg->getHyper().getNextOutDerivation(id, v);
			},
	[this, v](unsigned int id) {
		return p->dg->getHyper().dereferenceOutDerivation(id, v);
	});
}

void DG::list() const {
	p->dg->list(lib::IO::log());
}

void DG::listStats() const {
	if(!p->dg->getHasCalculated()) throw LogicError("No stats can be printed before calculation.\n");
	else p->dg->getHyper().printStats(lib::IO::log());
}

void DG::listIncidence(const std::shared_ptr<Graph> g) const {
	if(!p->dg->getHasCalculated()) throw LogicError("No incidence information is available before calculation.\n");
	else p->dg->getHyper().printIncidence(lib::IO::log(), g);
}

std::vector<std::shared_ptr<Graph> > DG::getStratOutputSubset() const {
	std::vector<std::shared_ptr<Graph> > res;
	if(auto *ptr = dynamic_cast<lib::DG::NonHyperRuleComp*> (p->dg.get())) {
		if(!p->dg->getHasCalculated()) throw LogicError("The output subset can not be retrieved before calculation.\n");
		else for(auto *g : ptr->getOutput().getSubset(0)) res.push_back(g->getAPIReference());
	} else throw LogicError("The output subset can only be retrieved from derivation graphs based on rule composition.\n");
	return res;
}

//------------------------------------------------------------------------------
// Static
//------------------------------------------------------------------------------

namespace {

std::shared_ptr<DG> wrapIt(DG *dgPtr) {
	std::shared_ptr<DG> dg(dgPtr);
	const lib::DG::NonHyper &cNonHyper = dg->getNonHyper();
	const_cast<lib::DG::NonHyper&> (cNonHyper).setAPIReference(dg);
	return dg;
}

} // namespace 

std::shared_ptr<DG> DG::derivations(const std::vector<Derivation> &derivations) {
	return wrapIt(new DG(make_unique<lib::DG::NonHyperDerivations>(derivations)));
}

std::shared_ptr<DG> DG::abstract(const std::string &specification) {
	std::istringstream s(specification);
	std::ostringstream err;
	std::unique_ptr<lib::DG::NonHyper> dgInternal(lib::IO::DG::Read::abstract(s, err));
	if(!dgInternal) throw InputError(err.str());
	return wrapIt(new DG(std::move(dgInternal)));
}

std::shared_ptr<DG> DG::sbml(const std::string &file) {
	throw FatalError("The SBML loading function does not currently work.");
	std::string fullName = prefixFilename(file);
	std::ostringstream err;
	std::unique_ptr<lib::DG::Matrix> matrix(lib::IO::DG::Read::sbml(fullName, err));
	if(!matrix) throw InputError(err.str());
	//	auto dgInternal = make_unique<lib::DG::NonHyperShallow>(*matrix);
	//	return wrapIt(new DG(std::move(dgInternal)));
	return nullptr;
}

std::shared_ptr<DG> DG::ruleComp(const std::vector<std::shared_ptr<Graph> > &graphs,
		std::shared_ptr<DGStrat> strategy) {
	auto dgInternal = make_unique<lib::DG::NonHyperRuleComp>(graphs, strategy->getStrategy().clone());
	return wrapIt(new DG(std::move(dgInternal)));
}

std::shared_ptr<DG> DG::dumpImport(const std::vector<std::shared_ptr<Graph> > &graphs, const std::vector<std::shared_ptr<Rule> > &rules, const std::string &file) {
	std::string fullName = prefixFilename(file);
	std::ifstream fileStream(fullName.c_str());
	if(!fileStream.is_open()) throw InputError("DG dump file not found, '" + fullName + "'\n");
	fileStream.close();
	std::ostringstream err;
	std::unique_ptr<lib::DG::NonHyper> dgInternal(lib::IO::DG::Read::dump(graphs, rules, fullName, err));
	if(!dgInternal) throw InputError(err.str());
	return wrapIt(new DG(std::move(dgInternal)));
}

void DG::diff(std::shared_ptr<mod::DG> dg1, std::shared_ptr<mod::DG> dg2) {
	lib::DG::NonHyper::diff(dg1->getNonHyper(), dg2->getNonHyper());
}

//------------------------------------------------------------------------------
// DerivationReference
//------------------------------------------------------------------------------

DerivationRef::DerivationRef(unsigned int id, std::shared_ptr<mod::DG> dg) : id(id), dg(dg) { }

DerivationRef::DerivationRef() : id(std::numeric_limits<unsigned int>::max()), dg(nullptr) { }

std::ostream &operator<<(std::ostream &s, const DerivationRef &d) {
	s << "'Derivation " << d.id << " in ";
	if(d.dg) s << *d.dg;
	else s << "<nullptr>";
	s << "'";
	return s;
}

unsigned int DerivationRef::getId() const {
	return id;
}

std::shared_ptr<DG> DerivationRef::getDG() const {
	return dg;
}

bool DerivationRef::isValid() const {
	return id != std::numeric_limits<unsigned int>::max()
			&& dg->getHyper().getGraph()[vertex(id, dg->getHyper().getGraph())].kind == lib::DG::HyperVertexKind::Edge;
}

Derivation DerivationRef::operator*() const {
	if(!isValid()) {
		std::stringstream ss;
		ss << "DerivationRef not valid: " << *this << std::endl;
		throw FatalError(ss.str());
	}
	return dg->getHyper().getDerivation(vertex(id, dg->getHyper().getGraph()));
}

void DerivationRef::print(const GraphPrinter &printer, const std::string &matchColour) const {
	if(!isValid()) throw LogicError("Can not print invalid derivation.");
	auto v = dg->getHyper().getVertexFromDerivationRef(*this);
	assert(dg->getHyper().getGraph()[v].kind == lib::DG::HyperVertexKind::Edge);
	assert(dg->getHyper().getGraph()[v].rules.size() == 1);
	if(!dg->getHyper().getGraph()[v].rules.front()->isReal()) throw LogicError("Can not print derivation with fake rule.");
	lib::IO::Derivation::Write::summary(dg->getNonHyper(), v, printer.getOptions(), matchColour);
}

bool DerivationRef::operator<(const DerivationRef &other) const {
	// invalid < valid
	// invalid == invalid
	if(!isValid() || !other.isValid()) {
		//	v1	v2	r
		//	0	0	0
		//	0	1	1
		//	1	0	0
		!isValid() && other.isValid();
	}
	if(dg->getId() != other.dg->getId()) return dg->getId() < other.dg->getId();
	else return id < other.id;
}
//------------------------------------------------------------------------------
// DGPrintData
//------------------------------------------------------------------------------

namespace {

void printDataCheckDerivationRef(std::shared_ptr<mod::DG> dg, DerivationRef dRef) {
	if(!dRef.isValid()) throw FatalError("DerivationRef is not valid.", 1);
	if(dRef.getDG()->getId() != dg->getId()) {
		std::string text = "DerivationRef is from another derivation graph (id=" + boost::lexical_cast<std::string>(dRef.getDG()->getId());
		text += ") than this (id=";
		text += boost::lexical_cast<std::string>(dg->getId());
		text += ").";
		throw FatalError(std::move(text), 1);
	}
}

void reconnectCommon(std::shared_ptr<mod::DG> dg, DerivationRef dRef, unsigned int eDup, std::shared_ptr<mod::Graph> g, unsigned int vDupTar, bool isTail) {
	printDataCheckDerivationRef(dg, dRef);
	if(!dg->getHyper().isVertexGraph(g))
		throw FatalError("Graph " + boost::lexical_cast<std::string>(g->getId()) + ", '" + g->getName()
			+ "' does not represent a vertex in this derivation graph.", 1);
	Derivation der = *dRef;
	const auto &gList = isTail ? der.left : der.right;
	bool found = std::find(begin(gList), end(gList), g) != end(gList);
	if(!found) throw FatalError("Graph " + boost::lexical_cast<std::string>(g->getId()) + ", '" + g->getName() + "' is not a tail.", 1);
}

} // namespace

DGPrintData::DGPrintData(std::shared_ptr<mod::DG> dg) : dg(dg), data(nullptr) {
	if(!dg->getNonHyper().getHasCalculated()) {
		throw FatalError("Can not create DGPrintData. DG " + boost::lexical_cast<std::string>(dg->getId()) + " has not been calculated.");
	}
	data.reset(new lib::IO::DG::Write::Data(dg->getHyper()));
}

DGPrintData::DGPrintData(const DGPrintData &other) : dg(other.dg), data(new lib::IO::DG::Write::Data(*other.data)) { }

DGPrintData::DGPrintData(DGPrintData &&other) : dg(other.dg), data(std::move(other.data)) { }

DGPrintData::~DGPrintData() { }

lib::IO::DG::Write::Data &DGPrintData::getData() {
	return *data;
}

lib::IO::DG::Write::Data &DGPrintData::getData() const {
	return *data;
}

std::shared_ptr<mod::DG> DGPrintData::getDG() const {
	return dg;
}

void DGPrintData::makeDuplicate(DerivationRef dRef, unsigned int eDup) {
	printDataCheckDerivationRef(getDG(), dRef);
	lib::DG::Hyper::Vertex e = getDG()->getHyper().getVertexFromDerivationRef(dRef);
	data->makeDuplicate(e, eDup);
}

void DGPrintData::removeDuplicate(DerivationRef dRef, unsigned int eDup) {
	printDataCheckDerivationRef(getDG(), dRef);
	lib::DG::Hyper::Vertex e = getDG()->getHyper().getVertexFromDerivationRef(dRef);
	data->removeDuplicate(e, eDup);
}

void DGPrintData::reconnectTail(DerivationRef dRef, unsigned int eDup, std::shared_ptr<mod::Graph> g, unsigned int vDupTar) {
	reconnectCommon(getDG(), dRef, eDup, g, vDupTar, true);
	lib::DG::Hyper::Vertex e = getDG()->getHyper().getVertexFromDerivationRef(dRef);
	lib::DG::Hyper::Vertex tail = getDG()->getHyper().getVertexFromGraph(&g->getGraph());
	data->reconnectTail(e, eDup, tail, vDupTar, std::numeric_limits<unsigned int>::max());
}

void DGPrintData::reconnectHead(DerivationRef dRef, unsigned int eDup, std::shared_ptr<mod::Graph> g, unsigned int vDupTar) {
	reconnectCommon(getDG(), dRef, eDup, g, vDupTar, false);
	lib::DG::Hyper::Vertex e = getDG()->getHyper().getVertexFromDerivationRef(dRef);
	lib::DG::Hyper::Vertex head = getDG()->getHyper().getVertexFromGraph(&g->getGraph());
	data->reconnectHead(e, eDup, head, vDupTar, std::numeric_limits<unsigned int>::max());
}

//------------------------------------------------------------------------------
// DGPrinter
//------------------------------------------------------------------------------

DGPrinter::DGPrinter() : graphPrinter(make_unique<GraphPrinter>()), printer(make_unique<lib::IO::DG::Write::Printer>()) {
	graphPrinter->enableAll();
	graphPrinter->setWithIndex(false);
}

DGPrinter::~DGPrinter() { }

lib::IO::DG::Write::Printer &DGPrinter::getPrinter() const {
	return *printer;
}

GraphPrinter &DGPrinter::getGraphPrinter() {
	return *graphPrinter;
}

const GraphPrinter &DGPrinter::getGraphPrinter() const {
	return *graphPrinter;
}

void DGPrinter::setWithShortcutEdges(bool value) {
	printer->baseOptions.WithShortcutEdges(value);
}

bool DGPrinter::getWithShortcutEdges() const {
	return printer->baseOptions.withShortcutEdges;
}

void DGPrinter::setWithGraphImages(bool value) {
	printer->baseOptions.WithGraphImages(value);
}

bool DGPrinter::getWithGraphImages() const {
	return printer->baseOptions.withGraphImages;
}

void DGPrinter::setLabelsAsLatexMath(bool value) {
	printer->baseOptions.LabelsAsLatexMath(value);
}

bool DGPrinter::getLabelsAsLatexMath() const {
	return printer->baseOptions.labelsAsLatexMath;
}

void DGPrinter::pushVertexVisible(std::function<bool(std::shared_ptr<Graph>, std::shared_ptr<DG>) > f) {
	printer->pushVertexVisible([f](lib::DG::HyperVertex v, const lib::DG::Hyper & dg) {
		return f(dg.getGraph()[v].graph->getAPIReference(), dg.getNonHyper().getAPIReference());
	});
}

void DGPrinter::popVertexVisible() {
	printer->popVertexVisible();
}

void DGPrinter::pushEdgeVisible(std::function<bool(DerivationRef) > f) {
	printer->pushEdgeVisible([f](lib::DG::HyperVertex v, const lib::DG::Hyper & dg) {
		return f(dg.getDerivationRefValid(v));
	});
}

void DGPrinter::popEdgeVisible() {
	printer->popEdgeVisible();
}

void DGPrinter::setWithShortcutEdgesAfterVisibility(bool value) {
	printer->baseOptions.WithShortcutEdgesAfterVisibility(value);
}

bool DGPrinter::getWithShortcutEdgesAfterVisibility() const {
	return printer->baseOptions.withShortcutEdgesAfterVisibility;
}

void DGPrinter::setVertexLabelSep(std::string sep) {
	printer->vertexLabelSep = sep;
}

const std::string &DGPrinter::getVertexLabelSep() {
	return printer->vertexLabelSep;
}

void DGPrinter::setEdgeLabelSep(std::string sep) {
	printer->edgeLabelSep = sep;
}

const std::string &DGPrinter::getEdgeLabelSep() {
	return printer->edgeLabelSep;
}

void DGPrinter::pushVertexLabel(std::function<std::string(std::shared_ptr<Graph>, std::shared_ptr<DG>) > f) {
	printer->pushVertexLabel([f](lib::DG::HyperVertex v, const lib::DG::Hyper & dg) {
		return f(dg.getGraph()[v].graph->getAPIReference(), dg.getNonHyper().getAPIReference());
	});
}

void DGPrinter::popVertexLabel() {
	printer->popVertexLabel();
}

void DGPrinter::pushEdgeLabel(std::function<std::string(DerivationRef) > f) {
	printer->pushEdgeLabel([f](lib::DG::HyperVertex v, const lib::DG::Hyper & dg) {
		return f(dg.getDerivationRefValid(v));
	});
}

void DGPrinter::popEdgeLabel() {
	printer->popEdgeLabel();
}

void DGPrinter::setWithGraphName(bool value) {
	printer->withGraphName = value;
}

bool DGPrinter::getWithGraphName() const {
	return printer->withGraphName;
}

void DGPrinter::setWithRuleName(bool value) {
	printer->withRuleName = value;
}

bool DGPrinter::getWithRuleName() const {
	return printer->withRuleName;
}

void DGPrinter::setWithRuleId(bool value) {
	printer->withRuleId = value;
}

bool DGPrinter::getWithRuleId() const {
	return printer->withRuleId;
}

void DGPrinter::pushVertexColour(std::function<std::string(std::shared_ptr<Graph>, std::shared_ptr<DG>) > f, bool extendToEdges) {
	printer->pushVertexColour([f](lib::DG::HyperVertex v, const lib::DG::Hyper & dg) {
		return f(dg.getGraph()[v].graph->getAPIReference(), dg.getNonHyper().getAPIReference());
	}, extendToEdges);
}

void DGPrinter::popVertexColour() {
	printer->popVertexColour();
}

void DGPrinter::pushEdgeColour(std::function<std::string(DerivationRef) > f) {
	printer->pushEdgeColour([f](lib::DG::HyperVertex v, const lib::DG::Hyper & dg) {
		return f(dg.getDerivationRefValid(v));
	});
}

void DGPrinter::popEdgeColour() {
	printer->popEdgeColour();
}

} // namespace mod