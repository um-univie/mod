#include "Misc.hpp"

#include <mod/BuildConfig.hpp>
#include <mod/dg/DG.hpp>
#include <mod/graph/Graph.hpp>
#include <mod/lib/DG/Hyper.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/Graph/Properties/Molecule.hpp>
#include <mod/lib/IO/IO.hpp>
#include <mod/lib/IO/Stereo.hpp>
#include <mod/lib/Random.hpp>
#include <mod/lib/Stereo/GeometryGraph.hpp>

#include <boost/lexical_cast.hpp>

#include <fstream>
#include <vector>

namespace mod {

std::string version() {
	return MOD_VERSION;
}

void rngReseed(unsigned int seed) {
	lib::Random::getInstance().reseed(seed);
}

void post(const std::string &text) {
	lib::IO::post() << text << std::endl;
}

void postChapter(const std::string &chapterTitle) {
	lib::IO::post() << "summaryChapter \"" << chapterTitle << "\"" << std::endl;
}

void postSection(const std::string &sectionTitle) {
	lib::IO::post() << "summarySection \"" << sectionTitle << "\"" << std::endl;
}

void printGeometryGraph() {
	lib::IO::Stereo::Write::summary(lib::Stereo::getGeometryGraph());
}

} // namespace mod
