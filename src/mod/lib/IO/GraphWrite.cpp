#include "Graph.h"

#include <mod/Config.h>
#include <mod/Function.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/Graph/DFSEncoding.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/Graph/Properties/Depiction.h>
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

const std::string &getFilePrefix(const lib::Graph::Single &g) {
	static std::map<unsigned int, std::string> cache;
	auto iter = cache.find(g.getId());
	if(iter == end(cache)) {
		std::string prefix = IO::getUniqueFilePrefix() + "g_" + boost::lexical_cast<std::string>(g.getId());
		return cache[g.getId()] = prefix;
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

void gml(const lib::Graph::Single &g, bool withCoords, std::ostream &s) {
	const auto &depict = g.getDepictionData();
	if(!depict.getHasCoordinates() && withCoords) MOD_ABORT;
	using Vertex = lib::Graph::Vertex;
	using Edge = lib::Graph::Edge;
	s << "graph [\n";
	for(Vertex v : asRange(vertices(g.getGraph()))) {
		s << "\tnode [ id " << get(boost::vertex_index_t(), g.getGraph(), v) << " label \"" << g.getStringState()[v] << "\"";
		if(withCoords)
			s << " vis2d [ x " << depict.getX(v, true) << " y " << depict.getY(v, true) << " ]";
		s << " ]\n";
	}
	for(Edge e : asRange(edges(g.getGraph())))
		s << "\tedge [ source " << get(boost::vertex_index_t(), g.getGraph(), source(e, g.getGraph()))
		<< " target " << get(boost::vertex_index_t(), g.getGraph(), target(e, g.getGraph()))
		<< " label \"" << g.getStringState()[e] << "\" ]\n";
	s << "]\n";
}

std::string gml(const lib::Graph::Single &g, bool withCoords) {
	static std::set < std::pair < std::size_t, bool> > cache;
	auto iter = cache.find(std::make_pair(g.getId(), withCoords));
	std::string fileNoExt = getFilePrefix(g);
	if(iter != end(cache)) return fileNoExt;
	cache.emplace(g.getId(), withCoords);
	FileHandle s(fileNoExt + ".gml");
	gml(g, withCoords, s);
	return s;
}

std::string dot(const lib::Graph::Single &g) {
	static std::set<unsigned int> cache;
	auto iter = cache.find(g.getId());
	std::string file = getFilePrefix(g) + ".dot";
	if(iter != end(cache)) return file;
	cache.insert(g.getId());
	FileHandle s(file);
	const lib::Graph::Single &gNonBoost = g;
	const auto &labelState = g.getStringState();
	{
		using Vertex = lib::Graph::Vertex;
		using Edge = lib::Graph::Edge;
		s << "graph g {" << std::endl;
		s << "\tnode [shape=plaintext]" << std::endl;
		s << getConfig().io.dotCoordOptions.get() << std::endl;
		const lib::Graph::GraphType &g = gNonBoost.getGraph();

		for(Vertex v : asRange(vertices(g))) {
			unsigned int id = get(boost::vertex_index_t(), g, v);
			const std::string &label = labelState[v];
			s << "\t" << id << " [ label=\"";
			escapeLabelForDot(label, s);
			s << "\" ]" << std::endl;
		}

		for(Edge e : asRange(edges(g))) {
			unsigned int fromId = get(boost::vertex_index_t(), g, source(e, g));
			unsigned int toId = get(boost::vertex_index_t(), g, target(e, g));
			const std::string &label = labelState[e];
			if(label == "-" || label == "=" || label == "#") {
				s << fromId << " -- " << toId << std::endl;
				if(label != "-") s << fromId << " -- " << toId << std::endl;
				if(label == "#") s << fromId << " -- " << toId << std::endl;
			} else {
				s << fromId << " -- " << toId << " [ label=\"";
				escapeLabelForDot(label, s);
				s << "\" ]" << std::endl;
			}
		}
		s << "}" << std::endl;
	}
	return s;
}

std::string coords(const lib::Graph::Single &g, bool collapseHydrogens) {
	static std::map < std::pair<unsigned int, bool>, std::string> cache;
	auto iter = cache.find(std::make_pair(g.getId(), collapseHydrogens));
	if(iter != end(cache)) return iter->second;
	std::string fileNoExt = getFilePrefix(g);
	const auto &depict = g.getDepictionData();
	if(!depict.getHasCoordinates()) {
		dot(g);
		IO::post() << "coordsFromGV graph \"" << fileNoExt << "\"" << std::endl;
		std::string file = fileNoExt + ".coord.tex";
		cache[std::make_pair(g.getId(), true)] = file;
		cache[std::make_pair(g.getId(), false)] = file;
		return file;
	} else {
		using Vertex = lib::Graph::Vertex;
		using Edge = lib::Graph::Edge;
		const lib::Graph::Single &gNonBoost = g;
		std::string molString = collapseHydrogens ? ".mol" : "";
		FileHandle s(getFilePrefix(g) + molString + ".coord.tex");
		s << "% dummy" << std::endl;
		const lib::Graph::GraphType &g = gNonBoost.getGraph();
		for(Vertex v : asRange(vertices(g))) {
			unsigned int vId = get(boost::vertex_index_t(), g, v);
			if(collapseHydrogens && Chem::isCollapsible(v, g, depict, depict)) continue;
			double x = depict.getX(v, !collapseHydrogens);
			double y = depict.getY(v, !collapseHydrogens);
			s << "\\coordinate[overlay] (v-coord-" << vId << ") at (" << std::fixed << x << ", " << y << ") {};" << std::endl;
		}
		std::string file = s;
		cache[std::make_pair(gNonBoost.getId(), collapseHydrogens)] = file;
		return file;
	}
}

std::pair<std::string, std::string> tikz(const lib::Graph::Single &g, const Options &options) {
	// a lot of the depiction rules are from Open Babel
	static std::set<std::pair<unsigned int, std::string> > cache;
	std::string strOptions = options;
	std::string file = getFilePrefix(g) + "." + strOptions + ".tex";
	std::string fileCoordsExt = coords(g, options.collapseHydrogens);
	auto iter = cache.find(std::make_pair(g.getId(), strOptions));
	if(iter != end(cache)) return std::make_pair(file, fileCoordsExt);
	cache.insert(std::make_pair(g.getId(), strOptions));

	const auto &depict = g.getDepictionData();
	FileHandle s(file);
	tikz(s, options, g.getGraph(), depict, fileCoordsExt);
	return std::make_pair(file, fileCoordsExt);
}

std::string pdf(const lib::Graph::Single &g, const Options &options) {
	{ // user-specified depiction
		static std::map<unsigned int, std::string> userCache;
		auto iter = userCache.find(g.getId());
		if(iter != end(userCache)) return iter->second;
		auto image = g.getDepictionData().getImage();
		if(image) {
			std::string imageNoExt = (*image)();
			if(imageNoExt.empty()) {
				IO::log() << "User-specified depiction file for '" << g.getName() << "' (id=" << g.getId() << ") can not be empty." << std::endl;
				throw 0;
			}
			std::string cmd = g.getDepictionData().getImageCommand();
			if(!cmd.empty()) IO::post() << cmd << std::endl;
			std::string image = imageNoExt + ".pdf";
			userCache[g.getId()] = image;
			return image;
		}
	}
	// auto-generated depiction
	static std::set<std::pair<unsigned int, std::string> > cache;
	std::string strOptions = options;
	std::string fileNoExt = getFilePrefix(g) + "." + strOptions;
	std::string file = fileNoExt + ".pdf";
	auto iter = cache.find(std::make_pair(g.getId(), strOptions));
	if(iter != end(cache)) return file;
	cache.insert(std::make_pair(g.getId(), strOptions));
	auto tikzFiles = tikz(g, options);
	std::string fileCoordsNoExt = tikzFiles.second.substr(0, tikzFiles.second.length() - 4);
	IO::post() << "compileTikz \"" << fileNoExt << "\" \"" << fileCoordsNoExt << "\"" << std::endl;
	return file;
}

std::string svg(const lib::Graph::Single &g, const Options &options) {
	static std::set<std::pair<unsigned int, std::string> > cache;
	std::string strOptions = options;
	std::string fileNoExt = getFilePrefix(g) + "." + strOptions;
	std::string file = fileNoExt + ".svg";
	auto iter = cache.find(std::make_pair(g.getId(), strOptions));
	if(iter != end(cache)) return file;
	cache.insert(std::make_pair(g.getId(), strOptions));
	std::string pdfFileNoExt = pdf(g, options);
	pdfFileNoExt.erase(end(pdfFileNoExt) - 4, end(pdfFileNoExt));
	IO::post() << "pdfToSvg \"" << pdfFileNoExt << "\" \"" << fileNoExt << "\"" << std::endl;
	return file;
}

std::pair<std::string, std::string> summary(const lib::Graph::Single &g, const Options &first, const Options &second) {
	std::string graphLike = pdf(g, first);
	std::string molLike = pdf(g, second);
	IO::post() << "summaryGraph \"" << g.getName() << "\" \""
			<< std::string(begin(graphLike), end(graphLike) - 4) << "\" \""
			<< std::string(begin(molLike), end(molLike) - 4) << "\"" << std::endl;
	return std::make_pair(graphLike, molLike);
}

} // namespace Write
} // namespace Graph
} // namespace IO
} // namespace lib
} // namespace mod
