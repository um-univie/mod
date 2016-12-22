#include "Graph.h"

#include <mod/Config.h>
#include <mod/Function.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/Graph/DFSEncoding.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/Graph/Properties/Depiction.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/IO/FileHandle.h>
#include <mod/lib/IO/GraphWriteDetail.h>
#include <mod/lib/IO/IO.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <boost/lexical_cast.hpp>

namespace mod {
namespace lib {
namespace IO {
namespace Graph {
namespace Write {
namespace {

// returns the filename _without_ extension

const std::string &getFilePrefix(const std::size_t gId) {
	static std::map<std::size_t, std::string> cache;
	auto iter = cache.find(gId);
	if(iter == end(cache)) {
		std::string prefix = IO::getUniqueFilePrefix() + "g_" + boost::lexical_cast<std::string>(gId);
		return cache[gId] = prefix;
	} else return iter->second;
}

void escapeLabelForDot(const std::string &label, std::ostream &s) {
	for(char c : label) {
		if(c == '"') s << "\\\"";
		else if(c == '\\') s << "\\\\";
		else s << c;
	}
}

} // namespace 

void gml(const lib::Graph::LabelledGraph &gLabelled, const lib::Graph::DepictionData &depict, const std::size_t gId, bool withCoords, std::ostream &s) {
	if(!depict.getHasCoordinates() && withCoords) MOD_ABORT;
	const auto &g = get_graph(gLabelled);
	const auto &pString = get_string(gLabelled);
	s << "graph [\n";
	for(auto v : asRange(vertices(g))) {
		s << "\tnode [ id " << get(boost::vertex_index_t(), g, v) << " label \"" << pString[v] << "\"";
		if(withCoords)
			s << " vis2d [ x " << depict.getX(v, true) << " y " << depict.getY(v, true) << " ]";
		s << " ]\n";
	}
	for(auto e : asRange(edges(g))) {
		s << "\tedge [ source " << get(boost::vertex_index_t(), g, source(e, g))
				<< " target " << get(boost::vertex_index_t(), g, target(e, g))
				<< " label \"" << pString[e] << "\" ]\n";
	}
	s << "]\n";
}

std::string gml(const lib::Graph::Single &g, bool withCoords) {
	static std::set < std::pair < std::size_t, bool> > cache;
	auto iter = cache.find(std::make_pair(g.getId(), withCoords));
	std::string fileNoExt = getFilePrefix(g.getId());
	if(iter != end(cache)) return fileNoExt;
	cache.emplace(g.getId(), withCoords);
	FileHandle s(fileNoExt + ".gml");
	gml(g.getLabelledGraph(), g.getDepictionData(), g.getId(), withCoords, s);
	return s;
}

std::string dot(const lib::Graph::LabelledGraph &gLabelled, const std::size_t gId) {
	static std::set<std::size_t> cache;
	auto iter = cache.find(gId);
	std::string file = getFilePrefix(gId) + ".dot";
	if(iter != end(cache)) return file;
	cache.insert(gId);
	FileHandle s(file);
	const auto &g = get_graph(gLabelled);
	const auto &pString = get_string(gLabelled);
	{
		s << "graph g {\n";
		s << "\tnode [shape=plaintext]\n";
		s << getConfig().io.dotCoordOptions.get() << "\n";

		for(auto v : asRange(vertices(g))) {
			unsigned int id = get(boost::vertex_index_t(), g, v);
			const std::string &label = pString[v];
			s << "\t" << id << " [ label=\"";
			escapeLabelForDot(label, s);
			s << "\" ]\n";
		}

		for(auto e : asRange(edges(g))) {
			auto fromId = get(boost::vertex_index_t(), g, source(e, g));
			auto toId = get(boost::vertex_index_t(), g, target(e, g));
			const std::string &label = pString[e];
			if(label == "-" || label == "=" || label == "#") {
				s << fromId << " -- " << toId << std::endl;
				if(label != "-") s << fromId << " -- " << toId << std::endl;
				if(label == "#") s << fromId << " -- " << toId << std::endl;
			} else {
				s << fromId << " -- " << toId << " [ label=\"";
				escapeLabelForDot(label, s);
				s << "\" ]\n";
			}
		}
		s << "}\n";
	}
	return s;
}

struct CoordsCacheEntry {
	std::size_t id;
	bool collapseHydrogens;
	int rotation;
public:

	friend bool operator<(const CoordsCacheEntry &a, const CoordsCacheEntry &b) {
		return std::tie(a.id, a.collapseHydrogens, a.rotation)
				< std::tie(b.id, b.collapseHydrogens, b.rotation);
	}
};

std::string coords(const lib::Graph::LabelledGraph &gLabelled, const lib::Graph::DepictionData &depict, const std::size_t gId, bool collapseHydrogens, int rotation) {
	static std::map<CoordsCacheEntry, std::string> cache;
	auto iter = cache.find({gId, collapseHydrogens, rotation});
	if(iter != end(cache)) return iter->second;
	std::string fileNoExt = getFilePrefix(gId);
	if(!depict.getHasCoordinates()) {
		dot(gLabelled, gId);
		IO::post() << "coordsFromGV graph \"" << fileNoExt << "\"" << std::endl;
		std::string file = fileNoExt + "_coord.tex";
		cache[{gId, true, rotation}
		] = file;
		cache[{gId, false, rotation}
		] = file;
		return file;
	} else {
		const auto &g = get_graph(gLabelled);
		std::string molString = collapseHydrogens ? "_mol" : "";
		FileHandle s(getFilePrefix(gId) + molString + "_coord.tex");
		s << "% dummy\n";
		for(auto v : asRange(vertices(g))) {
			unsigned int vId = get(boost::vertex_index_t(), g, v);
			if(collapseHydrogens && Chem::isCollapsible(v, g, depict, depict)) continue;
			double x, y;
			std::tie(x, y) = pointRotation(
					depict.getX(v, !collapseHydrogens),
					depict.getY(v, !collapseHydrogens),
					rotation);
			s << "\\coordinate[overlay] (v-coord-" << vId << ") at (" << std::fixed << x << ", " << y << ") {};\n";
		}
		std::string file = s;
		cache[{gId, collapseHydrogens, rotation}
		] = file;
		return file;
	}
}

std::pair<std::string, std::string>
tikz(const lib::Graph::LabelledGraph &gLabelled, const lib::Graph::DepictionData &depict, const std::size_t gId, const Options &options) {
	// a lot of the depiction rules are from Open Babel
	static std::set<std::pair<std::size_t, std::string> > cache;
	std::string strOptions = options;
	std::string file = getFilePrefix(gId) + "_" + strOptions + ".tex";
	std::string fileCoordsExt = coords(gLabelled, depict, gId, options.collapseHydrogens, options.rotation);
	auto iter = cache.find(std::make_pair(gId, strOptions));
	if(iter != end(cache)) return std::make_pair(file, fileCoordsExt);
	cache.insert(std::make_pair(gId, strOptions));

	FileHandle s(file);
	tikz(s, options, get_graph(gLabelled), depict, fileCoordsExt);
	return std::make_pair(file, fileCoordsExt);
}

std::string pdf(const lib::Graph::LabelledGraph &gLabelled, const lib::Graph::DepictionData &depict, const std::size_t gId, const Options &options) {
	{ // user-specified depiction
		static std::map<std::size_t, std::string> userCache;
		auto iter = userCache.find(gId);
		if(iter != end(userCache)) return iter->second;
		auto image = depict.getImage();
		if(image) {
			std::string imageNoExt = (*image)();
			if(imageNoExt.empty()) {
				IO::log() << "User-specified depiction file for graph with id " << gId << " can not be empty." << std::endl;
				throw 0;
			}
			std::string cmd = depict.getImageCommand();
			if(!cmd.empty()) IO::post() << cmd << std::endl;
			std::string image = imageNoExt + ".pdf";
			userCache[gId] = image;
			return image;
		}
	}
	// auto-generated depiction
	static std::set<std::pair<std::size_t, std::string> > cache;
	std::string strOptions = options;
	std::string fileNoExt = getFilePrefix(gId) + "_" + strOptions;
	std::string file = fileNoExt + ".pdf";
	auto iter = cache.find(std::make_pair(gId, strOptions));
	if(iter != end(cache)) return file;
	cache.insert(std::make_pair(gId, strOptions));
	auto tikzFiles = tikz(gLabelled, depict, gId, options);
	std::string fileCoordsNoExt = tikzFiles.second.substr(0, tikzFiles.second.length() - 4);
	IO::post() << "compileTikz \"" << fileNoExt << "\" \"" << fileCoordsNoExt << "\"" << std::endl;
	return file;
}

std::string svg(const lib::Graph::LabelledGraph &gLabelled, const lib::Graph::DepictionData &depict, const std::size_t gId, const Options &options) {
	static std::set<std::pair<std::size_t, std::string> > cache;
	std::string strOptions = options;
	std::string fileNoExt = getFilePrefix(gId) + "_" + strOptions;
	std::string file = fileNoExt + ".svg";
	auto iter = cache.find(std::make_pair(gId, strOptions));
	if(iter != end(cache)) return file;
	cache.insert(std::make_pair(gId, strOptions));
	std::string pdfFileNoExt = pdf(gLabelled, depict, gId, options);
	pdfFileNoExt.erase(end(pdfFileNoExt) - 4, end(pdfFileNoExt));
	IO::post() << "pdfToSvg \"" << pdfFileNoExt << "\" \"" << fileNoExt << "\"" << std::endl;
	return file;
}

std::pair<std::string, std::string> summary(const lib::Graph::Single &g, const Options &first, const Options &second) {
	std::string graphLike = pdf(g, first);
	std::string molLike = first == second ? "" : pdf(g, second);
	IO::post() << "summaryGraph \"" << g.getName() << "\" \""
			<< std::string(begin(graphLike), end(graphLike) - 4) << "\" \"";
	if(!molLike.empty())
		IO::post() << std::string(begin(molLike), end(molLike) - 4);
	IO::post() << "\"\n";
	if(molLike.empty())
		return std::make_pair(graphLike, graphLike);
	else
		return std::make_pair(graphLike, molLike);
}

//------------------------------------------------------------------------------
// Simplified interface for lib::Graph::Single
//------------------------------------------------------------------------------

void gml(const lib::Graph::Single &g, bool withCoords, std::ostream &s) {
	gml(g.getLabelledGraph(), g.getDepictionData(), g.getId(), withCoords, s);
}

std::string dot(const lib::Graph::Single &g) {
	return dot(g.getLabelledGraph(), g.getId());
}

std::string coords(const lib::Graph::Single &g, bool collapseHydrogens, int rotation) {
	return coords(g.getLabelledGraph(), g.getDepictionData(), g.getId(), collapseHydrogens, rotation);
}

std::pair<std::string, std::string> tikz(const lib::Graph::Single &g, const Options &options) {
	return tikz(g.getLabelledGraph(), g.getDepictionData(), g.getId(), options);
}

std::string pdf(const lib::Graph::Single &g, const Options &options) {
	return pdf(g.getLabelledGraph(), g.getDepictionData(), g.getId(), options);
}

std::string svg(const lib::Graph::Single &g, const Options &options) {
	return svg(g.getLabelledGraph(), g.getDepictionData(), g.getId(), options);
}

} // namespace Write
} // namespace Graph
} // namespace IO
} // namespace lib
} // namespace mod
