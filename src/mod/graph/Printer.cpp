#include "Printer.h"

#include <mod/lib/IO/Graph.h>

namespace mod {
namespace graph {

//------------------------------------------------------------------------------
// Printer
//------------------------------------------------------------------------------

Printer::Printer() : options(new lib::IO::Graph::Write::Options()) {
	options->EdgesAsBonds(true).RaiseCharges(true).RaiseIsotopes(true);
}

Printer::Printer(const Printer &that)
: options(new lib::IO::Graph::Write::Options(*that.options)) { }

Printer &Printer::operator=(const Printer &that) {
	*options = *that.options;
	return *this;
}

Printer::~Printer() { }

const lib::IO::Graph::Write::Options &Printer::getOptions() const {
	return *options;
}

void Printer::setMolDefault() {
	options->All().Thick(false).WithIndex(false);
}

void Printer::setReactionDefault() {
	options->All().Thick(false).WithIndex(false).SimpleCarbons(false);
}

void Printer::disableAll() {
	options->Non();
}

void Printer::enableAll() {
	options->All();
}

void Printer::setEdgesAsBonds(bool value) {
	options->EdgesAsBonds(value);
}

bool Printer::getEdgesAsBonds() const {
	return options->edgesAsBonds;
}

void Printer::setCollapseHydrogens(bool value) {
	options->CollapseHydrogens(value);
}

bool Printer::getCollapseHydrogens() const {
	return options->collapseHydrogens;
}

void Printer::setRaiseCharges(bool value) {
	options->RaiseCharges(value);
}

bool Printer::getRaiseCharges() const {
	return options->raiseCharges;
}

void Printer::setSimpleCarbons(bool value) {
	options->SimpleCarbons(value);
}

bool Printer::getSimpleCarbons() const {
	return options->simpleCarbons;
}

void Printer::setThick(bool value) {
	options->Thick(value);
}

bool Printer::getThick() const {
	return options->thick;
}

void Printer::setWithColour(bool value) {
	options->WithColour(value);
}

bool Printer::getWithColour() const {
	return options->withColour;
}

void Printer::setWithIndex(bool value) {
	options->WithIndex(value);
}

bool Printer::getWithIndex() const {
	return options->withIndex;
}

void Printer::setWithTexttt(bool value) {
	options->WithTexttt(value);
}

bool Printer::getWithTexttt() const {
	return options->withTexttt;
}

void Printer::setWithRawStereo(bool value) {
	options->WithRawStereo(value);
}

bool Printer::getWithRawStereo() const {
	return options->withRawStereo;
}

void Printer::setWithPrettyStereo(bool value) {
	options->WithPrettyStereo(value);
}

bool Printer::getWithPrettyStereo() const {
	return options->withPrettyStereo;
}

void Printer::setRotation(int degrees) {
	options->Rotation(degrees);
}

int Printer::getRotation() const {
	return options->rotation;
}

void Printer::setMirror(bool value) {
	options->Mirror(value);
}

bool Printer::getMirror() const {
	return options->mirror;
}

} // namespace graph
} // namespace mod