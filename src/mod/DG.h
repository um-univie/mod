#ifndef MOD_DG_H
#define MOD_DG_H

#include <mod/Config.h>

#include <boost/iterator/iterator_facade.hpp>

#include <iosfwd>
#include <memory>
#include <set>
#include <vector>

namespace mod {
class Derivation;
class DG;
class DGPrintData;
class DGPrinter;
class DGStrat;
template<typename T> class Function;
class Graph;
class GraphLess;
class GraphPrinter;
class Rule;
namespace lib {
namespace DG {
class Hyper;
class NonHyper;
} // namespace DG
namespace IO {
namespace DG {
namespace Write {
class Data;
class Printer;
} // namespace Write
} // namespace DG
} // namespace IO
} // namespace lib

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
	class HyperEdge;
	class VertexIterator;
	class EdgeIterator;
	class InEdgeIterator;
	class OutEdgeIterator;
	class InEdgeRange;
	class OutEdgeRange;
	class SourceRange;
	class TargetRange;
	class SourceIterator;
	class TargetIterator;
	class RuleRange;
	// rst-nested: DG::Vertex
	// rst:
	// rst:		A descriptor of either a vertex in a graph, or a null vertex.
	// rst-nested-start:

	class Vertex {
		friend class HyperEdge;
		friend class VertexIterator;
		friend class SourceIterator;
		friend class TargetIterator;
		Vertex(std::shared_ptr<DG> g, std::size_t vId);
	public:
		// rst:		.. function:: Vertex()
		// rst:
		// rst:			Constructs a null descriptor.
		Vertex();
		friend std::ostream &operator<<(std::ostream &s, const Vertex &v);
		friend bool operator==(const Vertex &v1, const Vertex &v2);
		friend bool operator!=(const Vertex &v1, const Vertex &v2);
		// rst:		.. function:: std::size_t getId() const
		// rst:
		// rst:			:returns: the index of the vertex. It will be in the range :math:`[0, numVertices + numEdges[`.
		// rst:			:throws: :cpp:class:`LogicError` if it is a null descriptor.
		std::size_t getId() const;
		// rst:		.. function:: std::shared_ptr<DG> getDG() const
		// rst:
		// rst:			:returns: the derivation graph the vertex belongs to.
		// rst:			:throws: :cpp:class:`LogicError` if it is a null descriptor.
		std::shared_ptr<DG> getDG() const;
		// rst:		.. function:: std::size_t inDegree() const
		// rst:
		// rst:			:returns: the in-degree of the vertex, including multiplicity of target multisets.
		// rst:			:throws: :cpp:class:`LogicError` if it is a null descriptor.
		std::size_t inDegree() const;
		// rst:		.. function:: InEdgeRange inEdges() const
		// rst:
		// rst:			:returns: a range of in-hyperedges for this vertex.
		// rst:			:throws: :cpp:class:`LogicError` if it is a null descriptor.
		InEdgeRange inEdges() const;
		// rst:		.. function:: std::size_t getOutDegree() const
		// rst:
		// rst:			:returns: the out-degree of the vertex, including multiplicity of source multisets.
		// rst:			:throws: :cpp:class:`LogicError` if it is a null descriptor.
		std::size_t outDegree() const;
		// rst:		.. function:: OutEdgeRange outEdges() const
		// rst:
		// rst:			:returns: a range of out-hyperedges for this vertex.
		// rst:			:throws: :cpp:class:`LogicError` if it is a null descriptor.
		OutEdgeRange outEdges() const;
		// rst:		.. function:: std::shared_ptr<Graph> &getGraph() const
		// rst:
		// rst: 		:returns: the graph label of the vertex.
		// rst: 		:throws: :cpp:class:`LogicError` if it is a null descriptor.
		std::shared_ptr<Graph> getGraph() const;
	private:
		std::shared_ptr<DG> g;
		std::size_t vId;
	};
	// rst-nested-end:

	// rst-nested: DG::HyperEdge
	// rst:
	// rst:		A descriptor of either a hyperedge in a derivation graph, or a null edge.
	// rst-nested-start:

	class HyperEdge {
		friend class EdgeIterator;
		friend class InEdgeIterator;
		friend class OutEdgeIterator;
		HyperEdge(std::shared_ptr<DG> g, std::size_t eId);
	public:
		// rst:		.. function:: HyperEdge()
		// rst:
		// rst:			Constructs a null descriptor.
		HyperEdge();
		friend std::ostream &operator<<(std::ostream &s, const HyperEdge &e);
		friend bool operator==(const HyperEdge &e1, const HyperEdge &e2);
		friend bool operator!=(const HyperEdge &e1, const HyperEdge &e2);
		// rst:		.. function:: std::size_t getId() const
		// rst:
		// rst:			:returns: the index of the hyperedge. It will be in the range :math:`[0, numVertices + numEdges[`.
		// rst:			:throws: :cpp:class:`LogicError` if it is a null descriptor.
		std::size_t getId() const;
		// rst:		.. function:: std::shared_ptr<DG> getDG() const
		// rst:
		// rst:			:returns: the derivation graph the hyperedge belongs to.
		// rst:			:throws: :cpp:class:`LogicError` if it is a null descriptor.
		std::shared_ptr<DG> getDG() const;
		// rst:		.. function:: std::size_t numSources() const
		// rst:
		// rst:			:returns: the number of sources of the hyperedge.
		// rst: 		:throws: :cpp:class:`LogicError` if it is a null descriptor.
		std::size_t numSources() const;
		// rst:		.. function:: SourceRange sources() const
		// rst:
		// rst:			:returns: the sources of the hyperedge.
		// rst: 		:throws: :cpp:class:`LogicError` if it is a null descriptor.
		SourceRange sources() const;
		// rst:		.. function:: std::size_t numTargets() const
		// rst:
		// rst:			:returns: the number of targets of the hyperedge.
		// rst: 		:throws: :cpp:class:`LogicError` if it is a null descriptor.
		std::size_t numTargets() const;
		// rst:		.. function:: TargetRange targets() const
		// rst:
		// rst:			:returns: the targets of the hyperedge.
		// rst: 		:throws: :cpp:class:`LogicError` if it is a null descriptor.
		TargetRange targets() const;
		// rst:		.. function:: RuleRange rules() const
		// rst:
		// rst: 		:returns: a range of the rules associated with the hyperedge.
		// rst: 		:throws: :cpp:class:`LogicError` if it is a null descriptor.
		RuleRange rules() const;
	private:
		std::shared_ptr<DG> g;
		std::size_t eId;
	};
	// rst-nested-end:
public: // VertexList
	// rst-nested: DG::VertexIterator
	// rst:
	// rst:		An iterator for traversing all vertices in a derivation graph.
	// rst:		It models a forward iterator.
	// rst-nested-start:

	class VertexIterator : public boost::iterator_facade<VertexIterator, Vertex, std::forward_iterator_tag, Vertex> {
		friend class DG;
		VertexIterator(std::shared_ptr<DG> g);
	public:
		// rst:		.. function:: VertexIterator()
		// rst:
		// rst:			Construct a past-the-end iterator.
		VertexIterator();
	private:
		friend class boost::iterator_core_access;
		Vertex dereference() const;
		bool equal(const VertexIterator &iter) const;
		void increment();
	private:
		std::shared_ptr<DG> g;
		std::size_t vId;
	};
	// rst-nested-end:

	// rst-nested: DG::VertexRange
	// rst:
	// rst:		A range of all vertices in a derivation graph.
	// rst-nested-start:

	struct VertexRange {
		using iterator = VertexIterator;
		using const_iterator = iterator;
	private:
		friend class DG;
		VertexRange(std::shared_ptr<DG> g);
	public:
		VertexIterator begin() const;
		VertexIterator end() const;
	private:
		std::shared_ptr<DG> g;
	};
	// rst-nested-end:
public: // EdgeList
	// rst-nested: DG::EdgeIterator
	// rst:
	// rst:		An iterator for traversing all edges in a graph.
	// rst:		It models a forward iterator.
	// rst-nested-start:

	class EdgeIterator : public boost::iterator_facade<EdgeIterator, HyperEdge, std::forward_iterator_tag, HyperEdge> {
		friend class DG;
		EdgeIterator(std::shared_ptr<DG> g);
	public:
		// rst:		.. function:: EdgeIterator()
		// rst:
		// rst:			Construct a past-the-end iterator.
		EdgeIterator();
	private:
		friend class boost::iterator_core_access;
		HyperEdge dereference() const;
		bool equal(const EdgeIterator &iter) const;
		void increment();
	private:
		std::shared_ptr<DG> g;
		std::size_t eId;
	};
	// rst-nested-end:

	// rst-nested: DG::EdgeRange
	// rst:
	// rst:		A range of all edges in a derivation graph.
	// rst-nested-start:

	struct EdgeRange {
		using iterator = EdgeIterator;
		using const_iterator = iterator;
	private:
		friend class DG;
		EdgeRange(std::shared_ptr<DG> g);
	public:
		EdgeIterator begin() const;
		EdgeIterator end() const;
	private:
		std::shared_ptr<DG> g;
	};
	// rst-nested-end:
public: // Bidirectional
	// rst-nested: DG::InEdgeIterator
	// rst:
	// rst:		An iterator for enumerating all in-edges of a vertex.
	// rst:		It models a forward iterator.
	// rst-nested-start:

	class InEdgeIterator : public boost::iterator_facade<InEdgeIterator, HyperEdge, std::forward_iterator_tag, HyperEdge> {
		friend class InEdgeRange;
		InEdgeIterator(std::shared_ptr<DG> g, std::size_t vId);
	public:
		// rst:		.. function:: InEdgeIterator()
		// rst:
		// rst:			Construct a past-the-end iterator.
		InEdgeIterator();
	private:
		friend class boost::iterator_core_access;
		HyperEdge dereference() const;
		bool equal(const InEdgeIterator &iter) const;
		void increment();
	private:
		std::shared_ptr<DG> g;
		std::size_t vId, eId;
	};
	// rst-nested-end:

	// rst-nested: DG::InEdgeRange
	// rst:
	// rst:		A range of all in-edges of a vertex.
	// rst-nested-start:

	struct InEdgeRange {
		using iterator = InEdgeIterator;
		using const_iterator = iterator;
	private:
		friend class Vertex;
		InEdgeRange(std::shared_ptr<DG> g, std::size_t vId);
	public:
		InEdgeIterator begin() const;
		InEdgeIterator end() const;
	private:
		std::shared_ptr<DG> g;
		std::size_t vId;
	};
	// rst-nested-end:


	// rst-nested: DG::OutEdgeIterator
	// rst:
	// rst:		An iterator for enumerating all out-edges of a vertex.
	// rst:		It models a forward iterator.
	// rst-nested-start:

	class OutEdgeIterator : public boost::iterator_facade<OutEdgeIterator, HyperEdge, std::forward_iterator_tag, HyperEdge> {
		friend class OutEdgeRange;
		OutEdgeIterator(std::shared_ptr<DG> g, std::size_t vId);
	public:
		// rst:		.. function:: OutEdgeIterator()
		// rst:
		// rst:			Construct a past-the-end iterator.
		OutEdgeIterator();
	private:
		friend class boost::iterator_core_access;
		HyperEdge dereference() const;
		bool equal(const OutEdgeIterator &iter) const;
		void increment();
	private:
		std::shared_ptr<DG> g;
		std::size_t vId, eId;
	};
	// rst-nested-end:

	// rst-nested: DG::OutEdgeRange
	// rst:
	// rst:		A range of all out-edges of a vertex.
	// rst-nested-start:

	struct OutEdgeRange {
		using iterator = OutEdgeIterator;
		using const_iterator = iterator;
	private:
		friend class Vertex;
		OutEdgeRange(std::shared_ptr<DG> g, std::size_t vId);
	public:
		OutEdgeIterator begin() const;
		OutEdgeIterator end() const;
	private:
		std::shared_ptr<DG> g;
		std::size_t vId;
	};
	// rst-nested-end:
public: // HyperBidirectional (sources and targets of hyperedges)
	// rst-nested: DG::SourceIterator
	// rst:
	// rst:		An iterator for enumerating all sources of a hyperedge.
	// rst:		It models a forward iterator.
	// rst-nested-start:

	class SourceIterator : public boost::iterator_facade<SourceIterator, Vertex, std::forward_iterator_tag, Vertex> {
		friend class SourceRange;
		SourceIterator(std::shared_ptr<DG> g, std::size_t eId);
	public:
		// rst:		.. function:: SourceIterator()
		// rst:
		// rst:			Construct a past-the-end iterator.
		SourceIterator();
	private:
		friend class boost::iterator_core_access;
		Vertex dereference() const;
		bool equal(const SourceIterator &iter) const;
		void increment();
	private:
		std::shared_ptr<DG> g;
		std::size_t eId, vId;
	};
	// rst-nested-end:

	// rst-nested: DG::SourceRange
	// rst:
	// rst:		A range of all sources of a hyperedge.
	// rst-nested-start:

	struct SourceRange {
		using iterator = SourceIterator;
		using const_iterator = iterator;
	private:
		friend class HyperEdge;
		SourceRange(std::shared_ptr<DG> g, std::size_t eId);
	public:
		SourceIterator begin() const;
		SourceIterator end() const;
	private:
		std::shared_ptr<DG> g;
		std::size_t eId;
	};
	// rst-nested-end:


	// rst-nested: DG::TargetIterator
	// rst:
	// rst:		An iterator for enumerating all targets of a hyperedge.
	// rst:		It models a forward iterator.
	// rst-nested-start:

	class TargetIterator : public boost::iterator_facade<TargetIterator, Vertex, std::forward_iterator_tag, Vertex> {
		friend class TargetRange;
		TargetIterator(std::shared_ptr<DG> g, std::size_t eId);
	public:
		// rst:		.. function:: TargetIterator()
		// rst:
		// rst:			Construct a past-the-end iterator.
		TargetIterator();
	private:
		friend class boost::iterator_core_access;
		Vertex dereference() const;
		bool equal(const TargetIterator &iter) const;
		void increment();
	private:
		std::shared_ptr<DG> g;
		std::size_t eId, vId;
	};
	// rst-nested-end:

	// rst-nested: DG::TargetRange
	// rst:
	// rst:		A range of all sources of a hyperedge.
	// rst-nested-start:

	struct TargetRange {
		using iterator = TargetIterator;
		using const_iterator = iterator;
	private:
		friend class HyperEdge;
		TargetRange(std::shared_ptr<DG> g, std::size_t eId);
	public:
		TargetIterator begin() const;
		TargetIterator end() const;
	private:
		std::shared_ptr<DG> g;
		std::size_t eId;
	};
	// rst-nested-end:
public:
	// rst-nested: DG::RuleIterator
	// rst:
	// rst:		An iterator for enumerating all rules of a hyperedge.
	// rst:		It models a forward iterator.
	// rst-nested-start:

	class RuleIterator : public boost::iterator_facade<RuleIterator, std::shared_ptr<Rule>, std::forward_iterator_tag, std::shared_ptr<Rule> > {
		friend class RuleRange;
		RuleIterator(std::shared_ptr<DG> g, std::size_t eId);
	public:
		// rst:		.. function:: RuleIterator()
		// rst:
		// rst:			Construct a past-the-end iterator.
		RuleIterator();
	private:
		friend class boost::iterator_core_access;
		std::shared_ptr<Rule> dereference() const;
		bool equal(const RuleIterator &iter) const;
		void increment();
	private:
		std::shared_ptr<DG> g;
		std::size_t eId, i;
	};
	// rst-nested-end:

	// rst-nested: DG::RuleRange
	// rst:
	// rst:		A range of all rules of a hyperedge.
	// rst-nested-start:

	struct RuleRange {
		using iterator = RuleIterator;
		using const_iterator = iterator;
	private:
		friend class HyperEdge;
		RuleRange(std::shared_ptr<DG> g, std::size_t eId);
	public:
		RuleIterator begin() const;
		RuleIterator end() const;
	private:
		std::shared_ptr<DG> g;
		std::size_t eId;
	};
	// rst-nested-end:
public:
	// rst-nested: DG::DerivationRefRange
	// rst:
	// rst:		This class represents a range of derivation references.
	// rst-nested-start:

	struct DerivationRefRange {

		struct const_iterator {
			using difference_type = std::size_t;
			using value_type = DerivationRef;
			using reference = DerivationRef;
			using iterator_category = std::forward_iterator_tag;
			using pointer = void;
		private:
			friend class DerivationRefRange;
			const_iterator(const DerivationRefRange &range, unsigned int id);
		public:
			bool operator!=(const const_iterator &other) const;
			bool operator==(const const_iterator &other) const;
			const_iterator operator++();
			const_iterator operator++(int);
			DerivationRef operator*() const;
		private:
			const DerivationRefRange &range;
			unsigned int id;
		};
		using iterator = const_iterator;
	private:
		friend class DG;

		DerivationRefRange(unsigned int firstId, unsigned int endId,
				std::function<unsigned int(unsigned int) > next,
				std::function<DerivationRef(unsigned int) > dereference)
		: firstId(firstId), endId(endId), next(next), dereference(dereference) { }
	public:
		// rst:		.. function:: const_iterator begin() const
		// rst:
		// rst:			:returns: the begin iterator for the range of derivation references.
		const_iterator begin() const;
		// rst:		.. function:: const_iterator end() const
		// rst:
		// rst:			:returns: the end iterator for the range of derivation references.
		const_iterator end() const;
	private:
		unsigned int firstId, endId;
		std::function<unsigned int(unsigned int) > next;
		std::function<DerivationRef(unsigned int) > dereference;
	};
	// rst-nested-end:
private:
	DG(std::unique_ptr<lib::DG::NonHyper> dg);
public:
	~DG();
	// rst: .. function:: unsigned int getId() const
	// rst:
	// rst: 	:returns: the instance identifier for the object.
	unsigned int getId() const;
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
	// rst: .. function:: const std::vector<std::shared_ptr<Graph> > &getVertexGraphs() const
	// rst:
	// rst: 	:returns: the list of all graphs actually in the derivation graph.
	const std::vector<std::shared_ptr<Graph> > &getVertexGraphs() const;
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
	// rst: .. function:: void printMatrix() const
	// rst:
	// rst: 	(Deprecated) Exports some of the derivation graph data as sparse matrix.
	// rst: 	Use the dump function instead.
	// rst:
	// rst: 	:throws: :class:`LogicError` if the DG has not been calculated.
	void printMatrix() const;
	// rst: .. function:: DerivationRef getDerivationRef(const std::vector<std::shared_ptr<Graph> > &educts, const std::vector<std::shared_ptr<Graph> > &products) const
	// rst: 
	// rst: 	:returns: a reference to a derivation with the left side being ``educts`` and right side being ``products``.
	// rst: 		If no such derivation exists in the derivation graph then the reference is invalid.
	// rst: 	:throws: :class:`LogicError` if the DG has not been calculated.
	DerivationRef getDerivationRef(const std::vector<std::shared_ptr<Graph> > &educts, const std::vector<std::shared_ptr<Graph> > &products) const;
	// rst: .. function:: DerivationRefRange derivations() const
	// rst:
	// rst: 	:returns: an iterator range of references to all derivations in this DG.
	// rst: 	:throws: :class:`LogicError` if the DG has not been calculated.
	DerivationRefRange derivations() const;
	// rst: .. function:: DerivationRefRange inDerivations(std::shared_ptr<Graph> g) const
	// rst:
	// rst: 	:returns: an iterator range of references to all derivations going in to the vertex corresponding to ``g``.
	// rst: 		A derivation reference is in the range as many times as the vertex is a member of the head-multiset of that derivation.
	// rst: 	:throws: :class:`LogicError` if the DG has not been calculated.
	DerivationRefRange inDerivations(std::shared_ptr<Graph> g) const;
	// rst: .. function:: DerivationRefRange outDerivations(std::shared_ptr<Graph> g) const
	// rst:
	// rst: 	:returns: an iterator range of references to all derivations going out of the vertex corresponding to ``g``.
	// rst: 		A derivation reference is in the range as many times as the vertex is a member of the tail-multiset of that derivation.
	// rst: 	:throws: :class:`LogicError` if the DG has not been calculated.
	DerivationRefRange outDerivations(std::shared_ptr<Graph> g) const;
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
	// rst: .. function:: void listIncidence(const std::shared_ptr<Graph> g) const
	// rst:
	// rst: 	Output the adjacent derivations of the vertex corresponding to the given graph.
	// rst:
	// rst: 	:throws: :class:`LogicError` if the DG has not been calculated.
	void listIncidence(const std::shared_ptr<Graph> g) const;
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
	// rst: .. function:: static std::shared_ptr<DG> sbml(const std::string &file)
	// rst:
	// rst: 	Load a derivation graph from an SBML file.
	// rst:
	// rst: 	:throws: :class:`InputError` on bad input.
	static std::shared_ptr<DG> sbml(const std::string &file);
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



//------------------------------------------------------------------------------
// Implementation Details
//------------------------------------------------------------------------------

// VertexRange
//------------------------------------------------------------------------------

inline DG::VertexRange::VertexRange(std::shared_ptr<DG> g) : g(g) { }

inline DG::VertexIterator DG::VertexRange::begin() const {
	return VertexIterator(g);
}

inline DG::VertexIterator DG::VertexRange::end() const {
	return VertexIterator();
}

// EdgeRange
//------------------------------------------------------------------------------

inline DG::EdgeRange::EdgeRange(std::shared_ptr<DG> g) : g(g) { }

inline DG::EdgeIterator DG::EdgeRange::begin() const {
	return EdgeIterator(g);
}

inline DG::EdgeIterator DG::EdgeRange::end() const {
	return EdgeIterator();
}

// InEdgeRange
//------------------------------------------------------------------------------

inline DG::InEdgeRange::InEdgeRange(std::shared_ptr<DG> g, std::size_t vId) : g(g), vId(vId) { }

inline DG::InEdgeIterator DG::InEdgeRange::begin() const {
	return InEdgeIterator(g, vId);
}

inline DG::InEdgeIterator DG::InEdgeRange::end() const {
	return InEdgeIterator();
}

// OutEdgeRange
//------------------------------------------------------------------------------

inline DG::OutEdgeRange::OutEdgeRange(std::shared_ptr<DG> g, std::size_t vId) : g(g), vId(vId) { }

inline DG::OutEdgeIterator DG::OutEdgeRange::begin() const {
	return OutEdgeIterator(g, vId);
}

inline DG::OutEdgeIterator DG::OutEdgeRange::end() const {
	return OutEdgeIterator();
}

// SourceRange
//------------------------------------------------------------------------------

inline DG::SourceRange::SourceRange(std::shared_ptr<DG> g, std::size_t eId) : g(g), eId(eId) { }

inline DG::SourceIterator DG::SourceRange::begin() const {
	return SourceIterator(g, eId);
}

inline DG::SourceIterator DG::SourceRange::end() const {
	return SourceIterator();
}

// TargetRange
//------------------------------------------------------------------------------

inline DG::TargetRange::TargetRange(std::shared_ptr<DG> g, std::size_t eId) : g(g), eId(eId) { }

inline DG::TargetIterator DG::TargetRange::begin() const {
	return TargetIterator(g, eId);
}

inline DG::TargetIterator DG::TargetRange::end() const {
	return TargetIterator();
}

// RuleRange
//------------------------------------------------------------------------------

inline DG::RuleRange::RuleRange(std::shared_ptr<DG> g, std::size_t eId) : g(g), eId(eId) { }

inline DG::RuleIterator DG::RuleRange::begin() const {
	return RuleIterator(g, eId);
}

inline DG::RuleIterator DG::RuleRange::end() const {
	return RuleIterator();
}

} // namespace mod

#endif /* MOD_DG_H */