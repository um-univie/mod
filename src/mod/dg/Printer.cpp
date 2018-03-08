#include "Printer.h"

#include <mod/Error.h>
#include <mod/dg/DG.h>
#include <mod/dg/GraphInterface.h>
#include <mod/graph/Printer.h>
#include <mod/lib/DG/Hyper.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/IO/DG.h>

#include <boost/lexical_cast.hpp>

namespace mod {
namespace dg {

//------------------------------------------------------------------------------
// PrintData
//------------------------------------------------------------------------------

namespace {

void printDataCheckHyperEdge(std::shared_ptr<DG> dg, DG::HyperEdge e) {
	if(e.isNull()) throw FatalError("HyperEdge is null.", 1);
	if(e.getDG()->getId() != dg->getId()) {
		std::string text = "HyperEdge is from another derivation graph (id=" + boost::lexical_cast<std::string>(e.getDG()->getId());
		text += ") than this (id=";
		text += boost::lexical_cast<std::string>(dg->getId());
		text += ").";
		throw FatalError(std::move(text), 1);
	}
}

void reconnectCommon(std::shared_ptr<DG> dg, DG::HyperEdge e, unsigned int eDup, std::shared_ptr<graph::Graph> g, unsigned int vDupTar, bool isTail) {
	printDataCheckHyperEdge(dg, e);
	if(!dg->getHyper().isVertexGraph(&g->getGraph()))
		throw FatalError("Graph " + boost::lexical_cast<std::string>(g->getId()) + ", '" + g->getName()
			+ "' does not represent a vertex in this derivation graph.", 1);
	const bool found = [&]() {
		if(isTail) {
			const auto src = e.sources();
			return src.end() != std::find_if(src.begin(), src.end(), [&](const auto &v) {
				return v.getGraph() == g;
			});
		} else {
			const auto tar = e.targets();
			return tar.end() != std::find_if(tar.begin(), tar.end(), [&](const auto &v) {
				return v.getGraph() == g;
			});
		}
	}();
	if(!found) {
		std::string name = isTail ? "tail." : "head.";
		throw FatalError("Graph " + boost::lexical_cast<std::string>(g->getId()) + ", '" + g->getName() + "' is not a " + name, 1);
	}
}

} // namespace

PrintData::PrintData(std::shared_ptr<DG> dg) : dg(dg), data(nullptr) {
	if(!dg->getNonHyper().getHasCalculated()) {
		throw LogicError("Can not create PrintData. DG " + boost::lexical_cast<std::string>(dg->getId()) + " has not been calculated.");
	}
	data.reset(new lib::IO::DG::Write::Data(dg->getHyper()));
}

PrintData::PrintData(const PrintData &other) : dg(other.dg), data(new lib::IO::DG::Write::Data(*other.data)) { }

PrintData::PrintData(PrintData &&other) : dg(other.dg), data(std::move(other.data)) { }

PrintData::~PrintData() { }

lib::IO::DG::Write::Data &PrintData::getData() {
	return *data;
}

lib::IO::DG::Write::Data &PrintData::getData() const {
	return *data;
}

std::shared_ptr<DG> PrintData::getDG() const {
	return dg;
}

void PrintData::makeDuplicate(DG::HyperEdge e, unsigned int eDup) {
	printDataCheckHyperEdge(getDG(), e);
	lib::DG::Hyper::Vertex eInner = *(vertices(getDG()->getHyper().getGraph()).first + e.getId());
	data->makeDuplicate(eInner, eDup);
}

void PrintData::removeDuplicate(DG::HyperEdge e, unsigned int eDup) {
	printDataCheckHyperEdge(getDG(), e);
	lib::DG::Hyper::Vertex eInner = *(vertices(getDG()->getHyper().getGraph()).first + e.getId());
	data->removeDuplicate(eInner, eDup);
}

void PrintData::reconnectTail(DG::HyperEdge e, unsigned int eDup, std::shared_ptr<graph::Graph> g, unsigned int vDupTar) {
	reconnectCommon(getDG(), e, eDup, g, vDupTar, true);
	lib::DG::Hyper::Vertex eInner = *(vertices(getDG()->getHyper().getGraph()).first + e.getId());
	lib::DG::Hyper::Vertex tail = getDG()->getHyper().getVertexFromGraph(&g->getGraph());
	data->reconnectTail(eInner, eDup, tail, vDupTar, std::numeric_limits<unsigned int>::max());
}

void PrintData::reconnectHead(DG::HyperEdge e, unsigned int eDup, std::shared_ptr<graph::Graph> g, unsigned int vDupTar) {
	reconnectCommon(getDG(), e, eDup, g, vDupTar, false);
	lib::DG::Hyper::Vertex eInner = *(vertices(getDG()->getHyper().getGraph()).first + e.getId());
	lib::DG::Hyper::Vertex head = getDG()->getHyper().getVertexFromGraph(&g->getGraph());
	data->reconnectHead(eInner, eDup, head, vDupTar, std::numeric_limits<unsigned int>::max());
}

//------------------------------------------------------------------------------
// Printer
//------------------------------------------------------------------------------

Printer::Printer() : graphPrinter(std::make_unique<graph::Printer>()), printer(std::make_unique<lib::IO::DG::Write::Printer>()) {
	graphPrinter->enableAll();
	graphPrinter->setWithIndex(false);
}

Printer::~Printer() { }

lib::IO::DG::Write::Printer &Printer::getPrinter() const {
	return *printer;
}

graph::Printer &Printer::getGraphPrinter() {
	return *graphPrinter;
}

const graph::Printer &Printer::getGraphPrinter() const {
	return *graphPrinter;
}

void Printer::setWithShortcutEdges(bool value) {
	printer->baseOptions.WithShortcutEdges(value);
}

bool Printer::getWithShortcutEdges() const {
	return printer->baseOptions.withShortcutEdges;
}

void Printer::setWithGraphImages(bool value) {
	printer->baseOptions.WithGraphImages(value);
}

bool Printer::getWithGraphImages() const {
	return printer->baseOptions.withGraphImages;
}

void Printer::setLabelsAsLatexMath(bool value) {
	printer->baseOptions.LabelsAsLatexMath(value);
}

bool Printer::getLabelsAsLatexMath() const {
	return printer->baseOptions.labelsAsLatexMath;
}

void Printer::pushVertexVisible(std::function<bool(std::shared_ptr<graph::Graph>, std::shared_ptr<DG>) > f) {
	printer->pushVertexVisible([f](lib::DG::HyperVertex v, const lib::DG::Hyper & dg) {
		return f(dg.getGraph()[v].graph->getAPIReference(), dg.getNonHyper().getAPIReference());
	});
}

void Printer::popVertexVisible() {
	printer->popVertexVisible();
}

void Printer::pushEdgeVisible(std::function<bool(DG::HyperEdge) > f) {
	printer->pushEdgeVisible([f](lib::DG::HyperVertex v, const lib::DG::Hyper & dg) {
		assert(dg.getGraph()[v].kind == lib::DG::HyperVertexKind::Edge);
		return f(dg.getInterfaceEdge(v));
	});
}

void Printer::popEdgeVisible() {
	printer->popEdgeVisible();
}

void Printer::setWithShortcutEdgesAfterVisibility(bool value) {
	printer->baseOptions.WithShortcutEdgesAfterVisibility(value);
}

bool Printer::getWithShortcutEdgesAfterVisibility() const {
	return printer->baseOptions.withShortcutEdgesAfterVisibility;
}

void Printer::setVertexLabelSep(std::string sep) {
	printer->vertexLabelSep = sep;
}

const std::string &Printer::getVertexLabelSep() {
	return printer->vertexLabelSep;
}

void Printer::setEdgeLabelSep(std::string sep) {
	printer->edgeLabelSep = sep;
}

const std::string &Printer::getEdgeLabelSep() {
	return printer->edgeLabelSep;
}

void Printer::pushVertexLabel(std::function<std::string(std::shared_ptr<graph::Graph>, std::shared_ptr<DG>) > f) {
	printer->pushVertexLabel([f](lib::DG::HyperVertex v, const lib::DG::Hyper & dg) {
		return f(dg.getGraph()[v].graph->getAPIReference(), dg.getNonHyper().getAPIReference());
	});
}

void Printer::popVertexLabel() {
	printer->popVertexLabel();
}

void Printer::pushEdgeLabel(std::function<std::string(DG::HyperEdge) > f) {
	printer->pushEdgeLabel([f](lib::DG::HyperVertex v, const lib::DG::Hyper & dg) {
		assert(dg.getGraph()[v].kind == lib::DG::HyperVertexKind::Edge);
		return f(dg.getInterfaceEdge(v));
	});
}

void Printer::popEdgeLabel() {
	printer->popEdgeLabel();
}

void Printer::setWithGraphName(bool value) {
	printer->withGraphName = value;
}

bool Printer::getWithGraphName() const {
	return printer->withGraphName;
}

void Printer::setWithRuleName(bool value) {
	printer->withRuleName = value;
}

bool Printer::getWithRuleName() const {
	return printer->withRuleName;
}

void Printer::setWithRuleId(bool value) {
	printer->withRuleId = value;
}

bool Printer::getWithRuleId() const {
	return printer->withRuleId;
}

void Printer::setWithInlineGraphs(bool value) {
	printer->baseOptions.withInlineGraphs = value;
}

bool Printer::getWithInlineGraphs() const {
	return printer->baseOptions.withInlineGraphs;
}

void Printer::pushVertexColour(std::function<std::string(std::shared_ptr<graph::Graph>, std::shared_ptr<DG>) > f, bool extendToEdges) {
	printer->pushVertexColour([f](lib::DG::HyperVertex v, const lib::DG::Hyper & dg) {
		return f(dg.getGraph()[v].graph->getAPIReference(), dg.getNonHyper().getAPIReference());
	}, extendToEdges);
}

void Printer::popVertexColour() {
	printer->popVertexColour();
}

void Printer::pushEdgeColour(std::function<std::string(DG::HyperEdge) > f) {
	printer->pushEdgeColour([f](lib::DG::HyperVertex v, const lib::DG::Hyper & dg) {
		assert(dg.getGraph()[v].kind == lib::DG::HyperVertexKind::Edge);
		return f(dg.getInterfaceEdge(v));
	});
}

void Printer::popEdgeColour() {
	printer->popEdgeColour();
}

} // namespace dg
} // namespace mod
