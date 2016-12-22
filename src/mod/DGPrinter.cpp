#include "DGPrinter.h"

#include <mod/DG.h>
#include <mod/Error.h>
#include <mod/GraphPrinter.h>
#include <mod/lib/DG/Hyper.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/IO/DG.h>

#include <boost/lexical_cast.hpp>

//#include <functional>
//#include <fstream>
//#include <sstream>

namespace mod {

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

DGPrinter::DGPrinter() : graphPrinter(std::make_unique<GraphPrinter>()), printer(std::make_unique<lib::IO::DG::Write::Printer>()) {
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