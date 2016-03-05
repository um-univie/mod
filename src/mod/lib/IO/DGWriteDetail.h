#ifndef MOD_LIB_IO_DGWRITEDETAIL_H
#define	MOD_LIB_IO_DGWRITEDETAIL_H

#include <mod/lib/Graph/Single.h>
#include <mod/lib/IO/DG.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

#include <boost/lexical_cast.hpp>

namespace mod {
namespace lib {
namespace IO {
namespace DG {
namespace Write {

template<typename T>
std::string toStr(const T &t) {
	return boost::lexical_cast<std::string>(t);
}

namespace detail {

inline std::string hyperEdgeComment(const lib::DG::Hyper &dg, Vertex v) {
	unsigned int vId = get(boost::vertex_index_t(), dg.getGraph(), v);
	return "id = " + toStr(vId) + toStr(dg.getDerivation(v));
}

template<typename Body>
void forEachVertex(const lib::DG::Hyper &dg, const Options &options, SyntaxPrinter &print, bool printHeader, Body body) {
	using DupVertex = Options::DupVertex;
	const auto &g = dg.getGraph();
	const auto &dupGraph = options.dupGraph;
	DupVertex prevVertex = dupGraph.null_vertex();
	for(DupVertex vDup : asRange(vertices(dupGraph))) {
		Vertex v = dupGraph[vDup].v;
		if(g[v].kind != lib::DG::HyperVertexKind::Vertex) continue;
		unsigned int vId = get(boost::vertex_index_t(), g, v);
		const auto &graph = *g[v].graph;
		if(printHeader && (prevVertex == dupGraph.null_vertex() || dupGraph[prevVertex].v != dupGraph[vDup].v)) {
			// print header for vertex
			std::string comment = "id = " + toStr(vId);
			comment += ", graphName = ";
			comment += graph.getName();
			print.comment(comment);
		}
		prevVertex = vDup;
		if(!options.isVertexVisible(v, dg)) continue;
		body(vDup);
	}
}

template<typename Body>
void forEachExplicitHyperEdge(const lib::DG::Hyper &dg, const Options &options, SyntaxPrinter &print, Body body) {
	using DupVertex = Options::DupVertex;
	const auto &g = dg.getGraph();
	const auto &dupGraph = options.dupGraph;
	DupVertex prevVertex = dupGraph.null_vertex();
	for(DupVertex vDup : asRange(vertices(dupGraph))) {
		Vertex v = dupGraph[vDup].v;
		if(g[v].kind != lib::DG::HyperVertexKind::Edge) continue;
		if(!options.isHyperedgeVisible(v, dg)) continue;
		unsigned int inDegreeVisible = options.inDegreeVisible(vDup, dg).first;
		unsigned int outDegreeVisible = options.outDegreeVisible(vDup, dg).first;

		// if all sources and targets are hidden, then hide this edge
		if(inDegreeVisible == 0 && outDegreeVisible == 0) continue;

		bool isShortcutEdge = options.isShortcutEdge(vDup, dg, inDegreeVisible, outDegreeVisible);
		if(!isShortcutEdge) {
			if(prevVertex == dupGraph.null_vertex() || dupGraph[prevVertex].v != dupGraph[vDup].v) {
				print.comment(hyperEdgeComment(dg, v));
			}
			prevVertex = vDup;
			body(vDup);
		}
	}
}

template<typename TailBody, typename HeadBody, typename ShortcutBody>
void forEachConnector(const lib::DG::Hyper &dg, const Options &options, SyntaxPrinter &print, TailBody tailBody, HeadBody headBody, ShortcutBody shortcutBody) {
	using DupVertex = Options::DupVertex;
	const auto &g = dg.getGraph();
	const auto &dupGraph = options.dupGraph;
	DupVertex prevVertex = dupGraph.null_vertex();
	for(DupVertex vDup : asRange(vertices(dupGraph))) {
		Vertex v = dupGraph[vDup].v;
		if(g[v].kind != lib::DG::HyperVertexKind::Edge) continue;
		if(prevVertex == dupGraph.null_vertex() || dupGraph[prevVertex].v != dupGraph[vDup].v) {
			print.comment(detail::hyperEdgeComment(dg, v));
		} else {
			print.comment("");
		}
		prevVertex = vDup;

		if(!options.isHyperedgeVisible(v, dg)) continue;

		unsigned int inDegreeVisible, outDegreeVisible;
		DupVertex vDupTailFirst, vDupHeadFirst;
		std::tie(inDegreeVisible, vDupTailFirst) = options.inDegreeVisible(vDup, dg);
		std::tie(outDegreeVisible, vDupHeadFirst) = options.outDegreeVisible(vDup, dg);

		// if all sources and targets are hidden, then hide this edge
		if(inDegreeVisible == 0 && outDegreeVisible == 0) continue;

		bool isShortcutEdge = options.isShortcutEdge(vDup, dg, inDegreeVisible, outDegreeVisible);
		if(!isShortcutEdge) {
			// dupVertex -> count
			std::map<DupVertex, unsigned int> tailCount, headCount;
			for(DupVertex vDupIn : asRange(inv_adjacent_vertices(vDup, dupGraph))) {
				auto iter = tailCount.find(vDupIn);
				if(iter == end(tailCount)) tailCount[vDupIn] = 1;
				else iter->second++;
			}
			for(DupVertex vDupOut : asRange(adjacent_vertices(vDup, dupGraph))) {
				auto iter = headCount.find(vDupOut);
				if(iter == end(headCount)) headCount[vDupOut] = 1;
				else iter->second++;
			}

			for(const auto p : tailCount) {
				DupVertex vDupAdj = p.first;
				Vertex vAdj = options.dupGraph[vDupAdj].v;
				if(!options.isVertexVisible(vAdj, dg)) continue;
				tailBody(vDup, vDupAdj, p.second);
			}
			for(const auto p : headCount) {
				DupVertex vDupAdj = p.first;
				Vertex vAdj = options.dupGraph[vDupAdj].v;
				if(!options.isVertexVisible(vAdj, dg)) continue;
				headBody(vDup, vDupAdj, p.second);
			}
		} else {
			bool hasReverse = g[v].reverse != g.null_vertex()
					&& options.isHyperedgeVisible(g[v].reverse, dg);
			if(hasReverse) {
				// check if the reverse goes between the same vertex incarnations
				// TODO
			}
			shortcutBody(vDup, vDupTailFirst, vDupHeadFirst, hasReverse);
		}
	}
}

} // namespace detail
} // namespace Write
} // namespace DG
} // namespace IO
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_IO_DGWRITEDETAIL_H */