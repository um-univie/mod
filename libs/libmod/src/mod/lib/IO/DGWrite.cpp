#include "DG.hpp"

#include <mod/Config.hpp>
#include <mod/Function.hpp>
#include <mod/graph/Graph.hpp>
#include <mod/rule/Rule.hpp>
#include <mod/lib/Algorithm.hpp>
#include <mod/lib/DG/Dump.hpp>
#include <mod/lib/DG/Hyper.hpp>
#include <mod/lib/Graph/MultisetIO.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/GraphMorphism/VF2Finder.hpp>
#include <mod/lib/IO/DGWriteDetail.hpp>
#include <mod/lib/IO/Graph.hpp>
#include <mod/lib/IO/IO.hpp>
#include <mod/lib/IO/Rule.hpp>
#include <mod/lib/RC/ComposeRuleReal.hpp>
#include <mod/lib/RC/MatchMaker/Super.hpp>
#include <mod/lib/Rules/Real.hpp>

#include <boost/lexical_cast.hpp>

#include <fstream>

namespace mod {
namespace lib {
namespace IO {
namespace DG {
namespace Write {

std::string dump(const lib::DG::NonHyper &dg) {
	FileHandle s(getUniqueFilePrefix() + "DG.dg");
	lib::DG::Dump::write(dg, s);
	return s;
}

std::string dotNonHyper(const lib::DG::NonHyper &nonHyper) {
	FileHandle s(getUniqueFilePrefix() + "dgNonHyper_" + boost::lexical_cast<std::string>(nonHyper.getId()) + ".dot");
	{ // printing
		using Vertex = lib::DG::NonHyperVertex;
		using Edge = lib::DG::NonHyperEdge;
		const lib::DG::NonHyperGraphType &g = nonHyper.getGraph();
		s << "digraph g {" << std::endl;

		for(const auto v : asRange(vertices(g))) {
			const auto id = get(boost::vertex_index_t(), g, v);
			s << id << " [ label=\"" << g[v].graphs << "\" ];\n";
		}

		for(const auto e : asRange(edges(g))) {
			const auto srcId = get(boost::vertex_index_t(), g, source(e, g));
			const auto tarId = get(boost::vertex_index_t(), g, target(e, g));
			s << srcId << " -> " << tarId << " [ label=\"";
			bool first = true;
			for(auto *r : g[e].rules) {
				if(!first) s << ", ";
				first = false;
				s << "r_" << r->getId();
			}
			s << "\" ];\n";
		}
		s << "}\n";
	}
	std::string fileNoExt = s;
	fileNoExt.erase(fileNoExt.end() - 4, fileNoExt.end());
	return fileNoExt;
}

std::string pdfNonHyper(const lib::DG::NonHyper &nonHyper) {
	std::string fileNoExt = dotNonHyper(nonHyper);
	IO::post() << "gv dgNonHyper \"" << fileNoExt << "\" pdf" << std::endl;
	return fileNoExt;
}

//------------------------------------------------------------------------------
// Old/New delimiter
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TikzPrinter
//------------------------------------------------------------------------------

void TikzPrinter::begin() {
	s << "\\begin{tikzpicture}[remember picture, " << getConfig().dg.tikzPictureOption.get() << "]\n";
	s << "\\input{\\modInputPrefix/" << coords << "}\n";
}

void TikzPrinter::end() {
	s << "\\end{tikzpicture}%\n";
}

void TikzPrinter::comment(const std::string &str) {
	s << "% " << str << "\n";
}

void TikzPrinter::vertex(const std::string &id,
								 const std::string &label,
								 const std::string &image,
								 const std::string &colour) {
	bool haveLabel = label != "";
	bool haveImage = image != "";
	s << "\\node[modStyleDGHyperVertex, at=(v-coord-" << id << ")";
	if(!colour.empty()) s << ", draw=" << colour;
	s << "] (v-" << id << ") {";
	if(haveImage) {
		if(options.withInlineGraphs) {
			s << "{\\input{\\modInputPrefix/" << image << "}}";
		} else {
			s << "\\includegraphics[scale=\\modDGHyperImageScale] {\\modInputPrefix/" << image << "}";
		}
	}
	if(haveImage && haveLabel) s << "\\\\";
	if(haveLabel) {
		s << "{";
		if(options.labelsAsLatexMath) s << asLatexMath(label);
		else s << escapeForLatex(label);
		s << "}";
	}
	s << "};" << std::endl;
}

void TikzPrinter::vertexHidden(const std::string &id, bool large) {
	s << "\\node[modStyleDGHyperVertexHidden";
	if(large) s << "Large";
	s << ", at=(v-coord-" << id << ")] (v-" << id << ") {};" << std::endl;
}

void TikzPrinter::transitVertex(const std::string &idHost,
										  const std::string &idTransit,
										  const std::string &angle,
										  const std::string &label) {
	s << "\\node[modStyleDGTransitVertex, at=(v-" << idHost << "." << angle << ")";
	if(!label.empty()) s << ", label=" << angle << ":{" << label << "}";
	s << "] (v-" << idTransit << ") {};" << std::endl;
}

void TikzPrinter::hyperEdge(const std::string &id, const std::string &label, const std::string &colour) {
	s << "\\node[modStyleDGHyperEdge, at=(v-coord-" << id << ")";
	if(!colour.empty()) s << ", draw=" << colour;
	s << "] (v-" << id << ") {";
	if(label != "") {
		if(options.labelsAsLatexMath) s << asLatexMath(label);
		else s << escapeForLatex(label);
	}
	s << "};" << std::endl;
}

void TikzPrinter::connector(const std::string &idTail,
									 const std::string &idHead,
									 const std::string &colour,
									 unsigned int num,
									 unsigned int maxNum) {
	if(maxNum > 1) {
		s << "\\pgfmathparse{";
		int position = num - maxNum / 2 - 1;
		s << position << "*\\modDGHyperConnectorBend";
		if(maxNum % 2 == 0) s << " + 0.5*\\modDGHyperConnectorBend";
		s << "}" << std::endl << "\\pgfmathsetmacro{\\modDGHyperConnectorResultAngle}{\\pgfmathresult}" << std::endl;
	}
	s << "\\path[modStyleDGHyperConnector";
	if(!colour.empty()) s << ", draw=" << colour;
	s << "] (v-" << idTail << ") to";
	if(maxNum > 1) s << "[bend right=\\modDGHyperConnectorResultAngle]";
	s << " (v-" << idHead << ");" << std::endl;
}

void TikzPrinter::tailConnector(const std::string &idVertex,
										  const std::string &idHyperEdge,
										  const std::string &colour,
										  unsigned int num,
										  unsigned int maxNum) {
	connector(idVertex, idHyperEdge, colour, num, maxNum);
}

void TikzPrinter::headConnector(const std::string &idHyperEdge,
										  const std::string &idVertex,
										  const std::string &colour,
										  unsigned int num,
										  unsigned int maxNum) {
	connector(idHyperEdge, idVertex, colour, num, maxNum);
}

void TikzPrinter::shortcutEdge(const std::string &idTail,
										 const std::string &idHead,
										 const std::string &label,
										 const std::string &colour,
										 bool hasReverse) {
	s << "\\path[modStyleDGHyperConnector";
	if(!colour.empty()) s << ", draw=" << colour;
	s << "] (v-" << idTail << ") to";
	if(hasReverse) s << "[modStyleDGHyperHasReverseShortcut]";
	s << " node[auto, swap] {";
	if(label != "") {
		if(options.labelsAsLatexMath) s << asLatexMath(label);
		else escapeForLatex(label);
	}
	s << "} (v-" << idHead << ");" << std::endl;
}

void TikzPrinter::transitEdge(const std::string &idTail,
										const std::string &idHead,
										const std::string &label,
										const std::string &colour) {
	s << "\\path[modStyleDGHyperConnector";
	if(!colour.empty()) s << ", draw=" << colour;
	s << "] (v-" << idTail << ") to";
	s << " node[auto, swap] {";
	if(label != "") {
		if(options.labelsAsLatexMath) s << asLatexMath(label);
		else escapeForLatex(label);
	}
	s << "} (v-" << idHead << ");" << std::endl;
}

std::function<std::string(const lib::DG::Hyper &,
								  lib::DG::HyperVertex,
								  const std::string &)> TikzPrinter::getImageCreator() {
	return [this](const lib::DG::Hyper &dg, lib::DG::HyperVertex v, const std::string &id) -> std::string {
		const auto &g = *dg.getGraph()[v].graph;
		const auto doIt = [&](const auto &gOpts) {
			if(this->options.withInlineGraphs) {
				return IO::Graph::Write::tikz(g, gOpts, true, "v-" + id + "-");
			} else {
				return IO::Graph::Write::pdf(g, gOpts);
			}
		};
		if(!options.rotationOverwrite && !options.mirrorOverwrite) {
			return doIt(graphOptions);
		} else {
			auto gOpts = graphOptions;
			const auto gAPI = g.getAPIReference();
			if(options.rotationOverwrite)
				gOpts.Rotation(options.rotationOverwrite(gAPI));
			if(options.mirrorOverwrite)
				gOpts.Mirror(options.mirrorOverwrite(gAPI));
			return doIt(gOpts);
		}
	};
}

//------------------------------------------------------------------------------
// Options
//------------------------------------------------------------------------------

std::pair<unsigned int, Options::DupVertex> Options::inDegreeVisible(DupVertex e, const lib::DG::Hyper &dg) const {
	const auto &g = dg.getGraph();
	DupVertex vDupFirst = g.null_vertex();
	unsigned int numVisible = 0;
	for(DupVertex vDupIn : asRange(inv_adjacent_vertices(e, dupGraph))) {
		Vertex vIn = dupGraph[vDupIn].v;
		bool isVisible = isVertexVisible(vIn, dg);
		if(!isVisible) continue;
		numVisible++;
		if(vDupFirst == g.null_vertex()) vDupFirst = vDupIn;
	}
	return std::make_pair(numVisible, vDupFirst);
}

std::pair<unsigned int, Options::DupVertex> Options::outDegreeVisible(DupVertex e, const lib::DG::Hyper &dg) const {
	const auto &g = dg.getGraph();
	DupVertex vDupFirst = g.null_vertex();
	unsigned int numVisible = 0;
	for(DupVertex vDupOut : asRange(adjacent_vertices(e, dupGraph))) {
		Vertex vOut = dupGraph[vDupOut].v;
		bool isVisible = isVertexVisible(vOut, dg);
		if(!isVisible) continue;
		numVisible++;
		if(vDupFirst == g.null_vertex()) vDupFirst = vDupOut;
	}
	return std::make_pair(numVisible, vDupFirst);
}

bool Options::isShortcutEdge(DupVertex e,
									  const lib::DG::Hyper &dg,
									  unsigned int inDegreeVisible,
									  unsigned int outDegreeVisible) const {
	const auto &g = dg.getGraph();
	Vertex v = dupGraph[e].v;
	return (withShortcutEdges && in_degree(v, g) == 1 && out_degree(v, g) == 1 && inDegreeVisible > 0 &&
			  outDegreeVisible > 0)
			 || (withShortcutEdgesAfterVisibility && inDegreeVisible == 1 && outDegreeVisible == 1);
}

std::string Options::vDupToId(DupVertex vDup, const lib::DG::Hyper &dg) const {
	const auto dupNum = dupGraph[vDup].dupNum;
	const auto v = dupGraph[vDup].v;
	const auto vId = get(boost::vertex_index_t(), dg.getGraph(), v);
	return toStr(vId) + '-' + toStr(dupNum);
}

//------------------------------------------------------------------------------
// Data
//------------------------------------------------------------------------------

Data::Data(const lib::DG::Hyper &dg) : dg(dg) {
	for(Vertex v : asRange(vertices(dg.getGraph()))) {
		if(dg.getGraph()[v].kind != lib::DG::HyperVertexKind::Edge) continue;
		std::unordered_map<unsigned int, Connections> val;
		Connections c(in_degree(v, dg.getGraph()), out_degree(v, dg.getGraph()));
		val.insert(std::make_pair(0, std::move(c)));
		connections.insert(std::make_pair(v, std::move(val)));
	}
}

void Data::makeDuplicate(Vertex e, unsigned int eDup) {
	const auto &dg = getDG().getGraph();
	(void) dg;
	assert(dg[e].kind == lib::DG::HyperVertexKind::Edge);
	auto iterEdgeDups = connections.find(e);
	assert(iterEdgeDups != end(connections));
	auto iterEdgeCons = iterEdgeDups->second.find(eDup);
	if(iterEdgeCons != end(iterEdgeDups->second)) return;
	std::unordered_map<unsigned int, Connections> val;
	Connections c(in_degree(e, getDG().getGraph()), out_degree(e, getDG().getGraph()));
	iterEdgeDups->second.insert(std::make_pair(eDup, std::move(c)));
}

void Data::removeDuplicate(Vertex e, unsigned int eDup) {
	const auto &dg = getDG().getGraph();
	(void) dg;
	assert(dg[e].kind == lib::DG::HyperVertexKind::Edge);
	auto iterEdgeDups = connections.find(e);
	assert(iterEdgeDups != end(connections));
	iterEdgeDups->second.erase(eDup);
}

namespace {

void reconnectCommon(const lib::DG::Hyper &dgHyper,
							Data::ConnectionsStore &connections,
							Vertex v,
							unsigned int eDup,
							Vertex headOrTail,
							unsigned int vDupTar,
							unsigned int vDupSrc,
							bool isTail) {
	//	std::cout << "reconnect(" << std::boolalpha << isTail << ", " << v << ", " << eDup << ", " << headOrTail << ", " << vDupTar << ", " << vDupSrc << ")" << std::endl;
	const auto &dg = dgHyper.getGraph();
	assert(dg[v].kind == lib::DG::HyperVertexKind::Edge);
	assert(dg[headOrTail].kind == lib::DG::HyperVertexKind::Vertex);
	auto iterEdgeDups = connections.find(v);
	assert(iterEdgeDups != end(connections));
	auto iterEdgeCons = iterEdgeDups->second.find(eDup);
	if(iterEdgeCons == end(iterEdgeDups->second)) {
		std::stringstream ss;
		ss << "Derivation duplicate " << eDup << " does not exist for derivation " << dgHyper.getDerivation(v)
			<< std::endl;
		ss << "Duplicates:";
		for(const auto &dups : iterEdgeDups->second) ss << " " << dups.first;
		ss << std::endl;
		throw FatalError(ss.str());
	}
	Data::Connections &cons = iterEdgeCons->second;
	unsigned int offset = std::numeric_limits<unsigned int>::max();
	unsigned int theDegree = isTail ? in_degree(v, dg) : out_degree(v, dg);
	auto &dupNums = isTail ? cons.tail : cons.head;
	for(unsigned int i = 0; i < theDegree; i++) {
		Vertex vCand = isTail
							? *(inv_adjacent_vertices(v, dg).first + i)
							: *(adjacent_vertices(v, dg).first + i);
		if(vCand != headOrTail) continue;
		if(vDupSrc != std::numeric_limits<unsigned int>::max() && dupNums[i] != vDupSrc) continue;
		offset = i;
		break;
	}
	if(offset == std::numeric_limits<unsigned int>::max()) {
		std::stringstream ss;
		if(isTail) ss << "Tail";
		else ss << "Head";
		ss << " duplicate " << vDupSrc << " does not exist. Duplicates are:";
		for(unsigned int i = 0; i < theDegree; i++) {
			Vertex vCand = isTail
								? *(inv_adjacent_vertices(v, dg).first + i)
								: *(adjacent_vertices(v, dg).first + i);
			if(vCand != headOrTail) continue;
			ss << " " << dupNums[i];
		}
		ss << std::endl;
		throw FatalError(ss.str());
	}
	dupNums[offset] = vDupTar;
}

} // namespace

void Data::reconnectTail(Vertex v, unsigned int eDup, Vertex tail, unsigned int vDupTar, unsigned int vDupSrc) {
	reconnectCommon(getDG(), connections, v, eDup, tail, vDupTar, vDupSrc, true);
}

void Data::reconnectHead(Vertex v, unsigned int eDup, Vertex head, unsigned int vDupTar, unsigned int vDupSrc) {
	reconnectCommon(getDG(), connections, v, eDup, head, vDupTar, vDupSrc, false);
}

void Data::removeVertexIfDegreeZero(Vertex v) {
	assert(getDG().getGraph()[v].kind == lib::DG::HyperVertexKind::Vertex);
	removedIfDegreeZero.insert(v);
}

void Data::compile(Options &options) const {
	using DupVertex = Options::DupVertex;
	const auto &dg = this->dg.getGraph();
	auto &dupGraph = options.dupGraph;
	assert(num_vertices(dupGraph) == 0);
	std::vector<std::map<unsigned int, DupVertex> > duplicates(num_vertices(dg));
	auto idx = get(boost::vertex_index_t(), dg);
	// collect duplicates, set all to null_vertex
	for(const auto &eDupsCons : connections) {
		Vertex e = eDupsCons.first;
		for(const auto &eDupCons : eDupsCons.second) {
			duplicates[idx[e]][eDupCons.first] = dupGraph.null_vertex();
			for(unsigned int i = 0; i < in_degree(e, dg); i++) {
				Vertex vIn = *(inv_adjacent_vertices(e, dg).first + i);
				unsigned int vDup = eDupCons.second.tail[i];
				duplicates[idx[vIn]][vDup] = dupGraph.null_vertex();
			}
			for(unsigned int i = 0; i < out_degree(e, dg); i++) {
				Vertex vOut = *(adjacent_vertices(e, dg).first + i);
				unsigned int vDup = eDupCons.second.head[i];
				duplicates[idx[vOut]][vDup] = dupGraph.null_vertex();
			}
		}
	}
	// create vertices
	for(Vertex v : asRange(vertices(dg))) {
		if(dg[v].kind != lib::DG::HyperVertexKind::Vertex) continue;
		unsigned int vId = idx[v];
		const auto &dups = duplicates[vId];
		if(dups.empty() &&
			removedIfDegreeZero.find(v) == removedIfDegreeZero.end()) { // happens when a vertex has degree 0
			DupVertex vDup = add_vertex(dupGraph);
			dupGraph[vDup].v = v;
			dupGraph[vDup].dupNum = 0;
		} else {
			for(auto &pDup : duplicates[vId]) {
				DupVertex vDup = add_vertex(dupGraph);
				pDup.second = vDup;
				dupGraph[vDup].v = v;
				dupGraph[vDup].dupNum = pDup.first;
			}
		}
	}
	// create edges
	for(Vertex v : asRange(vertices(dg))) {
		if(dg[v].kind != lib::DG::HyperVertexKind::Edge) continue;
		unsigned int vId = idx[v];
		for(auto &pDup : duplicates[vId]) {
			DupVertex vDup = add_vertex(dupGraph);
			pDup.second = vDup;
			dupGraph[vDup].v = v;
			dupGraph[vDup].dupNum = pDup.first;
		}
	}
	// create connectors
	for(const auto &eDupsCons : connections) {
		Vertex e = eDupsCons.first;
		for(const auto &eDupCons : eDupsCons.second) {
			DupVertex eDup = duplicates[idx[e]][eDupCons.first];
			for(unsigned int i = 0; i < in_degree(e, dg); i++) {
				Vertex vIn = *(inv_adjacent_vertices(e, dg).first + i);
				unsigned int vDupNum = eDupCons.second.tail[i];
				DupVertex vDupIn = duplicates[idx[vIn]][vDupNum];
				add_edge(vDupIn, eDup, dupGraph);
			}
			for(unsigned int i = 0; i < out_degree(e, dg); i++) {
				Vertex vOut = *(adjacent_vertices(e, dg).first + i);
				unsigned int vDupNum = eDupCons.second.head[i];
				DupVertex vDupOut = duplicates[idx[vOut]][vDupNum];
				add_edge(eDup, vDupOut, dupGraph);
			}
		}
	}
	if(options.dupVertexLabel) MOD_ABORT;
	else options.dupVertexLabel = this->dupVertexLabel;
}

//------------------------------------------------------------------------------
// Printer
//------------------------------------------------------------------------------

Printer::Printer() : vertexLabelSep(", "), edgeLabelSep(", "),
							withGraphName(true), withRuleName(false), withRuleId(true) {}

std::pair<std::string, std::string> Printer::printHyper(const Data &data,
																		  const IO::Graph::Write::Options &graphOptions) {
	Options options = prePrint(data);
	const auto &dg = data.getDG();
	const auto files = IO::DG::Write::pdf(dg, options, graphOptions);
	postPrint();
	return files;
}

void Printer::pushSuffix(const std::string suffix) {
	suffixes.push_back(suffix);
}

void Printer::popSuffix() {
	assert(!suffixes.empty());
	suffixes.pop_back();
}

void Printer::pushVertexVisible(std::function<bool(Vertex, const lib::DG::Hyper &)> f) {
	vertexVisibles.push_back(f);
}

void Printer::popVertexVisible() {
	assert(!vertexVisibles.empty());
	vertexVisibles.pop_back();
}

void Printer::pushEdgeVisible(std::function<bool(Vertex, const lib::DG::Hyper &)> f) {
	edgeVisibles.push_back(f);
}

void Printer::popEdgeVisible() {
	assert(!edgeVisibles.empty());
	edgeVisibles.pop_back();
}

void Printer::pushVertexLabel(std::function<std::string(Vertex, const lib::DG::Hyper &)> f) {
	vertexLabels.push_back(f);
}

void Printer::popVertexLabel() {
	assert(!vertexLabels.empty());
	vertexLabels.pop_back();
}

void Printer::pushEdgeLabel(std::function<std::string(Vertex, const lib::DG::Hyper &)> f) {
	edgeLabels.push_back(f);
}

void Printer::popEdgeLabel() {
	assert(!edgeLabels.empty());
	edgeLabels.pop_back();
}

void Printer::pushVertexColour(std::function<std::string(Vertex, const lib::DG::Hyper &)> f, bool extendToEdges) {
	vertexColour.emplace_back(f, extendToEdges);
}

void Printer::popVertexColour() {
	assert(!vertexColour.empty());
	vertexColour.pop_back();
}

void Printer::pushEdgeColour(std::function<std::string(Vertex, const lib::DG::Hyper &)> f) {
	edgeColour.push_back(f);
}

void Printer::popEdgeColour() {
	assert(!edgeColour.empty());
	edgeColour.pop_back();
}

void Printer::setRotationOverwrite(std::function<int(std::shared_ptr<graph::Graph>)> f) {
	baseOptions.rotationOverwrite = f;
}

void Printer::setMirrorOverwrite(std::function<bool(std::shared_ptr<graph::Graph>)> f) {
	baseOptions.mirrorOverwrite = f;
}

Options Printer::prePrint(const Data &data) {
	Options options = baseOptions;
	data.compile(options);
	if(withGraphName) {
		pushVertexLabel([](Vertex v, const lib::DG::Hyper &dg) -> std::string {
			const auto &g = dg.getGraph();
			assert(g[v].kind == lib::DG::HyperVertexKind::Vertex);
			return g[v].graph->getName();
		});
	}
	if(withRuleId) { // we want the rule id before its name
		pushEdgeLabel([this](Vertex v, const lib::DG::Hyper &dg) -> std::string {
			const auto &g = dg.getGraph();
			assert(g[v].kind == lib::DG::HyperVertexKind::Edge);
			std::string res;
			bool first = true;
			for(auto *r : dg.getRulesFromEdge(v)) {
				if(!first) res += this->edgeLabelSep;
				first = false;
				res += "r_{";
				res += boost::lexical_cast<std::string>(r->getId());
				res += '}';
			}
			return res;
		});
	}
	if(withRuleName) {
		pushEdgeLabel([this](Vertex v, const lib::DG::Hyper &dg) -> std::string {
			const auto &g = dg.getGraph();
			assert(g[v].kind == lib::DG::HyperVertexKind::Edge);
			std::string res;
			bool first = true;
			for(auto *r : dg.getRulesFromEdge(v)) {
				if(!first) res += this->edgeLabelSep;
				first = false;
				res += r->getName();
			}
			return res;
		});
	}

	setup(options);
	return options;
}

void Printer::postPrint() {
	if(withRuleId) popEdgeLabel();
	if(withRuleName) popEdgeLabel();
	if(withGraphName) popVertexLabel();
}

void Printer::setup(Options &options) {
	for(const auto &s : suffixes) options.suffix += s;

	options.vertexVisible = [this](Vertex v, const lib::DG::Hyper &dg) {
		for(const auto &f : vertexVisibles) {
			if(!f(v, dg)) return false;
		}
		return true;
	};

	options.hyperedgeVisible = [this](Vertex v, const lib::DG::Hyper &dg) {
		for(const auto &f : edgeVisibles) {
			if(!f(v, dg)) return false;
		}
		return true;
	};

	options.vertexLabel = [this](Vertex v, const lib::DG::Hyper &dg) {
		std::string label;
		if(!vertexLabels.empty()) label += vertexLabels.front()(v, dg);
		for(unsigned int i = 1; i < vertexLabels.size(); i++) {
			label += vertexLabelSep;
			label += vertexLabels[i](v, dg);
		}
		if(getConfig().dg.printVertexIds.get()) {
			if(!vertexLabels.empty()) label += vertexLabelSep;
			label += "HyperId: ";
			label += boost::lexical_cast<std::string>(get(boost::vertex_index_t(), dg.getGraph(), v));
		}
		return label;
	};
	options.hyperedgeLabel = [this](Vertex v, const lib::DG::Hyper &dg) {
		std::string label;
		if(!edgeLabels.empty()) label += edgeLabels.front()(v, dg);
		for(unsigned int i = 1; i < edgeLabels.size(); i++) {
			label += edgeLabelSep;
			label += edgeLabels[i](v, dg);
		}
		return label;
	};

	options.vertexColour = [this](Vertex v, const lib::DG::Hyper &dg) {
		for(const auto &fp : vertexColour) {
			std::string colour = fp.first(v, dg);
			if(!colour.empty()) return colour;
		}
		return std::string();
	};


	auto getRawHyperEdgeColour = [this](Vertex v, const lib::DG::Hyper &dg) {
		assert(dg.getGraph()[v].kind == lib::DG::HyperVertexKind::Edge);
		for(const auto &f : edgeColour) {
			std::string colour = f(v, dg);
			if(!colour.empty()) return colour;
		}
		return std::string();
	};

	auto getExtendColour = [this](Vertex v, const lib::DG::Hyper &dg) {
		assert(dg.getGraph()[v].kind == lib::DG::HyperVertexKind::Vertex);
		for(const auto &fp : vertexColour) {
			if(!fp.second) continue;
			std::string colour = fp.first(v, dg);
			if(!colour.empty()) return colour;
		}
		return std::string();
	};

	auto getHyperedgeExtendColour = [getExtendColour](Vertex v, const lib::DG::Hyper &dg) {
		assert(dg.getGraph()[v].kind == lib::DG::HyperVertexKind::Edge);
		std::string tailColour, headColour;
		for(Vertex vAdj : asRange(inv_adjacent_vertices(v, dg.getGraph()))) {
			tailColour = getExtendColour(vAdj, dg);
			if(!tailColour.empty()) break;
		}
		for(Vertex vAdj : asRange(adjacent_vertices(v, dg.getGraph()))) {
			headColour = getExtendColour(vAdj, dg);
			if(!headColour.empty()) break;
		}
		if(!tailColour.empty() && !headColour.empty()) return headColour;
		else return std::string();
	};

	options.hyperedgeColour = [getRawHyperEdgeColour, getHyperedgeExtendColour](Vertex v, const lib::DG::Hyper &dg) {
		assert(dg.getGraph()[v].kind == lib::DG::HyperVertexKind::Edge);
		std::string colour = getRawHyperEdgeColour(v, dg);
		if(!colour.empty()) return colour;
		return getHyperedgeExtendColour(v, dg);
	};
	options.tailColour = [getRawHyperEdgeColour, getExtendColour,
			getHyperedgeExtendColour](Vertex v, Vertex e, const lib::DG::Hyper &dg) {
		assert(dg.getGraph()[v].kind == lib::DG::HyperVertexKind::Vertex);
		assert(dg.getGraph()[e].kind == lib::DG::HyperVertexKind::Edge);
		std::string colour = getRawHyperEdgeColour(e, dg);
		if(!colour.empty()) return colour;
		colour = getHyperedgeExtendColour(e, dg);
		if(colour.empty()) return std::string();
		else return getExtendColour(v, dg);
	};
	options.headColour = [getRawHyperEdgeColour, getExtendColour,
			getHyperedgeExtendColour](Vertex e, Vertex v, const lib::DG::Hyper &dg) {
		assert(dg.getGraph()[v].kind == lib::DG::HyperVertexKind::Vertex);
		assert(dg.getGraph()[e].kind == lib::DG::HyperVertexKind::Edge);
		std::string colour = getRawHyperEdgeColour(e, dg);
		if(!colour.empty()) return colour;
		colour = getHyperedgeExtendColour(e, dg);
		if(colour.empty()) return std::string();
		else return getExtendColour(v, dg);
	};
}

//------------------------------------------------------------------------------
// Algorithms
//------------------------------------------------------------------------------

void generic(const lib::DG::Hyper &dg, const Options &options, SyntaxPrinter &print) {
	using DupVertex = Options::DupVertex;
	using DupEdge = Options::DupEdge;
	const auto imageCreator = print.getImageCreator();
	print.begin();

	// vertices
	detail::forEachVertex(dg, options, print, true, [&dg, &options, &print, imageCreator](DupVertex vDup) {
		Vertex v = options.dupGraph[vDup].v;
		std::string id = options.vDupToId(vDup, dg);
		std::string label = options.getVertexLabel(dg, vDup);
		std::string image;
		if(options.withGraphImages) image = imageCreator(dg, v, id);
		std::string colour = options.getVertexColour(v, dg);
		print.vertex(id, label, image, colour);
	});

	// edges
	detail::forEachExplicitHyperEdge(dg, options, print, [&dg, &options, &print](DupVertex vDup) {
		Vertex v = options.dupGraph[vDup].v;
		std::string id = options.vDupToId(vDup, dg);
		std::string label = options.getHyperedgeLabel(v, dg);
		std::string colour = options.getHyperedgeColour(v, dg);
		print.hyperEdge(id, label, colour);
	});

	// connectors
	auto fTail = [&dg, &options, &print](DupVertex vDup, DupVertex vDupAdj, unsigned int count) {
		Vertex v = options.dupGraph[vDup].v;
		Vertex vAdj = options.dupGraph[vDupAdj].v;

		std::string id = options.vDupToId(vDup, dg);
		std::string idAdj = options.vDupToId(vDupAdj, dg);
		std::string tailColour = options.getTailColour(vAdj, v, dg);
		for(unsigned int i = 1; i <= count; i++)
			print.tailConnector(idAdj, id, tailColour, i, count);
	};
	auto fHead = [&dg, &options, &print](DupVertex vDup, DupVertex vDupAdj, unsigned int count) {
		Vertex v = options.dupGraph[vDup].v;
		Vertex vAdj = options.dupGraph[vDupAdj].v;

		std::string id = options.vDupToId(vDup, dg);
		std::string idAdj = options.vDupToId(vDupAdj, dg);
		std::string headColour = options.getHeadColour(v, vAdj, dg);
		for(unsigned int i = 1; i <= count; i++)
			print.headConnector(id, idAdj, headColour, i, count);
	};
	auto fShortcut = [&dg, &options, &print](DupVertex vDup, DupVertex vDupTail, DupVertex vDupHead, bool hasReverse) {
		Vertex v = options.dupGraph[vDup].v;
		std::string idTail = options.vDupToId(vDupTail, dg);
		std::string idHead = options.vDupToId(vDupHead, dg);
		std::string label = options.getHyperedgeLabel(v, dg);
		std::string colour = options.getHyperedgeColour(v, dg);
		print.shortcutEdge(idTail, idHead, label, colour, hasReverse);
	};
	detail::forEachConnector(dg, options, print, fTail, fHead, fShortcut);
	if(options.auxPrinter) options.auxPrinter(dg, options, print);
	print.end();
}

std::string dot(const lib::DG::Hyper &dg, const Options &options, const IO::Graph::Write::Options &graphOptions) {

	struct DotPrinter : SyntaxPrinter {

		DotPrinter(std::string file, const IO::Graph::Write::Options &graphOptions)
				: SyntaxPrinter(file), graphOptions(graphOptions) {}

		std::string getName() const override {
			return "dot";
		}

		virtual void begin() override {
			s << "digraph g {\n";
			s << getConfig().dg.graphvizCoordsBegin.get() << '\n';
		}

		virtual void end() override {
			s << "}" << std::endl;
		}

		virtual void comment(const std::string &str) override {
			s << "// " << str << std::endl;
		}

		virtual void vertex(const std::string &id,
								  const std::string &label,
								  const std::string &image,
								  const std::string &colour) override {
			s << '"' << id << "\" [ shape=ellipse label=\"" << label << "\"";
			if(!image.empty()) s << " image=\"" << image << "\"";
			if(!colour.empty()) s << " color=\"" << colour << "\"";
			s << " ];" << std::endl;
		}

		virtual void vertexHidden(const std::string &id, bool large) override {
			s << '"' << id << "\" [ shape=point style=invis label=\"\" ];" << std::endl;
		}

		virtual void hyperEdge(const std::string &id, const std::string &label, const std::string &colour) override {
			s << '"' << id << "\" [ shape=box label=\"" << label << "\"";
			if(!colour.empty()) s << "color=\"" << colour << "\"";
			s << " ];" << std::endl;
		}

		void connector(const std::string &idSrc,
							const std::string &idTarget,
							const std::string &colour,
							unsigned int num,
							unsigned int maxNum) {
			// dot handles parallel edges fine, just print the data
			s << '"' << idSrc << "\" -> \"" << idTarget << "\" [";
			if(!colour.empty()) s << "color=\"" << colour << "\"";
			s << " ];" << std::endl;
		}

		virtual void tailConnector(const std::string &idVertex,
											const std::string &idHyperEdge,
											const std::string &colour,
											unsigned int num,
											unsigned int maxNum) override {
			connector(idVertex, idHyperEdge, colour, num, maxNum);
		}

		virtual void headConnector(const std::string &idHyperEdge,
											const std::string &idVertex,
											const std::string &colour,
											unsigned int num,
											unsigned int maxNum) override {
			connector(idHyperEdge, idVertex, colour, num, maxNum);
		}

		virtual void shortcutEdge(const std::string &idTail,
										  const std::string &idHead,
										  const std::string &label,
										  const std::string &colour,
										  bool hasReverse) override {
			s << '"' << idTail << "\" -> \"" << idHead << "\" [ label=\"" << label << "\"";
			if(!colour.empty()) s << " color=\"" << colour << "\"";
			s << " ];" << std::endl;
		}

		virtual std::function<std::string(const lib::DG::Hyper &,
													 lib::DG::HyperVertex,
													 const std::string &)> getImageCreator() override {
			return [this](const lib::DG::Hyper &dg, lib::DG::HyperVertex v, const std::string &id) -> std::string {
				const auto &g = *dg.getGraph()[v].graph;
				return IO::Graph::Write::svg(g, graphOptions);
			};
		}

	public:
		const IO::Graph::Write::Options &graphOptions;
	};
	std::string file = getUniqueFilePrefix();
	file += "dg_" + boost::lexical_cast<std::string>(dg.getNonHyper().getId()) + "_";
	file += options;
	file += ".dot";
	DotPrinter printer(file, graphOptions);
	generic(dg, options, printer);
	return file;
}

std::string coords(const lib::DG::Hyper &dg, const Options &options, const IO::Graph::Write::Options &graphOptions) {
	std::string fileNoExt = dot(dg, options, graphOptions);
	fileNoExt.erase(end(fileNoExt) - 4, end(fileNoExt));
	std::string type = "dgHyper";
	if(getConfig().dg.useDotCoords.get()) type = "dgHyperDot";
	IO::post() << "coordsFromGV " << type << " \"" << fileNoExt << "\"" << std::endl;
	return fileNoExt + "_coord.tex";
}

std::pair<std::string, std::string> tikz(const lib::DG::Hyper &dg,
													  const Options &options,
													  const IO::Graph::Write::Options &graphOptions) {
	std::string fileCoordsExt = coords(dg, options, graphOptions);
	std::string file = getUniqueFilePrefix();
	file += "dg_" + boost::lexical_cast<std::string>(dg.getNonHyper().getId()) + "_";
	file += options;
	file += ".tex";
	TikzPrinter printer(file, fileCoordsExt, options, graphOptions);
	generic(dg, options, printer);
	return std::make_pair(file, fileCoordsExt);
}

std::string pdfFromDot(const lib::DG::Hyper &dg,
							  const Options &options,
							  const IO::Graph::Write::Options &graphOptions) {
	std::string fileNoExt = dot(dg, options, graphOptions);
	fileNoExt.erase(end(fileNoExt) - 4, end(fileNoExt));
	IO::post() << "gv dgHyper \"" << fileNoExt << "\" pdf" << std::endl;
	return fileNoExt + ".pdf";
}

std::pair<std::string, std::string> pdf(const lib::DG::Hyper &dg,
													 const Options &options,
													 const IO::Graph::Write::Options &graphOptions) {
	auto tikzFiles = tikz(dg, options, graphOptions);
	std::string fileNoExt = tikzFiles.first.substr(0, tikzFiles.first.length() - 4);
	std::string fileCoordsNoExt = tikzFiles.second.substr(0, tikzFiles.second.length() - 4);
	IO::post() << "compileTikz \"" << fileNoExt << "\" \"" << fileCoordsNoExt << "\"";
	if(options.withInlineGraphs) IO::post() << " 3";
	IO::post() << std::endl;
	return {fileNoExt + ".pdf", tikzFiles.second};
}

std::pair<std::string, std::string> summary(const Data &data,
														  Printer &printer,
														  const IO::Graph::Write::Options &graphOptions) {
	const auto files = printer.printHyper(data, graphOptions);
	std::string fileNoExt = files.first;
	const auto &dg = data.getDG();
	fileNoExt.erase(end(fileNoExt) - 4, end(fileNoExt));
	IO::post() << "summaryDGHyper \"dg_" << dg.getNonHyper().getId() << "\" \"" << fileNoExt << "\"\n";
	if(getConfig().dg.printNonHyper.get()) {
		std::string fileNoExtNonHyper = pdfNonHyper(dg.getNonHyper());
		IO::post() << "summaryDGNonHyper \"dg_" << dg.getNonHyper().getId() << "\" \"" << fileNoExtNonHyper << "\"\n";
	}
	return files;
}

} // namespace Write
} // namespace DG
} // namespace IO
} // namespace lib
} // namespace mod
