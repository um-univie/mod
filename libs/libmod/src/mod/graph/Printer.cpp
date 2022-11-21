#include "Printer.hpp"

#include <mod/lib/Graph/IO/Write.hpp>

namespace mod::graph {

//------------------------------------------------------------------------------
// Printer
//------------------------------------------------------------------------------

Printer::Printer() : options(new lib::IO::Graph::Write::Options()) {
	options->EdgesAsBonds(true).RaiseCharges(true).RaiseIsotopes(true);
}

Printer::Printer(const Printer &that)
		: options(new lib::IO::Graph::Write::Options(*that.options)) {}

Printer &Printer::operator=(const Printer &that) {
	*options = *that.options;
	return *this;
}

Printer::~Printer() {}

const lib::IO::Graph::Write::Options &Printer::getOptions() const {
	return *options;
}

bool operator==(const Printer &a, const Printer &b) {
	return a.getOptions() == b.getOptions();
}

bool operator!=(const Printer &a, const Printer &b) {
	return a.getOptions() != b.getOptions();
}

void Printer::setMolDefault() {
	options->All().Thick(false).WithIndex(false);
}

void Printer::setReactionDefault() {
	setMolDefault();
	options->SimpleCarbons(false);
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

void Printer::setRaiseIsotopes(bool value) {
	options->RaiseIsotopes(value);
}

bool Printer::getRaiseIsotopes() const {
	return options->raiseIsotopes;
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

void Printer::setWithGraphvizCoords(bool value) {
	options->WithGraphvizCoords(value);
}

bool Printer::getWithGraphvizCoords() const {
	return options->withGraphvizCoords;
}

// ===================================================================

void Printer::setGraphvizPrefix(const std::string &prefix) {
	options->graphvizPrefix = prefix;
}

const std::string &Printer::getGraphvizPrefix() const {
	return options->graphvizPrefix;
}

} // namespace mod::graph