#ifndef MOD_LIB_IO_DG_H
#define MOD_LIB_IO_DG_H

#include <mod/lib/DG/Hyper.h>
#include <mod/lib/IO/FileHandle.h>
#include <mod/lib/IO/Graph.h>
#include <mod/lib/IO/IO.h>

#include <iosfwd>
#include <string>
#include <unordered_map>

namespace mod {
namespace lib {
namespace DG {
class Hyper;
class NonHyper;
} // namespace DG
namespace IO {
namespace DG {
namespace Read {
lib::DG::NonHyper *dump(const std::vector<std::shared_ptr<graph::Graph> > &graphs, const std::vector<std::shared_ptr<rule::Rule> > &rules, const std::string &file, std::ostream &err);
lib::DG::NonHyper *abstract(const std::string &s, std::ostream &err);
} // namespace Read
namespace Write {
using Vertex = lib::DG::HyperVertex;
using Edge = lib::DG::HyperEdge;

std::string dump(const lib::DG::NonHyper &dg);

std::string dotNonHyper(const lib::DG::NonHyper &nonHyper);
std::string pdfNonHyper(const lib::DG::NonHyper &nonHyper);

//------------------------------------------------------------------------------
// Old/New delimiter
//------------------------------------------------------------------------------

struct Options;

struct SyntaxPrinter {
	virtual void begin() = 0;
	virtual void end() = 0;
	virtual void comment(const std::string &str) = 0;
	virtual void vertex(const std::string &id, const std::string &label, const std::string &image, const std::string &colour) = 0;
	virtual void vertexHidden(const std::string &id, bool large) = 0;
	virtual void hyperEdge(const std::string &id, const std::string &label, const std::string &colour) = 0;
	virtual void tailConnector(const std::string &idVertex, const std::string &idHyperEdge, const std::string &colour, unsigned int num, unsigned int maxNum) = 0;
	virtual void headConnector(const std::string &idHyperEdge, const std::string &idVertex, const std::string &colour, unsigned int num, unsigned int maxNum) = 0;
	virtual void shortcutEdge(const std::string &idTail, const std::string &idHead, const std::string &label, const std::string &colour, bool hasReverse) = 0;
	virtual std::function<std::string(const lib::DG::Hyper&, lib::DG::HyperVertex, const std::string&) > getImageCreator() = 0;
};

struct TikzPrinter : SyntaxPrinter {

	TikzPrinter(std::string file, std::string coords, const Options &options, const IO::Graph::Write::Options &graphOptions)
	: s(file), coords(coords), options(options), graphOptions(graphOptions) { }
	void begin();
	void end();
	void comment(const std::string &str);
	void vertex(const std::string &id, const std::string &label, const std::string &image, const std::string &colour);
	void vertexHidden(const std::string &id, bool large);
	void transitVertex(const std::string &idHost, const std::string &idTransit, const std::string &angle, const std::string &label);
	void hyperEdge(const std::string &id, const std::string &label, const std::string &colour);
	void connector(const std::string &idTail, const std::string &idHead, const std::string &colour, unsigned int num, unsigned int maxNum);
	void tailConnector(const std::string &idVertex, const std::string &idHyperEdge, const std::string &colour, unsigned int num, unsigned int maxNum);
	void headConnector(const std::string &idHyperEdge, const std::string &idVertex, const std::string &colour, unsigned int num, unsigned int maxNum);
	void shortcutEdge(const std::string &idTail, const std::string &idHead, const std::string &label, const std::string &colour, bool hasReverse);
	void transitEdge(const std::string &idTail, const std::string &idHead, const std::string &label, const std::string &colour);
	std::function<std::string(const lib::DG::Hyper&, lib::DG::HyperVertex, const std::string&) > getImageCreator();
public:
	FileHandle s;
	std::string coords;
	const Options &options;
	const IO::Graph::Write::Options &graphOptions;
};

struct Options {

	struct DupVProp {
		Vertex v;
		unsigned int dupNum;
	};
	using DupGraphType = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, DupVProp>;
	using DupVertex = boost::graph_traits<DupGraphType>::vertex_descriptor;
	using DupEdge = boost::graph_traits<DupGraphType>::edge_descriptor;
public:

	Options() : withShortcutEdges(true), withGraphImages(true), labelsAsLatexMath(true),
	withShortcutEdgesAfterVisibility(false), withInlineGraphs(false) { }

	Options &Non() {
		return WithShortcutEdges(false).WithGraphImages(false).LabelsAsLatexMath(false);
	}

	Options &All() {
		return WithShortcutEdges(true).WithGraphImages(true).LabelsAsLatexMath(true);
	}

	Options &WithShortcutEdges(bool v) {
		withShortcutEdges = v;
		return *this;
	}

	Options &WithGraphImages(bool v) {
		withGraphImages = v;
		return *this;
	}

	Options &LabelsAsLatexMath(bool v) {
		labelsAsLatexMath = v;
		return *this;
	}

	Options &WithShortcutEdgesAfterVisibility(bool v) {
		withShortcutEdgesAfterVisibility = v;
		return *this;
	}

	Options &WithInlineGraphs(bool v) {
		withInlineGraphs = v;
		return *this;
	}

	Options &Suffix(std::string s) {
		suffix = s;
		return *this;
	}

	operator std::string() const {
		auto toChar = [](bool b) {
			return b ? '1' : '0';
		};
		std::string res;
		res += toChar(withShortcutEdges);
		res += toChar(withGraphImages);
		res += toChar(labelsAsLatexMath);
		res += toChar(withShortcutEdgesAfterVisibility);
		res += toChar(withInlineGraphs);
		if(!suffix.empty()) res += "_" + suffix;
		return res;
	}

	// stuff not giving state

	bool isVertexVisible(Vertex v, const lib::DG::Hyper &dg) const {
		return vertexVisible ? vertexVisible(v, dg) : true;
	}

	std::string getVertexLabel(const lib::DG::Hyper &dg, DupVertex vDup) const {
		std::string label;
		Vertex v = dupGraph[vDup].v;
		auto dupNum = dupGraph[vDup].dupNum;
		if(dupVertexLabel) label += dupVertexLabel(v, dupNum, dg);
		if(dupVertexLabel && vertexLabel) label += ", ";
		if(vertexLabel) label += vertexLabel(v, dg);
		return label;
	}

	bool isHyperedgeVisible(Vertex v, const lib::DG::Hyper &dg) const {
		return hyperedgeVisible ? hyperedgeVisible(v, dg) : true;
	}

	std::string getHyperedgeLabel(Vertex v, const lib::DG::Hyper &dg) const {
		return hyperedgeLabel ? hyperedgeLabel(v, dg) : "";
	}

	std::string getVertexColour(Vertex v, const lib::DG::Hyper &dg) const {
		return vertexColour ? vertexColour(v, dg) : "";
	}

	std::string getHyperedgeColour(Vertex v, const lib::DG::Hyper &dg) const {
		return hyperedgeColour ? hyperedgeColour(v, dg) : "";
	}

	std::string getTailColour(Vertex v, Vertex e, const lib::DG::Hyper &dg) const {
		return tailColour ? tailColour(v, e, dg) : "";
	}

	std::string getHeadColour(Vertex e, Vertex v, const lib::DG::Hyper &dg) const {
		return headColour ? headColour(e, v, dg) : "";
	}

	std::pair<unsigned int, DupVertex> inDegreeVisible(DupVertex e, const lib::DG::Hyper &dg) const;
	std::pair<unsigned int, DupVertex> outDegreeVisible(DupVertex e, const lib::DG::Hyper &dg) const;
	bool isShortcutEdge(DupVertex e, const lib::DG::Hyper &dg, unsigned int inDegreeVisible, unsigned int outDegreeVisible) const;
	std::string vDupToId(DupVertex vDup, const lib::DG::Hyper &dg) const;
public:
	bool withShortcutEdges;
	bool withGraphImages;
	bool labelsAsLatexMath;
	bool withShortcutEdgesAfterVisibility;
	bool withInlineGraphs;
	std::string suffix;
	// appearance (not giving state)
	std::function<bool(Vertex, const lib::DG::Hyper&) > vertexVisible;
	std::function<std::string(Vertex, const lib::DG::Hyper&) > vertexLabel;
	std::function<std::string(Vertex, unsigned int dupNum, const lib::DG::Hyper&) > dupVertexLabel;
	std::function<bool(Vertex, const lib::DG::Hyper&) > hyperedgeVisible;
	std::function<std::string(Vertex, const lib::DG::Hyper&) > hyperedgeLabel;
	std::function<std::string(Vertex, const lib::DG::Hyper&) > vertexColour;
	std::function<std::string(Vertex, const lib::DG::Hyper&) > hyperedgeColour;
	std::function<std::string(Vertex, Vertex, const lib::DG::Hyper&) > tailColour, headColour;
	std::function<void(const lib::DG::Hyper&, const Options&, SyntaxPrinter&) > auxPrinter;
	// duplication (not giving state)
	// all vertices must be first and then all edges
	// all incarnations must be contiguous and in increasing order
	// this is only for prettier tikz code
	// Eh, what? it should correspond to what the user specified
	DupGraphType dupGraph;
};

struct Data {

	struct Connections {

		Connections(unsigned int numTails, unsigned int numHeads) : tail(numTails, 0), head(numHeads, 0) { }
		// indices are offsets on the in-/out-edge iterators
		// values are the duplicate numbers
		std::vector<unsigned int> tail;
		std::vector<unsigned int> head;
	};
	using ConnectionsStore = std::unordered_map<Vertex, std::unordered_map<unsigned int, Connections> >;
public:
	explicit Data(const lib::DG::Hyper &dg);
	void compile(Options &options) const;
	void makeDuplicate(Vertex e, unsigned int eDup);
	void removeDuplicate(Vertex e, unsigned int eDup);
	void reconnectTail(Vertex e, unsigned int eDup, Vertex tail, unsigned int vDupTar, unsigned int vDupSrc);
	void reconnectHead(Vertex e, unsigned int eDup, Vertex head, unsigned int vDupTar, unsigned int vDupSrc);
	void removeVertexIfDegreeZero(Vertex v);

	const lib::DG::Hyper &getDG() const {
		return dg;
	}
private:
	const lib::DG::Hyper &dg;
	ConnectionsStore connections; // hyper-edge -> dupNum -> Connections
	std::set<Vertex> removedIfDegreeZero;
public:
	std::function<std::string(Vertex, unsigned int dupNum, const lib::DG::Hyper&) > dupVertexLabel;
};

struct Printer {
	Printer();
	std::string printHyper(const Data &data, const IO::Graph::Write::Options &graphOptions);
	void pushSuffix(const std::string suffix);
	void popSuffix();
	void pushVertexVisible(std::function<bool(Vertex, const lib::DG::Hyper&) > f); // visible(v) <=> all of pushed f(v))
	void popVertexVisible();
	void pushEdgeVisible(std::function<bool(Vertex, const lib::DG::Hyper&) > f); // visible(v) <=> all of pushed f(v))
	void popEdgeVisible();
	void pushVertexLabel(std::function<std::string(Vertex, const lib::DG::Hyper&) > f);
	void popVertexLabel();
	void pushEdgeLabel(std::function<std::string(Vertex, const lib::DG::Hyper&) > f);
	void popEdgeLabel();
	void pushVertexColour(std::function<std::string(Vertex, const lib::DG::Hyper&) > f, bool extendToEdges); // colour(v) == first f(v) != ""
	void popVertexColour();
	void pushEdgeColour(std::function<std::string(Vertex, const lib::DG::Hyper&) > f); // colour(v) == first f(v) != ""
	void popEdgeColour();
public:
	Options prePrint(const Data &data);
	void postPrint();
private:
	void setup(Options &options);
public:
	Options baseOptions; // only save stuff giving state, dynamically add the rest
private:
	std::vector<std::string> suffixes;
	// not giving state
	std::vector<std::function<bool(Vertex, const lib::DG::Hyper&)> > vertexVisibles, edgeVisibles;
	std::vector<std::function<std::string(Vertex, const lib::DG::Hyper&)> > vertexLabels, edgeLabels;
	std::vector<std::pair<std::function<std::string(Vertex, const lib::DG::Hyper&)>, bool> > vertexColour;
	std::vector<std::function<std::string(Vertex, const lib::DG::Hyper&)> > edgeColour;
public:
	std::string vertexLabelSep, edgeLabelSep;
	bool withGraphName, withRuleName, withRuleId;
};

void generic(const lib::DG::Hyper &dg, const Options &options, SyntaxPrinter &print);
std::string dot(const lib::DG::Hyper &dg, const Options &options, const IO::Graph::Write::Options &graphOptions);
std::string coords(const lib::DG::Hyper &dg, const Options &options, const IO::Graph::Write::Options &graphOptions);
std::pair<std::string, std::string> tikz(const lib::DG::Hyper &dg, const Options &options, const IO::Graph::Write::Options &graphOptions);
std::string pdfFromDot(const lib::DG::Hyper &dg, const Options &options, const IO::Graph::Write::Options &graphOptions);
std::string pdf(const lib::DG::Hyper &dg, const Options &options, const IO::Graph::Write::Options &graphOptions);
std::string summary(const Data &data, Printer &printer, const IO::Graph::Write::Options &graphOptions);

} // namespace Write
} // namespace DG
} // namespace IO
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_IO_DG_H */

