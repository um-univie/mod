#include "RC.h"

#include <mod/Config.h>
#include <mod/lib/IO/FileHandle.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/IO/Rule.h>
#include <mod/lib/RC/Evaluator.h>
#include <mod/lib/Rules/Real.h>
#include <mod/lib/Rules/Properties/Depiction.h>
#include <mod/lib/Rules/Properties/String.h>

namespace mod {
namespace lib {
namespace IO {
namespace RC {
namespace Read {
} // namespace Read
namespace Write {

std::string dot(const lib::RC::Evaluator &rc) {
	typedef lib::RC::Evaluator::Vertex Vertex;
	typedef lib::RC::Evaluator::Edge Edge;
	const lib::RC::Evaluator::GraphType &rcg = rc.getGraph();
	FileHandle s(getUniqueFilePrefix() + "rc.dot");
	std::string fileNoExt = s;
	fileNoExt.erase(end(fileNoExt) - 4, end(fileNoExt));
	s << "digraph g {" << std::endl;

	for(Vertex v : asRange(vertices(rcg))) {
		s << "\t" << get(boost::vertex_index_t(), rcg, v) << " [";
		switch(rcg[v].kind) {
		case lib::RC::Evaluator::VertexKind::Rule:
			s << " label=\"" << rcg[v].rule->getName() << "\"";
			break;
		case lib::RC::Evaluator::VertexKind::Composition:
			s << " shape=point";
			break;
		}
		s << " ];" << std::endl;
	}

	for(Edge e : asRange(edges(rcg))) {
		s << "\t" << get(boost::vertex_index_t(), rcg, source(e, rcg))
				<< " -> " << get(boost::vertex_index_t(), rcg, target(e, rcg)) << " [";
		switch(rcg[e].kind) {
		case lib::RC::Evaluator::EdgeKind::First:
			s << " label=1";
			break;
		case lib::RC::Evaluator::EdgeKind::Second:
			s << " label=2";
			break;
		case lib::RC::Evaluator::EdgeKind::Result:
			break;
		}
		s << " ];" << std::endl;
	}
	s << "}" << std::endl;
	return fileNoExt;
}

std::string svg(const lib::RC::Evaluator &rc) {
	std::string fileNoExt = dot(rc);
	IO::post() << "gv rc \"" << fileNoExt << "\" svg" << std::endl;
	return fileNoExt;
}

std::string pdf(const lib::RC::Evaluator &rc) {
	std::string fileNoExt = svg(rc);
	IO::post() << "svgToPdf \"" << fileNoExt << "\"" << std::endl;
	return fileNoExt;
}

void test(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, const CoreCoreMap &match, const lib::Rules::Real &rNew) {
	using CoreVertex = lib::Rules::Vertex;
	using CoreEdge = lib::Rules::Edge;
	IO::Rules::Write::Options options;
	options.CollapseHydrogens(true);
	options.EdgesAsBonds(true);
	if(getConfig().rc.matchesWithIndex.get())
		options.WithIndex(true);
	auto visible = [](CoreVertex) {
		return true;
	};
	auto vColour = [](CoreVertex) {
		return std::string();
	};
	auto eColour = [](CoreEdge) {
		return std::string();
	};
	// make a fake rule with all the vertices and edges, just for coords
	std::map<CoreVertex, CoreVertex> vFirstToCommon, vSecondToCommon, vNewToCommon;
	lib::Rules::LabelledRule dpoCommon(rFirst.getDPORule(), false);
	lib::Rules::GraphType &gComon = get_graph(dpoCommon);
	lib::Rules::PropStringCore &pStringCommon = *dpoCommon.pString;
	for(CoreVertex v : asRange(vertices(rFirst.getGraph()))) vFirstToCommon[v] = v;
	// TODO: this will completely break if vertices are deleted in the composed rule
	for(CoreVertex v : asRange(vertices(rNew.getGraph())))
		vNewToCommon[v] = v;
	// copy rSecond vertices
	for(CoreVertex v : asRange(vertices(rSecond.getGraph()))) {
		auto rightIter = match.right.find(v);
		if(rightIter != match.right.end()) vSecondToCommon[v] = rightIter->second;
		else {
			CoreVertex vCommon = add_vertex(gComon);
			vSecondToCommon[v] = vCommon;
			gComon[vCommon].membership = lib::Rules::Membership::Context;
			const std::string &label = rSecond.getGraph()[v].membership == lib::Rules::Membership::Left
					? rSecond.getStringState().getLeft()[v]
					: rSecond.getStringState().getRight()[v];
			pStringCommon.add(vCommon, label, label);
		}
	}
	// copy rSecond edges
	for(CoreEdge e : asRange(edges(rSecond.getGraph()))) {
		CoreVertex vSrcSecond = source(e, rSecond.getGraph());
		CoreVertex vTarSecond = target(e, rSecond.getGraph());
		auto iterSrc = vSecondToCommon.find(vSrcSecond);
		auto iterTar = vSecondToCommon.find(vTarSecond);
		assert(iterSrc != end(vSecondToCommon));
		assert(iterTar != end(vSecondToCommon));
		CoreVertex vSrc = iterSrc->second;
		CoreVertex vTar = iterTar->second;
		auto pEdge = edge(vSrc, vTar, gComon);
		if(pEdge.second) continue;
		pEdge = add_edge(vSrc, vTar, gComon);
		gComon[pEdge.first].membership = lib::Rules::Membership::Context;
		const std::string &label = rSecond.getGraph()[e].membership == lib::Rules::Membership::Left
				? rSecond.getStringState().getLeft()[e]
				: rSecond.getStringState().getRight()[e];
		pStringCommon.add(pEdge.first, label, label);
	}
	lib::Rules::Real rCommon(std::move(dpoCommon));
	lib::Rules::Real rFirstCopy(lib::Rules::LabelledRule(rFirst.getDPORule(), false));
	lib::Rules::Real rSecondCopy(lib::Rules::LabelledRule(rSecond.getDPORule(), false));
	lib::Rules::Real rNewCopy(lib::Rules::LabelledRule(rNew.getDPORule(), false));
	rFirstCopy.getDepictionData().copyCoords(rCommon.getDepictionData(), vFirstToCommon);
	rSecondCopy.getDepictionData().copyCoords(rCommon.getDepictionData(), vSecondToCommon);
	rNewCopy.getDepictionData().copyCoords(rCommon.getDepictionData(), vNewToCommon);

	unsigned int secondIdOffset = num_vertices(rFirst.getGraph());
	std::set<CoreVertex> matchVerticesInCommon;
	for(CoreVertex v : asRange(vertices(rFirst.getGraph()))) {
		if(match.left.find(v) == match.left.end()) continue;
		auto iter = vFirstToCommon.find(v);
		assert(iter != end(vFirstToCommon));
		matchVerticesInCommon.insert(iter->second);
	}
	auto disallowCollapseFirst = [&matchVerticesInCommon, &vFirstToCommon](CoreVertex v) {
		auto iter = vFirstToCommon.find(v);
		assert(iter != end(vFirstToCommon));
		return matchVerticesInCommon.find(iter->second) != end(matchVerticesInCommon);
	};
	auto disallowCollapseSecond = [&matchVerticesInCommon, &vSecondToCommon](CoreVertex v) {
		auto iter = vSecondToCommon.find(v);
		assert(iter != end(vSecondToCommon));
		return matchVerticesInCommon.find(iter->second) != end(matchVerticesInCommon);
	};
	auto disallowCollapseNew = [&matchVerticesInCommon, &vNewToCommon](CoreVertex v) {
		auto iter = vNewToCommon.find(v);
		assert(iter != end(vNewToCommon));
		return matchVerticesInCommon.find(iter->second) != end(matchVerticesInCommon);
	};
	auto rawFilesFirst = IO::Rules::Write::tikz(rFirstCopy, 0, options, "L", "K", "R", visible, vColour, eColour, disallowCollapseFirst);
	auto rawFilesSecond = IO::Rules::Write::tikz(rSecondCopy, secondIdOffset, options, "L", "K", "R", visible, vColour, eColour, disallowCollapseSecond);
	auto rawFilesNew = IO::Rules::Write::tikz(rNewCopy, 0, options, "L", "K", "R", visible, vColour, eColour, disallowCollapseNew);
	FileHandle s(getUniqueFilePrefix() + "rcMatch.tex");
	//	IO::log() << "rFirstFiles: " << rawFilesFirst.first << ", " << rawFilesFirst.second << std::endl;
	//	IO::log() << "rSecondFiles: " << rawFilesSecond.first << ", " << rawFilesSecond.second << std::endl;
	{
		s << "\\newpage" << std::endl;
		s << "\\subsection{RC Match}" << std::endl;
		s << "{\\centering" << std::endl;
		auto arrow = [](std::string edgeLabel) -> std::string {
			return R"XXX(\begin{tikzpicture}[node distance=20pt,
				baseline={([yshift={-\ht\strutbox}]A.center)}]
				\node (A) {};
				\node (B) [right=of A] {};
				\draw[->, >=triangle 45] (A) to node[above] {)XXX" + edgeLabel + R"XXX(} (B);
				\end{tikzpicture})XXX";
		};
		s << "\\input{\\modInputPrefix/" << rawFilesFirst.first << "_L.tex}" << std::endl;
		s << arrow("$r_1$") << std::endl;
		s << "\\input{\\modInputPrefix/" << rawFilesFirst.first << "_R.tex}" << std::endl;
		s << "\\\\" << std::endl;
		s << "\\mbox{}\\hfill";
		s << "\\input{\\modInputPrefix/" << rawFilesSecond.first << "_L.tex}" << std::endl;
		{ // the match lines
			s << "\\begin{tikzpicture}[remember picture, overlay]" << std::endl;
			for(auto m : match.left) {
				CoreVertex vFirst = m.first;
				CoreVertex vSecond = m.second;
				unsigned int vIdFirst = get(boost::vertex_index_t(), rFirst.getGraph(), vFirst);
				unsigned int vIdSecond = get(boost::vertex_index_t(), rSecond.getGraph(), vSecond);
				vIdSecond += num_vertices(rFirst.getGraph());
				s << "\\path[modRCMatchEdge] (v-" << vIdFirst << ") to[modRCMatchEdgeTo] (v-" << vIdSecond << ");" << std::endl;
			}
			s << "\\end{tikzpicture}" << std::endl;
		}
		s << arrow("$r_2$") << std::endl;
		s << "\\input{\\modInputPrefix/" << rawFilesSecond.first << "_R.tex}" << std::endl;
		s << "\\\\" << std::endl;
		s << "\\input{\\modInputPrefix/" << rawFilesNew.first << "_L.tex}" << std::endl;
		s << arrow("$r$") << std::endl;
		s << "\\input{\\modInputPrefix/" << rawFilesNew.first << "_R.tex}" << std::endl;
		s << "}" << std::endl;
	}
	FileHandle sAux(getUniqueFilePrefix() + "rcMatch_aux.tex");
	{
		sAux << "\\\\" << std::endl;
		sAux << "Files: \\texttt{" << IO::escapeForLatex(rawFilesFirst.first)
				<< "}, \\texttt{" << IO::escapeForLatex(rawFilesSecond.first)
				<< "}, \\texttt{" << IO::escapeForLatex(s)
				<< "}, \\texttt{" << IO::escapeForLatex(sAux) << "}" << std::endl;
		sAux << "\\\\" << std::endl;
		sAux << "Match: " << std::endl;
		bool first = true;
		for(auto m : match.left) {
			if(!first) sAux << ", ";
			sAux << "$" << m.first << "\\rightarrow " << m.second << "$" << std::endl;
			first = false;
		}
	}
	IO::post() << "summaryInput \"" << std::string(s) << "\"" << std::endl;
	IO::post() << "summaryInput \"" << std::string(sAux) << "\"" << std::endl;
}

} // namespace Write
} // namespace RC
} // namespace IO
} // namespace lib
} // namespace mod