#include "Stereo.hpp"

#include <mod/Post.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/Graph/Properties/Depiction.hpp>
#include <mod/lib/Graph/Properties/Stereo.hpp>
#include <mod/lib/IO/GraphWriteDetail.hpp>
#include <mod/lib/Rules/Real.hpp>
#include <mod/lib/Rules/Properties/Depiction.hpp>
#include <mod/lib/Rules/Properties/Stereo.hpp>

#include <mod/lib/Stereo/Configuration/Any.hpp>
#include <mod/lib/Stereo/Configuration/Linear.hpp>
#include <mod/lib/Stereo/Configuration/TrigonalPlanar.hpp>
#include <mod/lib/Stereo/Configuration/Tetrahedral.hpp>

#include <boost/lexical_cast.hpp>

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
	s << "\\coordinate[overlay, at=($(v-coord-" << centerId << ")+(" << angle << ":1)";
	s << "$)] (v-coord-" << id << ") {};" << std::endl;
}

} // namespace

// Configuration
//------------------------------------------------------------------------------

IO::Graph::Write::EdgeFake3DType Configuration::getEdgeDepiction(std::size_t i) const {
	return IO::Graph::Write::EdgeFake3DType::None;
}

void Configuration::printCoords(std::ostream &s, const std::vector<std::size_t> &vIds) const {
	const std::string &name = getGeometryGraph().getGraph()[getGeometryVertex()].name;
	s << "\\coordinate[overlay] (v-coord-" << vIds.back() << ") {};\n";
	s << "\\node[at=($(v-coord-" << vIds.back() << ")+(-90:1)+(-90:12pt)$)] {" << name << "};\n";
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
		MOD_ABORT;
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
namespace mod::lib::IO::Stereo::Read {
} // namesapce mod::lib::IO::Stereo::Read
namespace mod::lib::IO::Stereo::Write {
namespace {

template<typename GraphInner>
std::string coords(const GraphInner &gStereo, const lib::Stereo::Configuration &conf, const std::string &name,
                   std::map<typename boost::graph_traits<GraphInner>::vertex_descriptor, int> &vMap) {
	post::FileHandle s(getUniqueFilePrefix() + name + "_coord.tex");
	std::vector<std::size_t> vIds(num_vertices(gStereo));
	using SVertex = typename boost::graph_traits<GraphInner>::vertex_descriptor;
	for(SVertex vStereo : asRange(vertices(gStereo))) {
		auto vId = get(boost::vertex_index_t(), gStereo, vStereo);
		auto iter = vMap.find(vStereo);
		assert(iter != end(vMap));
		if(iter->second == -1) vIds[vIds.size() - 1] = vId;
		else vIds[iter->second] = vId;
	}
	conf.printCoords(s, vIds);
	return s;
}

template<typename GraphPrint, typename Depict, typename ShownId>
std::pair<std::string, std::string>
tikz(const GraphPrint &g, typename boost::graph_traits<GraphPrint>::vertex_descriptor v,
     const lib::Stereo::Configuration &conf, const std::string &name, const Depict &depict,
     const IO::Graph::Write::Options &options, ShownId shownId) {
	const bool printLonePairs = true;
	using GVertex = typename boost::graph_traits<GraphPrint>::vertex_descriptor;
	using GEdge = typename boost::graph_traits<GraphPrint>::edge_descriptor;
	using GraphStereo = lib::Graph::GraphType;
	using SVertex = lib::Graph::Vertex;
	using SEdge = lib::Graph::Edge;

	// we make a new graph with copies and then the extra lone pairs
	GraphStereo gStereo;
	std::map<SVertex, int> vMap; // the order id, though -1 => the center
	SVertex vCenter = add_vertex(gStereo);
	vMap[vCenter] = -1;
	std::map<std::pair<SVertex, SVertex>, IO::Graph::Write::EdgeFake3DType> edgeDepiction;
	for(std::size_t i = 0; i < conf.degree(); ++i) {
		SVertex vStereo = add_vertex(gStereo);
		vMap[vStereo] = i;
		add_edge(vCenter, vStereo, gStereo);
		auto edge3Dtype = conf.getEdgeDepiction(i);
		edgeDepiction[std::make_pair(vCenter, vStereo)] = edge3Dtype;
		edgeDepiction[std::make_pair(vStereo, vCenter)] = IO::Graph::Write::invertEdgeFake3DType(edge3Dtype);
	}

	std::string coordFile = coords(gStereo, conf, name, vMap);
	post::FileHandle s(getUniqueFilePrefix() + name + ".tex");

	struct DepictorAndAdvOptions {
		DepictorAndAdvOptions(const GraphPrint &gOuter, GVertex vOuterCenter, const lib::Graph::GraphType &g,
		                      const Depict &depict, bool printLonePairs, const std::map<SVertex, int> &vMap,
		                      const lib::Stereo::Configuration &conf,
		                      const std::map<std::pair<SVertex, SVertex>, IO::Graph::Write::EdgeFake3DType> &edgeDepiction,
		                      ShownId shownId)
				: gOuter(gOuter), vOuterCenter(vOuterCenter),
				  nullVertexOuter(boost::graph_traits<GraphPrint>::null_vertex()), gInner(g),
				  depict(depict), printLonePairs(printLonePairs), vMap(vMap), conf(conf), edgeDepiction(edgeDepiction),
				  shownId(shownId) {}

		GVertex getOuterVertexFromInnerVertex(SVertex vInner) const {
			auto iter = vMap.find(vInner);
			assert(iter != end(vMap));
			if(iter->second == -1) return vOuterCenter;
			const auto &emb = conf.begin()[iter->second];
			if(emb.type != lib::Stereo::EmbeddingEdge::Type::Edge) return nullVertexOuter;
			auto eOuter = emb.getEdge(vOuterCenter, gOuter);
			return target(eOuter, gOuter);
		}

		GEdge getOuterEdgeFromInnerEdge(SEdge eInner) const {
			GVertex vSrcOuter = getOuterVertexFromInnerVertex(source(eInner, gInner));
			GVertex vTarOuter = getOuterVertexFromInnerVertex(target(eInner, gInner));
			assert(vSrcOuter != nullVertexOuter);
			assert(vTarOuter != nullVertexOuter);
			auto p = edge(vSrcOuter, vTarOuter, gOuter);
			assert(p.second);
			return p.first;
		}

		unsigned char getAtomId(SVertex vInner) const {
			GVertex vOuter = getOuterVertexFromInnerVertex(vInner);
			if(vOuter == nullVertexOuter) return AtomIds::Invalid;
			else return depict.getAtomId(vOuter);
		}

		Isotope getIsotope(SVertex vInner) const {
			GVertex vOuter = getOuterVertexFromInnerVertex(vInner);
			if(vOuter == nullVertexOuter) return Isotope();
			else return depict.getIsotope(vOuter);
		}

		char getCharge(SVertex vInner) const {
			GVertex vOuter = getOuterVertexFromInnerVertex(vInner);
			if(vOuter == nullVertexOuter) return 0;
			else return depict.getCharge(vOuter);
		}

		bool getRadical(SVertex vInner) const {
			GVertex vOuter = getOuterVertexFromInnerVertex(vInner);
			if(vOuter == nullVertexOuter) return false;
			else return depict.getRadical(vOuter);
		}

		BondType getBondData(SEdge eInner) const {
			GVertex vSrcOuter = getOuterVertexFromInnerVertex(source(eInner, gInner));
			GVertex vTarOuter = getOuterVertexFromInnerVertex(target(eInner, gInner));
			if(vSrcOuter == nullVertexOuter || vTarOuter == nullVertexOuter) return BondType::Invalid;
			else return depict.getBondData(getOuterEdgeFromInnerEdge(eInner));
		}

		AtomData operator()(SVertex v) const {
			GVertex vOuter = getOuterVertexFromInnerVertex(v);
			if(vOuter == nullVertexOuter) return AtomData();
			else return depict(vOuter);
		}

		BondType operator()(SEdge eInner) const {
			GVertex vSrcOuter = getOuterVertexFromInnerVertex(source(eInner, gInner));
			GVertex vTarOuter = getOuterVertexFromInnerVertex(target(eInner, gInner));
			if(vSrcOuter == nullVertexOuter || vTarOuter == nullVertexOuter) return BondType::Invalid;
			else return depict(getOuterEdgeFromInnerEdge(eInner));
		}

		std::string getVertexLabelNoIsotopeChargeRadical(SVertex vInner) const {
			GVertex vOuter = getOuterVertexFromInnerVertex(vInner);
			if(vOuter != nullVertexOuter)return depict.getVertexLabelNoIsotopeChargeRadical(vOuter);
			auto iter = vMap.find(vInner);
			assert(iter != end(vMap));
			assert(iter->second != -1);
			const auto &emb = conf.begin()[iter->second];
			switch(emb.type) {
			case lib::Stereo::EmbeddingEdge::Type::Edge:
				MOD_ABORT;
			case lib::Stereo::EmbeddingEdge::Type::LonePair:
				return "e";
			case lib::Stereo::EmbeddingEdge::Type::Radical:
				return "r";
			}
			MOD_ABORT;
		}

		std::string getEdgeLabel(SEdge eInner) const {
			GVertex vSrcOuter = getOuterVertexFromInnerVertex(source(eInner, gInner));
			GVertex vTarOuter = getOuterVertexFromInnerVertex(target(eInner, gInner));
			if(vSrcOuter == nullVertexOuter || vTarOuter == nullVertexOuter) return "";
			else return depict.getEdgeLabel(getOuterEdgeFromInnerEdge(eInner));
		}

		bool hasImportantStereo(SVertex vInner) const {
			return true;
		}

		bool getHasCoordinates() const {
			return false;
		}

		double getX(SVertex v, bool b) const {
			return 0;
		}

		double getY(SVertex v, bool b) const {
			return 0;
		}

		bool isVisible(SVertex v) const {
			if(printLonePairs) return true;
			else
				MOD_ABORT;
			return true;
		}

		std::string getColour(SVertex) const {
			return "";
		}

		std::string getColour(SEdge) const {
			return "";
		}

		std::string getShownId(SVertex vInner) const {
			GVertex vOuter = getOuterVertexFromInnerVertex(vInner);
			if(vOuter == nullVertexOuter) MOD_ABORT;
			else return boost::lexical_cast<std::string>(shownId(gOuter, vOuter));
		}

		bool overwriteWithIndex(SVertex vInner) const {
			auto vOuter = getOuterVertexFromInnerVertex(vInner);
			return vOuter == nullVertexOuter;
		}

		IO::Graph::Write::EdgeFake3DType getEdgeFake3DType(SEdge eInner, bool withHydrogen) const {
			auto iter = edgeDepiction.find(std::make_pair(source(eInner, gInner), target(eInner, gInner)));
			assert(iter != end(edgeDepiction));
			return iter->second;
		}

		std::string getRawStereoString(SVertex vInner) const {
			return "";
		}

		std::string getPrettyStereoString(SVertex vInner) const {
			return "";
		}

		std::string getStereoString(SEdge eInner) const {
			return "";
		}

		std::string getEdgeAnnotation(SEdge eInner) const {
			SVertex vSrcInner = source(eInner, gInner), vTarInner = target(eInner, gInner);
			auto iterSrc = vMap.find(vSrcInner), iterTar = vMap.find(vTarInner);
			assert(iterSrc != end(vMap));
			assert(iterTar != end(vMap));
			assert(iterSrc->second == -1 || iterTar->second == -1);
			bool swapped = false;
			if(iterSrc->second != -1) {
				std::swap(vSrcInner, vTarInner);
				std::swap(iterSrc, iterTar);
				swapped = true;
			}
			std::string res;
			// if the edge category does not correspond to the bond type, then print it
			const lib::Stereo::EmbeddingEdge &emb = conf.begin()[iterTar->second];
			if(emb.type == lib::Stereo::EmbeddingEdge::Type::Edge) {
				auto eOuter = getOuterEdgeFromInnerEdge(eInner);
				auto eCatFromBt = lib::Stereo::bondTypeToEdgeCategory(depict.getBondData(eOuter));
				if(eCatFromBt != emb.cat) {
					res += " node[auto] {";
					res += boost::lexical_cast<std::string>(emb.cat);
					res += "}";
				}
			}

			// print the offset and whatever the configuration wants
			res += " node[auto, pos=";
			if(swapped) res += "0.25";
			else res += "0.75";
			res += "] {{\\tiny ";
			res += boost::lexical_cast<std::string>(iterTar->second);
			res += conf.getEdgeAnnotation(iterTar->second);
			res += "}} ";
			return res;
		}

		bool disallowCollapse(SVertex) const {
			return false;
		}

		bool disallowCollapse(SEdge) const {
			return false;
		}

		std::string getOpts(SVertex v) const {
			return std::string();
		}
	public:
		unsigned int idOffset = 0;
	private:
		const GraphPrint &gOuter;
		GVertex vOuterCenter, nullVertexOuter;
		const lib::Graph::GraphType &gInner;
		const Depict &depict;
		bool printLonePairs;
		const std::map<SVertex, int> &vMap;
		const lib::Stereo::Configuration &conf;
		const std::map<std::pair<SVertex, SVertex>, IO::Graph::Write::EdgeFake3DType> &edgeDepiction;
		ShownId shownId;
	} depictAndAdvOptions(g, v, gStereo, depict, printLonePairs, vMap, conf, edgeDepiction, shownId);
	auto bonusWriter = [&](std::ostream &s) {
	};
	lib::IO::Graph::Write::tikz(s, options, gStereo, depictAndAdvOptions, coordFile, depictAndAdvOptions, bonusWriter,
	                            "");
	return std::pair<std::string, std::string>(s, coordFile);
}

template<typename Graph, typename Depict, typename ShownId>
std::string pdf(const Graph &g, typename boost::graph_traits<Graph>::vertex_descriptor v,
                const lib::Stereo::Configuration &conf, const std::string &name, const Depict &depict,
                const IO::Graph::Write::Options &options, ShownId shownId) {
	const auto p = tikz(g, v, conf, name, depict, options, shownId);
	std::string fileTikz = p.first, fileCoords = p.second;
	std::string fileNoExt = fileTikz.substr(0, fileTikz.size() - 4);
	std::string fileCoordsNoExt = fileCoords.substr(0, fileCoords.size() - 4);
	IO::post() << "compileTikz \"" << fileNoExt << "\" \"" << fileCoordsNoExt << "\"" << std::endl;
	return fileNoExt + ".pdf";
}

} // namespace

std::string summary(const lib::Graph::Single &gLib, lib::Graph::Vertex v, const lib::Stereo::Configuration &conf,
                    const IO::Graph::Write::Options &options, int shownIdOffset, const std::string &nameSuffix) {
	const auto &g = gLib.getGraph();
	const auto vId = get(boost::vertex_index_t(), g, v);
	std::string name = "g_" + boost::lexical_cast<std::string>(gLib.getId()) + "_stereo_" +
	                   boost::lexical_cast<std::string>(vId);
	IO::post() << "summarySubsection \"Stereo, g " << gLib.getId() << ", v " << vId
	           << nameSuffix << "\"\n";
	std::string f = pdf(g, v, conf, name, gLib.getDepictionData(), options, [shownIdOffset](const auto &g, const auto v) {
		return get(boost::vertex_index_t(), g, v) + shownIdOffset;
	});
	post::FileHandle s(getUniqueFilePrefix() + "stereo.tex");
	s << "\\begin{center}\n";
	s << "\\includegraphics{" << f << "}\\\\\n";
	s << "File: \\texttt{" << escapeForLatex(f) << "}\n";
	s << "\\end{center}\n";
	IO::post() << "summaryInput \"" << std::string(s) << "\"\n";
	return f;
}

std::string summary(const lib::Rules::Real &r, lib::Rules::Vertex v, lib::Rules::Membership m,
                    const IO::Graph::Write::Options &options) {
	assert(m != lib::Rules::Membership::Context);
	if(m == lib::Rules::Membership::Left) assert(membership(r.getDPORule(), v) != lib::Rules::Membership::Right);
	if(m == lib::Rules::Membership::Right) assert(membership(r.getDPORule(), v) != lib::Rules::Membership::Left);
	const std::string side = m == lib::Rules::Membership::Left ? "L" : "R";
	const auto &g = get_graph(r.getDPORule());
	std::string name = "r_" + std::to_string(r.getId()) + "_" + side + "_stereo_" +
	                   std::to_string(get(boost::vertex_index_t(), g, v));
	IO::post() << "summarySubsection \"Stereo, r " << r.getId() << ", v " << get(boost::vertex_index_t(), g, v) << " "
	           << side << "\"" << std::endl;
	const auto handler = [&](const auto &gLabelled, const auto &depict) {
		const auto &g = get_graph(gLabelled);
		return pdf(g, v, *get_stereo(gLabelled)[v], name, depict, options, [](const auto &g, const auto v) {
			return get(boost::vertex_index_t(), g, v);
		});
	};
	std::string f = m == lib::Rules::Membership::Left
	                ? handler(get_labelled_left(r.getDPORule()), r.getDepictionData().getLeft())
	                : handler(get_labelled_right(r.getDPORule()), r.getDepictionData().getRight());
	post::FileHandle s(getUniqueFilePrefix() + "stereo.tex");
	s << "\\begin{center}\n";
	s << "\\includegraphics{" << f << "}\\\\\n";
	s << "File: \\texttt{" << escapeForLatex(f) << "}\n";
	s << "\\end{center}\n";
	IO::post() << "summaryInput \"" << std::string(s) << "\"\n";
	return f;
}

//------------------------------------------------------------------------------

void summary(const lib::Stereo::GeometryGraph &graph) {
	const auto &g = graph.getGraph();
	const auto n = num_vertices(g);
	std::string fileDot = [&]() {
		post::FileHandle s(IO::getUniqueFilePrefix() + "geometryGraph.dot");
		s << "digraph g {\nrankdir=LR;\n";
		for(auto v : asRange(vertices(g))) {
			s << get(boost::vertex_index_t(), g, v) << " [ label=\"" << g[v].name << "\" ];\n";
		}
		for(auto e : asRange(edges(g))) {
			auto vSrc = source(e, g);
			auto vTar = target(e, g);
			s << get(boost::vertex_index_t(), g, vSrc) << " -> " << get(boost::vertex_index_t(), g, vTar);
			s << " [];\n";
		}
		for(std::size_t i = 0; i < graph.chemValids.size(); ++i) {
			auto &cv = graph.chemValids[i];
			s << (i + n) << " [ label=\"" << lib::Chem::symbolFromAtomId(cv.atomId);
			if(cv.charge != 0) {
				if(cv.charge > 0) s << "+";
				else s << "-";
				if(std::abs(cv.charge) != 1) s << std::abs(cv.charge);
			}
			if(cv.catCount.sum() > 0) s << ", " << cv.catCount;
			if(cv.lonePair > 0) s << ", e = " << cv.lonePair;
			s << "\" ];\n";
			s << get(boost::vertex_index_t(), g, cv.geometry) << " -> " << (i + n) << " [];\n";
		}
		s << "}\n";
		std::string f = s;
		return std::string(f.begin(), f.end() - 4);
	}();
	IO::post() << "coordsFromGV dgHyperDot \"" << fileDot << "\"\n";
	std::string fileCoords = fileDot + "_coord";
	std::string fileFig = [&]() {
		post::FileHandle s(IO::getUniqueFilePrefix() + "geometryGraph.tex");
		s << "\\begin{tikzpicture}[scale=\\modDGHyperScale]\n";
		s << "\\input{" << fileCoords << ".tex}\n";
		for(auto v : asRange(vertices(g))) {
			auto vId = get(boost::vertex_index_t(), g, v);
			s << "\\node[draw] (v-" << vId << ") at (v-coord-" << vId << "){";
			s << g[v].name;
			s << "};\n";
		}
		for(auto e : asRange(edges(g))) {
			auto vSrc = source(e, g);
			auto vTar = target(e, g);
			s << "\\path[draw, ->, >=stealth] (v-" << get(boost::vertex_index_t(), g, vSrc)
			  << ") to (v-" << get(boost::vertex_index_t(), g, vTar)
			  << ");\n";
		}
		for(std::size_t i = 0; i < graph.chemValids.size(); ++i) {
			auto &cv = graph.chemValids[i];
			s << "\\node[draw, ellipse] (v-" << (i + n) << ") at (v-coord-" << (i + n) << "){";
			s << lib::Chem::symbolFromAtomId(cv.atomId);
			if(cv.charge != 0) {
				s << "$^{";
				if(cv.charge > 0) s << "+";
				else s << "-";
				if(std::abs(cv.charge) != 1) s << std::abs(cv.charge);
				s << "}$";
			}
			bool hasMore = cv.catCount.sum() > 0 || cv.lonePair > 0;
			if(hasMore) s << "$";
			if(cv.catCount.sum() > 0) s << ", " << cv.catCount;
			if(cv.lonePair > 0) s << ", e = " << cv.lonePair;
			if(hasMore) s << "$";
			s << "};\n";
			s << "\\path[draw] (v-" << get(boost::vertex_index_t(), g, cv.geometry) << ") to (v-" << (i + n) << ");\n";
		}
		s << "\\end{tikzpicture}";
		std::string f = s;
		return std::string(f.begin(), f.end() - 4);
	}();
	std::string fileTex = [&]() {
		post::FileHandle s(IO::getUniqueFilePrefix() + "geometryGraphSummary.tex");
		s
				<< "\\begin{center}\n"
				<< "\\includegraphics{" << fileFig << "}\n\n"
				<< "File: \\texttt{" << escapeForLatex(fileFig) << "}\n"
				<< "\\end{center}\n"
				<< "\n"
				<< "\\section{Stereo, Chemically Valid Configurations}\n"
				<< "\\begin{center}\n"
				<< "\\begin{longtable}{@{}lllllll@{}}\n"
				<< "\\toprule\n"
				<< "Atom	& S	& D	& T	& A	& LP	& Geometry\\\\\n"
				<< "\\midrule\n";
		for(std::size_t i = 0; i < graph.chemValids.size(); ++i) {
			auto &cv = graph.chemValids[i];
			s << lib::Chem::symbolFromAtomId(cv.atomId);
			if(cv.charge != 0) {
				s << "$^{";
				if(cv.charge > 0) s << "+";
				else s << "-";
				if(std::abs(cv.charge) != 1) s << std::abs(cv.charge);
				s << "}$";
			}
			for(auto cat :{lib::Stereo::EdgeCategory::Single, lib::Stereo::EdgeCategory::Double,
			               lib::Stereo::EdgeCategory::Triple, lib::Stereo::EdgeCategory::Aromatic}) {
				s << "	& ";
				if(auto count = cv.catCount[cat]) {
					s << int(count);
				}
			}
			s << "	& ";
			if(cv.lonePair > 0) s << cv.lonePair;
			s << "	& " << g[cv.geometry].name << " \\\\\n";
		}
		s
				<< "\\bottomrule\n"
				<< "\\end{longtable}\n"
				<< "\\end{center}\n";
		return std::string(s);
	}();

	IO::post() << "compileTikz \"" << fileFig << "\" \"" << fileCoords << "\"" << std::endl;
	IO::post() << "summarySection \"Stereo, Geometry Graph\"" << std::endl;
	IO::post() << "summaryInput \"" << fileTex << "\"" << std::endl;
}

} // namesapce mod::lib::IO::Stereo::Write