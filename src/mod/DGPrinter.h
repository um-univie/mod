#ifndef MOD_DGPRINTER_H
#define MOD_DGPRINTER_H

#include <memory>

namespace mod {
struct DerivationRef;
struct DG;
struct Graph;
struct GraphPrinter;
namespace lib {
namespace DG {
//class Hyper;
//class NonHyper;
} // namespace DG
namespace IO {
namespace DG {
namespace Write {
struct Data;
struct Printer;
} // namespace Write
} // namespace DG
} // namespace IO
} // namespace lib

// rst-class: DGPrintData
// rst:
// rst:		This class is used to hold extra data about how a specific derivation graph
// rst:		is visualised.
// rst:
// rst-class-start:

struct DGPrintData {
	// rst: .. function:: DGPrintData(std::shared_ptr<DG> dg)
	// rst:
	// rst:		Construct a data object where all derivations have a single version, 0, connected to version 0 of all heads and tails.
	DGPrintData(std::shared_ptr<DG> dg);
	DGPrintData(const DGPrintData &other);
	DGPrintData(DGPrintData &&other);
	~DGPrintData();
	lib::IO::DG::Write::Data &getData();
	lib::IO::DG::Write::Data &getData() const;
	// rst: .. function:: std::shared_ptr<DG> getDG() const
	// rst:
	// rst:		:returns: the derivation graph the object holds data for.
	std::shared_ptr<DG> getDG() const;
	// rst: .. function:: void makeDuplicate(DerivationRef dRef, unsigned int eDup)
	// rst:
	// rst:		Create another version of the given derivation and give it the given duplicate number.
	void makeDuplicate(DerivationRef dRef, unsigned int eDup);
	// rst: .. function:: void removeDuplicate(DerivationRef dRef, unsigned int eDup)
	// rst:
	// rst:		Remove the version of the given derivation with the given duplicate number.
	void removeDuplicate(DerivationRef dRef, unsigned int eDup);
	// rst: .. function:: void reconnectTail(DerivationRef dRef, unsigned int eDup, std::shared_ptr<Graph> g, unsigned int vDupTar)
	// rst:
	// rst:		Reconnect an arbitrary version of the tail specified by the given graph in the derivation duplicate given.
	void reconnectTail(DerivationRef dRef, unsigned int eDup, std::shared_ptr<Graph> g, unsigned int vDupTar); // TODO: make overload with explicit source
	// rst: .. function:: void reconnectHead(DerivationRef dRef, unsigned int eDup, std::shared_ptr<Graph> g, unsigned int vDupTar)
	// rst:
	// rst:		Reconnect an arbitrary version of the head specified by the given graph in the derivation duplicate given.
	void reconnectHead(DerivationRef dRef, unsigned int eDup, std::shared_ptr<Graph> g, unsigned int vDupTar); // TODO: make overload with explicit source
private:
	std::shared_ptr<DG> dg;
	std::unique_ptr<lib::IO::DG::Write::Data> data;
};
// rst-class-end:

// rst-class: DGPrinter
// rst:
// rst:		This class is used to configure how derivation graphs are visualised,
// rst:		how much is visualised and which extra properties are printed.
// rst: 
// rst-class-start:

struct DGPrinter {
	DGPrinter();
	DGPrinter(const DGPrinter&) = delete;
	DGPrinter &operator=(const DGPrinter&) = delete;
	~DGPrinter();
	lib::IO::DG::Write::Printer &getPrinter() const;
	// rst: .. function:: GraphPrinter &getGraphPrinter()
	// rst:               const GraphPrinter &getGraphPrinter() const
	// rst:
	// rst:		:returns: a reference to the :class:`GraphPrinter` used for graphs in vertices of the DG.
	GraphPrinter &getGraphPrinter();
	const GraphPrinter &getGraphPrinter() const;
	// rst: .. function:: void setWithShortcutEdges(bool value)
	// rst:               bool getWithShortcutEdges() const
	// rst:
	// rst:		Control whether or not hyperedges :math:`(T, H)` with :math:`|T| = |H| = 1`
	// rst:		are printed as a simple arrow without the usual rectangle.
	void setWithShortcutEdges(bool value);
	bool getWithShortcutEdges() const;
	// rst: .. function:: 	void setWithGraphImages(bool value)
	// rst:                 bool getWithGraphImages() const
	// rst:
	// rst:		Control whether or not each vertex is printed with a image of its graph in it.
	void setWithGraphImages(bool value);
	bool getWithGraphImages() const;
	// rst: .. function:: void setLabelsAsLatexMath(bool value)
	// rst:               bool getLabelsAsLatexMath() const
	// rst:
	// rst:		Control whether or not labels on vertices and hyperedges are put inline :math:`\LaTeX` math.
	// rst:		Specifically, a label ``label`` is written as ``$\mathrm{label'}$``, with ``label'`` being
	// rst:		``label`` with all space characters escaped.
	void setLabelsAsLatexMath(bool value);
	bool getLabelsAsLatexMath() const;
	// rst: .. function:: void pushVertexVisible(std::function<bool(std::shared_ptr<Graph>, std::shared_ptr<DG>)> f)
	// rst:
	// rst:		Add another function controlling the visibility of vertices.
	// rst:		All visibility functions must return ``true`` for a vertex to be visible.
	void pushVertexVisible(std::function<bool(std::shared_ptr<Graph>, std::shared_ptr<DG>) > f);
	// rst: .. function:: void popVertexVisible()
	// rst:
	// rst:		Remove the last pushed vertex visibility function.
	void popVertexVisible();
	// rst: .. function:: void pushEdgeVisible(std::function<bool(DerivationRef) > f)
	// rst:
	// rst:		Add another function controlling the visibility of hyperedges.
	// rst:		All visibility functions must return ``true`` for a hyperedge to be visible.
	void pushEdgeVisible(std::function<bool(DerivationRef) > f);
	// rst: .. function:: void popEdgeVisible()
	// rst:
	// rst:		Remove the last pushed hyperedge visibility function.
	void popEdgeVisible();
	// rst: .. function:: void setWithShortcutEdgesAfterVisibility(bool value)
	// rst:               bool getWithShortcutEdgesAfterVisibility() const
	// rst:
	// rst:		Control whether or not hyperedges are depicted as simple arrows when all but 1 head and tail are hidden.
	void setWithShortcutEdgesAfterVisibility(bool value);
	bool getWithShortcutEdgesAfterVisibility() const;
	// rst: .. function:: void setVertexLabelSep(std::string sep)
	// rst:               const std::string &getVertexLabelSep()
	// rst:
	// rst:		Set/get the string used as separator between each part of each vertex label.
	void setVertexLabelSep(std::string sep);
	const std::string &getVertexLabelSep();
	// rst: .. function:: void setEdgeLabelSep(std::string sep)
	// rst:               const std::string &getEdgeLabelSep()
	// rst:
	// rst:		Set/get the string used as separator between each part of each edge label.
	void setEdgeLabelSep(std::string sep);
	const std::string &getEdgeLabelSep();
	// rst: .. function:: void pushVertexLabel(std::function<std::string(std::shared_ptr<Graph>, std::shared_ptr<DG>)> f)
	// rst:
	// rst:		Add another function for vertex labelling. The result of this function is appended to each label.
	void pushVertexLabel(std::function<std::string(std::shared_ptr<Graph>, std::shared_ptr<DG>) > f);
	// rst: .. function:: void popVertexLabel()
	// rst:
	// rst:		Remove the last pushed vertex labelling function.
	void popVertexLabel();
	// rst: .. function:: void pushEdgeLabel(std::function<std::string(DerivationRef)> f)
	// rst:
	// rst:		Add another function for edge labelling. The result of this function is appended to each label.
	void pushEdgeLabel(std::function<std::string(DerivationRef) > f);
	// rst: .. function:: void popEdgeLabel()
	// rst:
	// rst:		Remove the last pushed edge labelling function.
	void popEdgeLabel();
	// rst: .. function:: void setWithGraphName(bool value)
	// rst:               bool getWithGraphName() const
	// rst:
	// rst:		Control whether or not graph names are appended to the vertex labels (see also :func:`pushVertexLabel`).
	void setWithGraphName(bool value);
	bool getWithGraphName() const;
	// rst: .. function:: void setWithRuleName(bool value)
	// rst:               bool getWithRuleName() const
	// rst:
	// rst:		Control whether or not rules names are appended to the edge labels (see also :func:`pushEdgeLabel`).
	void setWithRuleName(bool value);
	bool getWithRuleName() const;
	// rst: .. function:: void setWithRuleId(bool value)
	// rst:               bool getWithRuleId() const
	// rst:
	// rst:		Control whether or not rules IDs are appended to the edge labels (see also :func:`pushEdgeLabel`).
	void setWithRuleId(bool value);
	bool getWithRuleId() const;
	// rst: .. function:: void pushVertexColour(std::function<std::string(std::shared_ptr<Graph>, std::shared_ptr<DG>)> f, bool extendToEdges)
	// rst:
	// rst:		Add another function for colouring vertices. The final colour of a vertex is the result of the first colour function returning a non-empty string.
	// rst:		The functions are evaluated in the order they are pushed and the resulting string is used directly as a colour in Tikz.
	// rst:		A hyperedge is also coloured if at least one head and one tail *can* be coloured with a colour for which ``extendToEdges`` is ``true``.
	// rst:		In this case, the hyperedge (and a subset of the head and tail connectors) is coloured with the first applicable colour.
	// rst:		The edge extension of vertex colour takes lower precedence than explicitly added hyperedge colouring functions.
	void pushVertexColour(std::function<std::string(std::shared_ptr<Graph>, std::shared_ptr<DG>) > f, bool extendToEdges);
	// rst: .. function:: void popVertexColour()
	// rst:
	// rst:		Remove the last pushed vertex colouring function.
	void popVertexColour();
	// rst: .. function:: void pushEdgeColour(std::function<std::string(DerivationRef)> f)
	// rst:
	// rst:		Add another function for colouring hyperedges. The final colour of a hyperedge (and all of its head and tail connectors) is the result of the
	// rst:		first colour function returning a non-empty string.
	void pushEdgeColour(std::function<std::string(DerivationRef) > f);
	// rst: .. function:: void popEdgeColour()
	// rst:
	// rst:		Remove the last pushed hyperedge colouring function.
	void popEdgeColour();
private:
	std::unique_ptr<GraphPrinter> graphPrinter;
	std::unique_ptr<lib::IO::DG::Write::Printer> printer;
};
// rst-class-end:

} // namespace mod

#endif /* MOD_DGPRINTER_H */