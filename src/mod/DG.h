#ifndef MOD_DG_H
#define MOD_DG_H

#include <mod/Config.h>

#include <memory>
#include <set>
#include <vector>

namespace mod {
struct Derivation;
struct DGPrintData;
struct DGPrinter;
struct DGStrat;
struct Graph;
struct GraphLess;
struct GraphPrinter; // DerivationRef
struct Rule;
namespace lib {
namespace DG {
struct Hyper;
struct NonHyper;
} // namespace DG
} // namespace lib
//class DerivationRef;

// rst-class: DG
// rst:
// rst:		The main derivation graph class. A derivation graph is a directed hypergraph
// rst:		:math:`\mathcal{H} = (V, E)`.
// rst:		Each vertex is annotated with a graph, and each hyperedge is annotated with a transformation rule.
// rst:		A derivation graph can either be calculated from a strategy or loaded from external data.
// rst: 
// rst-class-start:

struct DG {
	DG(const DG&) = delete;
	DG &operator=(const DG&) = delete;
public:
	class Vertex;
	class HyperEdge;
	class VertexIterator;
	class VertexRange;
	class EdgeIterator;
	class EdgeRange;
	class InEdgeIterator;
	class InEdgeRange;
	class OutEdgeIterator;
	class OutEdgeRange;
	class SourceIterator;
	class SourceRange;
	class TargetIterator;
	class TargetRange;
	class RuleIterator;
	class RuleRange;
private:
	DG(std::unique_ptr<lib::DG::NonHyper> dg);
public:
	~DG();
	// rst: .. function:: std::size_t getId() const
	// rst:
	// rst: 	:returns: the instance identifier for the object.
	std::size_t getId() const;
	friend std::ostream &operator<<(std::ostream &s, const DG &dg);
	// rst: .. function:: const lib::DG::NonHyper &getNonHyper() const
	// rst:               const lib::DG::Hyper &getHyper() const
	// rst: 
	// rst: 	:returns: the internal data structures of the derivation graph.
	const lib::DG::NonHyper &getNonHyper() const;
	const lib::DG::Hyper &getHyper() const;
public: // hypergraph interface
	// rst: .. function:: std::size_t numVertices() const
	// rst:
	// rst:		:returns: the number of vertices in the derivation graph.
	std::size_t numVertices() const;
	// rst: .. function:: VertexRange vertices() const
	// rst:
	// rst:		:returns: a range of all vertices in the derivation graph.
	VertexRange vertices() const;
	// rst: .. function:: std::size_t numEdges() const
	// rst:
	// rst:		:returns: the number of edges in the derivation graph.
	std::size_t numEdges() const;
	// rst: .. function:: EdgeRange edges() const
	// rst:
	// rst:		:returns: a range of all edges in the derivation graph.
	EdgeRange edges() const;
public: // searching for vertices and hyperedges
	// rst: .. function:: Vertex findVertex(std::shared_ptr<Graph> g) const
	// rst:
	// rst:		:returns: a vertex descriptor for which the given graph is associated,
	// rst:			or a null descriptor if no such vertex exists.
	Vertex findVertex(std::shared_ptr<Graph> g) const;
	// rst: .. function:: HyperEdge findEdge(const std::vector<Vertex> &sources, const std::vector<Vertex> &targets) const
	// rst:               HyperEdge findEdge(const std::vector<std::shared_ptr<Graph> > &sources, const std::vector<std::shared_ptr<Graph> > &targets) const
	// rst: 
	// rst: 	:returns: a hyperedge with the given sources and targets.
	// rst: 	  If no such hyperedge exists in the derivation graph then a null edge is returned.
	// rst:		  In the second version, the graphs are put through :func:`findVertex` first.
	// rst:		:throws: :class:`LogicError` if a vertex descriptor is null, or does not belong to the derivation graph.
	HyperEdge findEdge(const std::vector<Vertex> &sources, const std::vector<Vertex> &targets) const;
	HyperEdge findEdge(const std::vector<std::shared_ptr<Graph> > &sources, const std::vector<std::shared_ptr<Graph> > &targets) const;
public:
	// rst: .. function:: void calc()
	// rst:
	// rst:	 	Compute the derivation graph.
	// rst:
	void calc();
	// rst: .. function:: const std::set<std::shared_ptr<Graph>, GraphLess> &getGraphDatabase() const
	// rst:
	// rst: 	:returns: the set of all graphs created by the derivation graph,
	// rst: 		and all graphs given when constructed.
	const std::set<std::shared_ptr<Graph>, GraphLess> &getGraphDatabase() const;
	// rst: .. function:: const std::vector<std::shared_ptr<Graph> > &getProducts() const
	// rst:
	// rst: 	:returns: the list of new graphs discovered by the derivation graph.
	const std::vector<std::shared_ptr<Graph> > &getProducts() const;
	// rst: .. function:: void print(const DGPrintData &data, const DGPrinter &printer) const
	// rst:
	// rst: 	Print the derivation graph in style of a hypergraph.
	// rst:
	// rst: 	:throws: :class:`LogicError` if the print data is not for this DG.
	void print(const DGPrintData &data, const DGPrinter &printer) const;
	// rst: .. function:: std::string dump() const
	// rst:
	// rst: 	Exports the derivation graph to a text file, which can be importetet.
	// rst:
	// rst: 	:returns: the name of the file with the exported data.
	// rst: 	:throws: :class:`LogicError` if the DG has not been calculated.
	std::string dump() const;
	// rst: .. function:: void list() const
	// rst:
	// rst: 	Output information on the calculation of the derivation graph.
	// rst: 	For strategy-based calculations, this outputs the expression tree.
	void list() const;
	// rst: .. function:: void listStats() const
	// rst: 
	// rst: 	Output various stats of the derivation graph.
	// rst:
	// rst: 	:throws: :class:`LogicError` if the DG has not been calculated.
	void listStats() const;
	// :throws: :class:`LogicError` if the DG has not been calculated or if the DG is not created from strategies.
	std::vector<std::shared_ptr<Graph> > getStratOutputSubset() const;
private:
	struct Pimpl;
	std::unique_ptr<Pimpl> p;
public:
	// rst: .. function:: static std::shared_ptr<DG> derivations(const std::vector<Derivation> &derivations)
	// rst:
	// rst: 	Create a derivation graph from a list of :class:`Derivation`.
	static std::shared_ptr<DG> derivations(const std::vector<Derivation> &derivations);
	// rst: .. function:: static std::shared_ptr<DG> abstract(const std::string &specification)
	// rst:
	// rst: 	Create a derivation graph from an abstract description according to the following grammar.
	// rst:
	// rst: 	.. productionlist::
	// rst: 		description: `derivation` { `derivation` }
	// rst: 		derivation: `side` ("->" | "<=>") `side`
	// rst:	 		side: `term` { "+" `term` }
	// rst: 		term: [ `unsignedInt` ] `identifier`
	// rst:
	// rst: 	:throws: :class:`InputError` on bad input.
	static std::shared_ptr<DG> abstract(const std::string &specification);
	// rst: .. function:: static std::shared_ptr<DG> ruleComp(const std::vector<std::shared_ptr<Graph> > &graphs, \
	// rst:               std::shared_ptr<DGStrat> strategy)
	// rst:
	// rst: 	Initialize a derivation graph with a :cpp:class:`DGStrat` and an initial graph database.
	// rst: 	Any derived graph isomorphic to a given graph is replaced by the given graph.
	static std::shared_ptr<DG> ruleComp(const std::vector<std::shared_ptr<Graph> > &graphs,
			std::shared_ptr<DGStrat> strategy);
	// rst: .. function:: static std::shared_ptr<DG> dumpImport(const std::vector<std::shared_ptr<Graph> > &graphs, const std::vector<std::shared_ptr<Rule> > &rules, const std::string &file)
	// rst:
	// rst: 	Load a derivation graph dump. Any graph in the dump which is isomorphic to a given graph is replaced by the given graph.
	// rst: 	The same procedure is done for the rules, however only using the name of the rule for comparison.
	// rst:
	// rst: 	:throws: :class:`InputError` on bad input.
	static std::shared_ptr<DG> dumpImport(const std::vector<std::shared_ptr<Graph> > &graphs, const std::vector<std::shared_ptr<Rule> > &rules, const std::string &file);
	// rst: .. function:: static void diff(std::shared_ptr<DG> dg1, std::shared_ptr<DG> dg2)
	// rst:
	// rst: 	Compare two derivation graphs and lists the difference.
	// rst: 	This is not a general isomorphism check; two vertices are equal if they have
	// rst: 	the same graph attached. Edges are equal if the head and tail sets are equal
	// rst: 	and if the attached rule is the same.
	static void diff(std::shared_ptr<DG> dg1, std::shared_ptr<DG> dg2);
};
// rst-class-end:

// rst-class: DerivationRef
// rst:
// rst:		The class represents a reference to a derivation in a derivation graph.
// rst:		A reference may be invalid.
// rst: 
// rst-class-start:

class DerivationRef {
	friend class lib::DG::Hyper;
	DerivationRef(unsigned int id, std::shared_ptr<DG> dg);
public:
	// rst: .. function:: DerivationRef()
	// rst:
	// rst:		Constructor for an invalid reference.
	DerivationRef();
	DerivationRef(DG::HyperEdge e);
	operator DG::HyperEdge() const;
	friend std::ostream &operator<<(std::ostream &s, const DerivationRef &d);
	// rst: .. function:: unsigned int getId() const
	// rst:
	// rst:		:returns: the id of the derivation.
	unsigned int getId() const;
	// rst: .. function:: std::shared_ptr<DG> getDG() const
	// rst:
	// rst:		:returns: the derivation graph the reference is pointing into.
	std::shared_ptr<DG> getDG() const;
	// rst: .. function:: bool isValid() const
	// rst:
	// rst:		:returns: ``true`` iff the reference is valid.
	bool isValid() const;
	// rst: .. function:: Derivation operator*() const
	// rst:
	// rst:		:returns: the derivation represented by the reference.
	Derivation operator*() const;
	// rst: .. function:: void print(const GraphPrinter &printer, const std::string &matchColour) const
	// rst:
	// rst:		Print the derivation pointed to by the derivation reference.
	// rst:		All possible Double-Pushout diagrams for the derivation are printed.
	// rst:		The ``matchColour`` must be a valid colour for TikZ, which is applied to the rule
	// rst:		and its image in the bottom span.
	void print(const GraphPrinter &printer, const std::string &matchColour) const;
	bool operator<(const DerivationRef &other) const;

	friend bool operator==(const DerivationRef &a, const DerivationRef &b) {
		return a.id == b.id && a.dg == b.dg;
	}
private:
	unsigned int id;
	std::shared_ptr<DG> dg;
};
// rst-class-end:

} // namespace mod

#endif /* MOD_DG_H */