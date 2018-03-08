#include "Misc.h"

#include <mod/BuildConfig.h>
#include <mod/dg/DG.h>
#include <mod/graph/Graph.h>
#include <mod/lib/DG/Hyper.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/IO/Stereo.h>
#include <mod/lib/Stereo/GeometryGraph.h>

#include <boost/lexical_cast.hpp>

#include <fstream>
#include <vector>

namespace mod {
namespace {

std::vector<std::string> filePrefixes;

} // namespace 

std::string prefixFilename(const std::string &name) {
	if(name.empty() || name[0] == '/') return name;
	std::string prefixed;
	for(const std::string &str : filePrefixes) {
		if(!str.empty() && str[0] == '/') prefixed = str;
		else prefixed += str;
	}
	prefixed += name;
	return prefixed;
}

void pushFilePrefix(const std::string &prefix) {
	filePrefixes.push_back(prefix);
}

void popFilePrefix() {
	if(filePrefixes.empty()) {
		lib::IO::log() << "popFilePrefix failed; the stack is empty" << std::endl;
		std::exit(1);
	} else filePrefixes.pop_back();
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

std::string version() {
	return MOD_VERSION;
}

} // namespace mod