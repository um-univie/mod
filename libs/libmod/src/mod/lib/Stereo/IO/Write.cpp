#include "Write.hpp"

#include <mod/Post.hpp>
#include <mod/lib/Chem/MoleculeUtil.hpp>
#include <mod/lib/IO/IO.hpp>
#include <mod/lib/Stereo/GeometryGraph.hpp>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

namespace mod::lib::Stereo::Write {

void summary(const GeometryGraph &graph) {
	const auto &g = graph.getGraph();
	const auto n = num_vertices(g);
	std::string fileDot = [&]() {
		post::FileHandle s(IO::makeUniqueFilePrefix() + "geometryGraph.dot");
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
		post::FileHandle s(IO::makeUniqueFilePrefix() + "geometryGraph.tex");
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
		post::FileHandle s(IO::makeUniqueFilePrefix() + "geometryGraphSummary.tex");
		s
		<< "\\begin{center}\n"
		<< "\\includegraphics{" << fileFig << "}\n\n"
		<< "File: \\texttt{" << IO::escapeForLatex(fileFig) << "}\n"
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
			for(auto cat :{EdgeCategory::Single, EdgeCategory::Double,
								EdgeCategory::Triple, EdgeCategory::Aromatic}) {
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

} // namespace mod::lib::Stereo::Write