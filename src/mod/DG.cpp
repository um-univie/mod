#include "DG.h"

#include <mod/DGGraphInterface.h>
#include <mod/DGPrinter.h>
#include <mod/Error.h>
#include <mod/GraphPrinter.h>
#include <mod/Misc.h>
#include <mod/lib/DG/Hyper.h>
#include <mod/lib/DG/NonHyper.h>
#include <mod/lib/DG/NonHyperDerivations.h>
#include <mod/lib/DG/NonHyperRuleComp.h>
#include <mod/lib/DG/Strategies/GraphState.h>
#include <mod/lib/DG/Strategies/Strategy.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/IO/Derivation.h>
#include <mod/lib/IO/DG.h>
#include <mod/lib/IO/IO.h>

#include <boost/lexical_cast.hpp>

//#include <functional>
//#include <fstream>
//#include <sstream>

namespace mod {

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

std::size_t DG::getId() const {
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
	using boost::vertices;
	auto vs = vertices(getHyper().getGraph());
	return std::count_if(vs.first, vs.second, [this](lib::DG::HyperVertex v) {
		return getHyper().getGraph()[v].kind == lib::DG::HyperVertexKind::Edge;
	});
}

DG::EdgeRange DG::edges() const {
	return EdgeRange(getNonHyper().getAPIReference());
}

//------------------------------------------------------------------------------

DG::Vertex DG::findVertex(std::shared_ptr<Graph> g) const {
	const auto &dg = p->dg->getHyper().getGraph();
	auto v = p->dg->getHyper().getVertexFromGraph(&g->getGraph());
	if(v == dg.null_vertex()) return Vertex();
	else return Vertex(p->dg->getAPIReference(), get(boost::vertex_index_t(), dg, v));
}

DG::HyperEdge DG::findEdge(const std::vector<Vertex> &sources, const std::vector<Vertex> &targets) const {
	using boost::vertices;
	const auto &dg = p->dg->getHyper().getGraph();
	auto vs = vertices(dg).first;
	std::vector<lib::DG::Hyper::Vertex> vSources, vTargets;
	for(auto v : sources) {
		if(v.isNull()) throw LogicError("Source vertex descriptor is null.");
		if(v.getDG() != p->dg->getAPIReference()) throw LogicError("Source vertex descriptor does not belong to this derivation graph: "
				+ boost::lexical_cast<std::string>(v));
		vSources.push_back(vs[v.getId()]);
	}
	for(auto v : targets) {
		if(v.isNull()) throw LogicError("Target vertex descriptor is null.");
		if(v.getDG() != p->dg->getAPIReference()) throw LogicError("Target vertex descriptor does not belong to this derivation graph: "
				+ boost::lexical_cast<std::string>(v));
		vTargets.push_back(vs[v.getId()]);
	}
	return p->dg->getDerivationRef(vSources, vTargets);
}

DG::HyperEdge DG::findEdge(const std::vector<std::shared_ptr<Graph> > &sources, const std::vector<std::shared_ptr<Graph> > &targets) const {
	std::vector<Vertex> vSources, vTargets;
	for(auto g : sources) vSources.push_back(findVertex(g));
	for(auto g : targets) vTargets.push_back(findVertex(g));
	return findEdge(vSources, vTargets);
}

//------------------------------------------------------------------------------

void DG::calc() {
	if(p->dg->getHasCalculated()) {
		lib::IO::log() << "Notice: no effect of calculation for derivation graph. Already done." << std::endl;
		return;
	}
	p->dg->calculate();
}

const std::set<std::shared_ptr<Graph>, GraphLess> &DG::getGraphDatabase() const {
	return p->dg->getGraphDatabase();
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

void DG::list() const {
	p->dg->list(lib::IO::log());
}

void DG::listStats() const {
	if(!p->dg->getHasCalculated()) throw LogicError("No stats can be printed before calculation.\n");
	else p->dg->getHyper().printStats(lib::IO::log());
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
	for(const auto &d : derivations) {
		if(d.left.empty()) throw InputError("Derivation has empty left side: " + boost::lexical_cast<std::string>(d));
		if(d.right.empty()) throw InputError("Derivation has empty right side: " + boost::lexical_cast<std::string>(d));
	}
	return wrapIt(new DG(std::make_unique<lib::DG::NonHyperDerivations>(derivations)));
}

std::shared_ptr<DG> DG::abstract(const std::string &specification) {
	std::istringstream s(specification);
	std::ostringstream err;
	std::unique_ptr<lib::DG::NonHyper> dgInternal(lib::IO::DG::Read::abstract(s, err));
	if(!dgInternal) throw InputError(err.str());
	return wrapIt(new DG(std::move(dgInternal)));
}

std::shared_ptr<DG> DG::ruleComp(const std::vector<std::shared_ptr<Graph> > &graphs,
		std::shared_ptr<DGStrat> strategy) {
	auto dgInternal = std::make_unique<lib::DG::NonHyperRuleComp>(graphs, strategy->getStrategy().clone());
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

DerivationRef::DerivationRef(DG::HyperEdge e) : id(e.getId()), dg(e.getDG()) { }

DerivationRef::operator DG::HyperEdge() const {
	if(!isValid()) return DG::HyperEdge();
	else return DG::HyperEdge(dg, id);
}

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

} // namespace mod
