#include "WriteConfiguration.hpp"

#include <mod/lib/Stereo/Configuration/Any.hpp>
#include <mod/lib/Stereo/Configuration/Linear.hpp>
#include <mod/lib/Stereo/Configuration/TrigonalPlanar.hpp>
#include <mod/lib/Stereo/Configuration/Tetrahedral.hpp>

#include <ostream>

namespace mod::lib::Stereo::Write {

} // namespace mod::lib::Stereo::Write
namespace mod::lib::Stereo {
namespace {

#define MOD_anyPrintCoords()                                                    \
Configuration::printCoords(s, vIds);                                          \
if(vIds.size() == 1) return;                                                  \
auto centerId = vIds.back();                                                  \
double angle = 360.0 / (vIds.size() - 1);                                     \
for(std::size_t i = 0; i < vIds.size() - 1; i++)                              \
printSateliteCoord(s, centerId, angle * i + 90, vIds[i]);

void printSateliteCoord(std::ostream &s, std::size_t centerId, double angle, std::size_t id) {
	s << "\\coordinate[overlay, at=($(\\modIdPrefix v-coord-" << centerId << ")+(" << angle << ":1)";
	s << "$)] (\\modIdPrefix v-coord-" << id << ") {};\n";
}

} // namespace

// Configuration
//------------------------------------------------------------------------------

IO::Graph::Write::EdgeFake3DType Configuration::getEdgeDepiction(std::size_t i) const {
	return IO::Graph::Write::EdgeFake3DType::None;
}

void Configuration::printCoords(std::ostream &s, const std::vector<std::size_t> &vIds) const {
	const std::string &name = getGeometryGraph().getGraph()[getGeometryVertex()].name;
	s << "\\coordinate[overlay] (\\modIdPrefix v-coord-" << vIds.back() << ") {};\n";
	s << "\\node[at=($(\\modIdPrefix v-coord-" << vIds.back() << ")+(-90:1)+(-90:12pt)$)] {" << name << "};\n";
}

std::string Configuration::getEdgeAnnotation(std::size_t i) const {
	return "";
}

// Any
//------------------------------------------------------------------------------

void Any::printCoords(std::ostream &s, const std::vector<std::size_t> &vIds) const {
	MOD_anyPrintCoords();
}

// Linear
//------------------------------------------------------------------------------

void Linear::printCoords(std::ostream &s, const std::vector<std::size_t> &vIds) const {
	Configuration::printCoords(s, vIds);
	printSateliteCoord(s, vIds.back(), 180, vIds[0]);
	printSateliteCoord(s, vIds.back(), 0, vIds[1]);
}

// TrigonalPlanar
//------------------------------------------------------------------------------

void TrigonalPlanar::printCoords(std::ostream &s, const std::vector<std::size_t> &vIds) const {
	Configuration::printCoords(s, vIds);
	printSateliteCoord(s, vIds.back(), 180, vIds[0]);
	printSateliteCoord(s, vIds.back(), 300, vIds[1]);
	printSateliteCoord(s, vIds.back(), 60, vIds[2]);
}

std::string TrigonalPlanar::getEdgeAnnotation(std::size_t i) const {
	if(fixed) return "";
	else return "?";
}

// Tetrahedral
//------------------------------------------------------------------------------

IO::Graph::Write::EdgeFake3DType Tetrahedral::getEdgeDepiction(std::size_t i) const {
	switch(i) {
	case 0:
	case 1:
		return lib::IO::Graph::Write::EdgeFake3DType::None;
	case 2:
		return lib::IO::Graph::Write::EdgeFake3DType::WedgeSL;
	case 3:
		return lib::IO::Graph::Write::EdgeFake3DType::HashSL;
	default:
		__builtin_unreachable();
	}
}

void Tetrahedral::printCoords(std::ostream &s, const std::vector<std::size_t> &vIds) const {
	Configuration::printCoords(s, vIds);
	printSateliteCoord(s, vIds.back(), 90, vIds[0]);
	printSateliteCoord(s, vIds.back(), 210, vIds[1]);
	printSateliteCoord(s, vIds.back(), -60, vIds[2]);
	printSateliteCoord(s, vIds.back(), -10, vIds[3]);
}

std::string Tetrahedral::getEdgeAnnotation(std::size_t i) const {
	if(fixed) return "";
	else return "?";
}

} // namespace mod::lib::Stereo