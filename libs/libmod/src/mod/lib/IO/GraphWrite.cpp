#include "Graph.hpp"

#include <mod/Config.hpp>
#include <mod/Function.hpp>
#include <mod/lib/Chem/MoleculeUtil.hpp>
#include <mod/lib/Graph/DFSEncoding.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/Graph/Properties/Depiction.hpp>
#include <mod/lib/Graph/Properties/String.hpp>
#include <mod/lib/Graph/Properties/Term.hpp>
#include <mod/lib/IO/FileHandle.hpp>
#include <mod/lib/IO/GraphWriteDetail.hpp>
#include <mod/lib/IO/IO.hpp>
#include <mod/lib/IO/Term.hpp>
#include <mod/lib/Term/WAM.hpp>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <boost/lexical_cast.hpp>

namespace mod {
namespace lib {
namespace IO {
namespace Graph {
namespace Write {
namespace {

// returns the filename _without_ extension

std::string getFilePrefix(const std::size_t gId, bool useCache) {
	static std::map<std::size_t, std::string> cache;
	auto iter = cache.find(gId);
	if(!useCache || iter == end(cache)) {
		std::string prefix = IO::getUniqueFilePrefix() + "g_" + boost::lexical_cast<std::string>(gId);
		if(useCache) cache[gId] = prefix;
		return prefix;
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

EdgeFake3DType invertEdgeFake3DType(EdgeFake3DType t) {
	switch(t) {
	case EdgeFake3DType::None: return t;
	case EdgeFake3DType::WedgeSL: return EdgeFake3DType::WedgeLS;
	case EdgeFake3DType::WedgeLS: return EdgeFake3DType::WedgeSL;
	case EdgeFake3DType::HashSL: return EdgeFake3DType::HashLS;
	case EdgeFake3DType::HashLS: return EdgeFake3DType::HashSL;
	}
	MOD_ABORT; // no, GCC, shh
}

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
	std::string fileNoExt = getFilePrefix(g.getId(), true);
	if(iter != end(cache)) return fileNoExt;
	cache.emplace(g.getId(), withCoords);
	FileHandle s(fileNoExt + ".gml");
	gml(g.getLabelledGraph(), g.getDepictionData(), g.getId(), withCoords, s);
	return s;
}

std::string dot(const lib::Graph::LabelledGraph &gLabelled, const std::size_t gId) {
	static std::set<std::size_t> cache;
	auto iter = cache.find(gId);
	std::string file = getFilePrefix(gId, true) + ".dot";
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
	bool mirror;
public:

	friend bool operator<(const CoordsCacheEntry &a, const CoordsCacheEntry &b) {
		return std::tie(a.id, a.collapseHydrogens, a.rotation, a.mirror)
				< std::tie(b.id, b.collapseHydrogens, b.rotation, b.mirror);
	}
};

std::string coords(const lib::Graph::LabelledGraph &gLabelled, const lib::Graph::DepictionData &depict, const std::size_t gId, const Options &options, bool asInline) {
	static std::map<CoordsCacheEntry, std::string> cache;
	if(!asInline) {
		const auto iter = cache.find({gId, options.collapseHydrogens, options.rotation, options.mirror});
		if(iter != end(cache)) return iter->second;
	}
	const std::string fileNoExt = getFilePrefix(gId, !asInline);
	if(!depict.getHasCoordinates()) {
		dot(gLabelled, gId);
		IO::post() << "coordsFromGV graph \"" << fileNoExt << "\"\n";
		const std::string file = fileNoExt + "_coord.tex";
		if(!asInline) {
			for(bool collapse :{true, false}) {
				for(bool mirror :{true, false}) {
					cache[{gId, collapse, options.rotation, mirror}
					] = file;
				}
			}
		}
		return file;
	} else {
		const auto &g = get_graph(gLabelled);
		const std::string molString = options.collapseHydrogens ? "_mol" : "";
		std::string f = getFilePrefix(gId, !asInline) + molString;
		if(asInline) f += "i";
		FileHandle s(f + "_coord.tex");
		s << "% dummy\n";
		for(const auto v : asRange(vertices(g))) {
			const auto vId = get(boost::vertex_index_t(), g, v);
			if(options.collapseHydrogens && Chem::isCollapsible(v, g, depict, depict, [&depict](const auto v) {
					return depict.hasImportantStereo(v);
				})) continue;
			double x, y;
			std::tie(x, y) = pointTransform(
					depict.getX(v, !options.collapseHydrogens),
					depict.getY(v, !options.collapseHydrogens),
					options.rotation, options.mirror);
			s << "\\coordinate[overlay] (\\modIdPrefix v-coord-" << vId << ") at (" << std::fixed << x << ", " << y << ") {};\n";
		}
		const std::string file = s;
		if(!asInline) {
			cache[{gId, options.collapseHydrogens, options.rotation, options.mirror}
			] = file;
		}
		return file;
	}
}

std::pair<std::string, std::string>
tikz(const lib::Graph::LabelledGraph &gLabelled, const lib::Graph::DepictionData &depict, const std::size_t gId, const Options &options,
		bool asInline, const std::string &idPrefix) {
	static std::set<std::pair<std::size_t, std::string> > cache;
	std::string strOptions = options.getStringEncoding();
	std::string file = getFilePrefix(gId, !asInline) + "_" + strOptions;
	if(asInline) file += "i";
	file += ".tex";
	std::string fileCoordsExt = coords(gLabelled, depict, gId, options, asInline);
	if(!asInline) {
		const auto iter = cache.find(std::make_pair(gId, strOptions));
		if(iter != end(cache)) return std::make_pair(file, fileCoordsExt);
		cache.insert(std::make_pair(gId, strOptions));
	}
	FileHandle s(file);
	tikz(s, options, get_graph(gLabelled), depict, fileCoordsExt, asInline, idPrefix);
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
	std::string strOptions = options.getStringEncoding();
	std::string fileNoExt = getFilePrefix(gId, true) + "_" + strOptions;
	std::string file = fileNoExt + ".pdf";
	auto iter = cache.find(std::make_pair(gId, strOptions));
	if(iter != end(cache)) return file;
	cache.insert(std::make_pair(gId, strOptions));
	auto tikzFiles = tikz(gLabelled, depict, gId, options, false, "");
	std::string fileCoordsNoExt = tikzFiles.second.substr(0, tikzFiles.second.length() - 4);
	IO::post() << "compileTikz \"" << fileNoExt << "\" \"" << fileCoordsNoExt << "\"" << std::endl;
	return file;
}

std::string svg(const lib::Graph::LabelledGraph &gLabelled, const lib::Graph::DepictionData &depict, const std::size_t gId, const Options &options) {
	static std::set<std::pair<std::size_t, std::string> > cache;
	std::string strOptions = options.getStringEncoding();
	std::string fileNoExt = getFilePrefix(gId, true) + "_" + strOptions;
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

void termState(const lib::Graph::Single &g) {
	using Vertex = lib::Graph::Vertex;
	using Edge = lib::Graph::Edge;
	using namespace lib::Term;
	IO::post() << "summarySubsection \"Term State for " << g.getName() << "\"" << std::endl;
	FileHandle s(getUniqueFilePrefix() + "termState.tex");
	s << "\\begin{verbatim}\n";
	const auto &termState = get_term(g.getLabelledGraph());
	if(isValid(termState)) {
		std::unordered_map<Address, std::set<Vertex> > addrToVertex;
		std::unordered_map<Address, std::set<Edge> > addrToEdge;
		for(Vertex v : asRange(vertices(g.getGraph()))) {
			Address a{AddressType::Heap, termState[v]};
			addrToVertex[a].insert(v);
		}
		for(Edge e : asRange(edges(g.getGraph()))) {
			Address a{AddressType::Heap, termState[e]};
			addrToEdge[a].insert(e);
		}
		lib::IO::Term::Write::wam(getMachine(termState), lib::Term::getStrings(), s, [&](Address addr, std::ostream & s) {
			s << "        ";
			bool first = true;
			for(auto v : addrToVertex[addr]) {
				if(!first) s << ", ";
						first = false;
						s << "v" << get(boost::vertex_index_t(), g.getGraph(), v);
				}
			for(auto e : addrToEdge[addr]) {
				if(!first) s << ", ";
						first = false;
						s << "e("
						<< get(boost::vertex_index_t(), g.getGraph(), source(e, g.getGraph()))
						<< ", "
						<< get(boost::vertex_index_t(), g.getGraph(), target(e, g.getGraph()))
						<< ")";
				}
		});
	} else {
		std::string msg = "Parsing failed for graph '" + g.getName() + "'. " + termState.getParsingError();
		throw TermParsingError(std::move(msg));
	}
	s << "\\end{verbatim}\n";
	IO::post() << "summaryInput \"" << std::string(s) << "\"" << std::endl;
}

//------------------------------------------------------------------------------
// Simplified interface for lib::Graph::Single
//------------------------------------------------------------------------------

void gml(const lib::Graph::Single &g, bool withCoords, std::ostream &s) {
	gml(g.getLabelledGraph(), g.getDepictionData(), g.getId(), withCoords, s);
}

std::string tikz(const lib::Graph::Single &g, const Options &options, bool asInline, const std::string &idPrefix) {
	auto res = tikz(g.getLabelledGraph(), g.getDepictionData(), g.getId(), options, asInline, idPrefix);
	return res.first;
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
