#include "Misc.hpp"

#include <mod/BuildConfig.hpp>
#include <mod/graph/Graph.hpp>
#include <mod/lib/DG/Hyper.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/Graph/Properties/Molecule.hpp>
#include <mod/lib/IO/IO.hpp>
#include <mod/lib/IO/JsonUtils.hpp>
#include <mod/lib/IO/Stereo.hpp>
#include <mod/lib/Random.hpp>
#include <mod/lib/Stereo/GeometryGraph.hpp>

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/lexical_cast.hpp>

#include <fstream>
#include <iomanip>
#include <vector>

namespace mod {

std::string version() {
	return MOD_VERSION;
}

void rngReseed(unsigned int seed) {
	lib::Random::getInstance().reseed(seed);
}

double rngUniformReal() {
	return std::uniform_real_distribution<double>(0, 1)(lib::getRng());
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

std::string makeUniqueFilePrefix() {
    return lib::IO::getUniqueFilePrefix();
}

void showDump(const std::string &file) {
	boost::iostreams::mapped_file_source ifs(file);
	std::vector<std::uint8_t> data(ifs.begin(), ifs.end());
	std::ostringstream err;
	auto jOpt = lib::IO::readJson(data, err);
	if(!jOpt)
		throw InputError("Error showing dump: " + err.str());
	lib::IO::log() << std::setw(3) << *jOpt << std::endl;
}

void printGeometryGraph() {
	lib::IO::Stereo::Write::summary(lib::Stereo::getGeometryGraph());
}

} // namespace mod
