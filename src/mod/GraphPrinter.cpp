#include "GraphPrinter.h"

#include <mod/lib/IO/Graph.h>

namespace mod {

//------------------------------------------------------------------------------
// GraphPrinter
//------------------------------------------------------------------------------

GraphPrinter::GraphPrinter() : options(new lib::IO::Graph::Write::Options()) {
	options->EdgesAsBonds(true).RaiseCharges(true);
}

GraphPrinter::~GraphPrinter() { }

const lib::IO::Graph::Write::Options &GraphPrinter::getOptions() const {
	return *options;
}

void GraphPrinter::setMolDefault() {
	options->All().Thick(false).WithIndex(false);
}

void GraphPrinter::setReactionDefault() {
	options->All().Thick(false).WithIndex(false).SimpleCarbons(false);
}

void GraphPrinter::disableAll() {
	options->Non();
}

void GraphPrinter::enableAll() {
	options->All();
}

void GraphPrinter::setEdgesAsBonds(bool value) {
	options->EdgesAsBonds(value);
}

bool GraphPrinter::getEdgesAsBonds() const {
	return options->edgesAsBonds;
}

void GraphPrinter::setCollapseHydrogens(bool value) {
	options->CollapseHydrogens(value);
}

bool GraphPrinter::getCollapseHydrogens() const {
	return options->collapseHydrogens;
}

void GraphPrinter::setRaiseCharges(bool value) {
	options->RaiseCharges(value);
}

bool GraphPrinter::getRaiseCharges() const {
	return options->raiseCharges;
}

void GraphPrinter::setSimpleCarbons(bool value) {
	options->SimpleCarbons(value);
}

bool GraphPrinter::getSimpleCarbons() const {
	return options->simpleCarbons;
}

void GraphPrinter::setThick(bool value) {
	options->Thick(value);
}

bool GraphPrinter::getThick() const {
	return options->thick;
}

void GraphPrinter::setWithColour(bool value) {
	options->WithColour(value);
}

bool GraphPrinter::getWithColour() const {
	return options->withColour;
}

void GraphPrinter::setWithIndex(bool value) {
	options->WithIndex(value);
}

bool GraphPrinter::getWithIndex() const {
	return options->withIndex;
}

void GraphPrinter::setWithTexttt(bool value) {
	options->WithTexttt(value);
}

bool GraphPrinter::getWithTexttt() const {
	return options->withTexttt;
}

void GraphPrinter::setRotation(int degrees) {
	options->Rotation(degrees);
}

int GraphPrinter::getRotation() const {
	return options->rotation;
}

} // namespace mod