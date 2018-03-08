#include "DG.h"

#include <mod/Error.h>
#include <mod/Misc.h>
#include <mod/dg/GraphInterface.h>
#include <mod/dg/Printer.h>
#include <mod/graph/Printer.h>
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

namespace mod {
namespace dg {

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
	if(!p->dg->getHasCalculated()) throw LogicError("Can not get number of vertices before the derivation graph it has been calculated.\n");
	using boost::vertices;
	auto vs = vertices(getHyper().getGraph());
	return std::count_if(vs.first, vs.second, [this](lib::DG::HyperVertex v) {
		return getHyper().getGraph()[v].kind == lib::DG::HyperVertexKind::Vertex;
	});
}

DG::VertexRange DG::vertices() const {
	if(!p->dg->getHasCalculated()) throw LogicError("Can not get vertices before the derivation graph it has been calculated.\n");
	return VertexRange(p->dg->getAPIReference());
}

std::size_t DG::numEdges() const {
	if(!p->dg->getHasCalculated()) throw LogicError("Can not get number of edges before the derivation graph it has been calculated.\n");
	using boost::vertices;
	auto vs = vertices(getHyper().getGraph());
	return std::count_if(vs.first, vs.second, [this](lib::DG::HyperVertex v) {
		return getHyper().getGraph()[v].kind == lib::DG::HyperVertexKind::Edge;
	});
}

DG::EdgeRange DG::edges() const {
	if(!p->dg->getHasCalculated()) throw LogicError("Can not get edges before the derivation graph it has been calculated.\n");
	return EdgeRange(getNonHyper().getAPIReference());
}

//------------------------------------------------------------------------------

DG::Vertex DG::findVertex(std::shared_ptr<graph::Graph> g) const {
	const auto &dg = p->dg->getHyper();
	const auto v = dg.getVertexOrNullFromGraph(&g->getGraph());
	return dg.getInterfaceVertex(v);
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
	const auto vInner = p->dg->findHyperEdge(vSources, vTargets);
	if(vInner == boost::graph_traits<lib::DG::HyperGraphType>::null_vertex()) return HyperEdge();
	return p->dg->getHyper().getInterfaceEdge(vInner);
}

DG::HyperEdge DG::findEdge(const std::vector<std::shared_ptr<graph::Graph> > &sources, const std::vector<std::shared_ptr<graph::Graph> > &targets) const {
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

const std::set<std::shared_ptr<graph::Graph>, graph::GraphLess> &DG::getGraphDatabase() const {
	return p->dg->getGraphDatabase();
}

const std::vector<std::shared_ptr<graph::Graph> > &DG::getProducts() const {
	if(!p->dg->getHasCalculated()) {
		lib::IO::log() << "Notice: The set of product graphs is empty as the calculation has not been done." << std::endl;
		static const std::vector<std::shared_ptr<graph::Graph> > empty;
		return empty;
	}
	return getNonHyper().getProducts();
}

std::string DG::print(const PrintData &data, const Printer &printer) const {
	if(data.getDG() != getNonHyper().getAPIReference()) {
		std::ostringstream err;
		err << "PrintData is for another derivation graph (id=" << data.getDG()->getId()
				<< ") than this (id=" << getId() << ")" << std::endl;
		throw LogicError(err.str());
	}
	return lib::IO::DG::Write::summary(data.getData(), printer.getPrinter(), printer.getGraphPrinter().getOptions());
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

std::vector<std::shared_ptr<graph::Graph> > DG::getStratOutputSubset() const {
	std::vector<std::shared_ptr<graph::Graph> > res;
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
	std::ostringstream err;
	std::unique_ptr<lib::DG::NonHyper> dgInternal(lib::IO::DG::Read::abstract(specification, err));
	if(!dgInternal) throw InputError(err.str());
	return wrapIt(new DG(std::move(dgInternal)));
}

std::shared_ptr<DG> DG::ruleComp(const std::vector<std::shared_ptr<graph::Graph> > &graphs,
		std::shared_ptr<Strategy> strategy, LabelSettings labelSettings, bool ignoreRuleLabelTypes) {
	auto dgInternal = std::make_unique<lib::DG::NonHyperRuleComp>(graphs, strategy->getStrategy().clone(), labelSettings, ignoreRuleLabelTypes);
	return wrapIt(new DG(std::move(dgInternal)));
}

std::shared_ptr<DG> DG::dumpImport(const std::vector<std::shared_ptr<graph::Graph> > &graphs, const std::vector<std::shared_ptr<rule::Rule> > &rules, const std::string &file) {
	std::string fullName = prefixFilename(file);
	std::ifstream fileStream(fullName.c_str());
	if(!fileStream.is_open()) throw InputError("DG dump file not found, '" + fullName + "'\n");
	fileStream.close();
	std::ostringstream err;
	std::unique_ptr<lib::DG::NonHyper> dgInternal(lib::IO::DG::Read::dump(graphs, rules, fullName, err));
	if(!dgInternal) throw InputError(err.str());
	return wrapIt(new DG(std::move(dgInternal)));
}

void DG::diff(std::shared_ptr<DG> dg1, std::shared_ptr<DG> dg2) {
	lib::DG::NonHyper::diff(dg1->getNonHyper(), dg2->getNonHyper());
}

} // namespace dg
} // namespace mod
