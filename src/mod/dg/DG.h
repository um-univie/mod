#ifndef MOD_DG_DG_H
#define MOD_DG_DG_H

#include <mod/Config.h>
#include <mod/dg/ForwardDecl.h>
#include <mod/graph/ForwardDecl.h>
#include <mod/rule/ForwardDecl.h>

#include <memory>
#include <unordered_set>
#include <vector>

namespace mod {
struct Derivation;
namespace dg {

// rst-class: dg::DG
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
	// rst: .. function:: Vertex findVertex(std::shared_ptr<graph::Graph> g) const
	// rst:
	// rst:		:returns: a vertex descriptor for which the given graph is associated,
	// rst:			or a null descriptor if no such vertex exists.
	Vertex findVertex(std::shared_ptr<graph::Graph> g) const;
	// rst: .. function:: HyperEdge findEdge(const std::vector<Vertex> &sources, const std::vector<Vertex> &targets) const
	// rst:               HyperEdge findEdge(const std::vector<std::shared_ptr<graph::Graph> > &sources, const std::vector<std::shared_ptr<graph::Graph> > &targets) const
	// rst: 
	// rst: 	:returns: a hyperedge with the given sources and targets.
	// rst: 	  If no such hyperedge exists in the derivation graph then a null edge is returned.
	// rst:		  In the second version, the graphs are put through :func:`findVertex` first.
	// rst:		:throws: :class:`LogicError` if a vertex descriptor is null, or does not belong to the derivation graph.
	HyperEdge findEdge(const std::vector<Vertex> &sources, const std::vector<Vertex> &targets) const;
	HyperEdge findEdge(const std::vector<std::shared_ptr<graph::Graph> > &sources, const std::vector<std::shared_ptr<graph::Graph> > &targets) const;
public:
	// rst: .. function:: void calc(bool printInfo = true)
	// rst:
	// rst:		Compute the derivation graph.
	// rst:
	// rst:		:throws: :class:`LogicError` if created from :cpp:any:`ruleComp` and a dynamic add strategy adds a graph
	// rst:			isomorphic to an already known graph, but represented by a different object.
	void calc(bool printInfo = true);
	// rst: .. function:: const std::unordered_set<std::shared_ptr<graph::Graph>, graph::GraphLess> &getGraphDatabase() const
	// rst:
	// rst: 	:returns: the set of all graphs created by the derivation graph,
	// rst: 		and all graphs given when constructed.
	const std::unordered_set<std::shared_ptr<graph::Graph> > &getGraphDatabase() const;
	// rst: .. function:: const std::vector<std::shared_ptr<graph::Graph> > &getProducts() const
	// rst:
	// rst: 	:returns: the list of new graphs discovered by the derivation graph.
	const std::vector<std::shared_ptr<graph::Graph> > &getProducts() const;
	// rst: .. function:: std::pair<std::string, std::string> print(const PrintData &data, const Printer &printer) const
	// rst:
	// rst: 	Print the derivation graph in style of a hypergraph.
	// rst: 	:returns: the name of the PDF-file that will be compiled in post-processing and the name of the coordinate tex-file used.
	// rst: 	:throws: :class:`LogicError` if the print data is not for this DG.
	std::pair<std::string, std::string> print(const PrintData &data, const Printer &printer) const;
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
	std::vector<std::shared_ptr<graph::Graph> > getStratOutputSubset() const;
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
	// rst: .. function:: static std::shared_ptr<DG> ruleComp(const std::vector<std::shared_ptr<graph::Graph> > &graphs, \
	// rst:               std::shared_ptr<Strategy> strategy, LabelSettings labelSettings, bool ignoreRuleLabelTypes)
	// rst:
	// rst: 	Initialize a derivation graph with a :cpp:class:`Strategy` and an initial graph database.
	// rst: 	Any derived graph isomorphic to a given graph is replaced by the given graph.
	// rst:		The given :cpp:class:`LabelSettings` is used for both monomorphism enumeration,
	// rst:		and its :cpp:enum:`LabelType` is used in isomorphism checks.
	// rst:
	// rst:		:throws: :class:`LogicError` if two graphs in :cpp:any:`graphs` are different by isomorphic objects,
	// rst:			or similarly if a static add strategy adds an isomorphic graph.
	// rst:		:throws: :class:`LogicError` if :cpp:any:`ignoreRuleLabelTypes` is `false` and a rule in the given strategy
	// rst:			has an intended label type different from the given type in :cpp:any:`labelSettings`.
	static std::shared_ptr<DG> ruleComp(const std::vector<std::shared_ptr<graph::Graph> > &graphs,
			std::shared_ptr<Strategy> strategy, LabelSettings labelSettings, bool ignoreRuleLabelTypes);
	// rst: .. function:: static std::shared_ptr<DG> dumpImport(const std::vector<std::shared_ptr<graph::Graph> > &graphs, const std::vector<std::shared_ptr<rule::Rule> > &rules, const std::string &file)
	// rst:
	// rst: 	Load a derivation graph dump. Any graph in the dump which is isomorphic to a given graph is replaced by the given graph.
	// rst: 	The same procedure is done for the rules, however only using the name of the rule for comparison.
	// rst:
	// rst: 	:throws: :class:`InputError` on bad input.
	static std::shared_ptr<DG> dumpImport(const std::vector<std::shared_ptr<graph::Graph> > &graphs, const std::vector<std::shared_ptr<rule::Rule> > &rules, const std::string &file);
	// rst: .. function:: static void diff(std::shared_ptr<DG> dg1, std::shared_ptr<DG> dg2)
	// rst:
	// rst: 	Compare two derivation graphs and lists the difference.
	// rst: 	This is not a general isomorphism check; two vertices are equal if they have
	// rst: 	the same graph attached. Edges are equal if the head and tail sets are equal
	// rst: 	and if the attached rule is the same.
	static void diff(std::shared_ptr<DG> dg1, std::shared_ptr<DG> dg2);
};
// rst-class-end:

} // namespace dg
} // namespace mod

#endif /* MOD_DG_DG_H */