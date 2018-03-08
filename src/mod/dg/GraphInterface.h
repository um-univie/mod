#ifndef MOD_DG_GRAPHINTERFACE_H
#define MOD_DG_GRAPHINTERFACE_H

// rst: This header contains the definitions for the hypergraph interface for :cpp:class:`dg::DG`.
// rst:

#include <mod/dg/DG.h>
#include <mod/graph/ForwardDecl.h>

#include <boost/iterator/iterator_facade.hpp>

namespace mod {
namespace dg {

// Graph
// -----------------------------------------------------------------------------

// rst-class: dg::DG::Vertex
// rst:
// rst:		A descriptor of either a vertex in a graph, or a null vertex.
// rst-class-start:

class DG::Vertex {
	friend class lib::DG::Hyper;
	Vertex(std::shared_ptr<DG> g, std::size_t vId);
public:
	// rst:	.. function:: Vertex()
	// rst:
	// rst:		Constructs a null descriptor.
	Vertex();
	friend std::ostream &operator<<(std::ostream &s, const Vertex &v);
	friend bool operator==(const Vertex &v1, const Vertex &v2);
	friend bool operator!=(const Vertex &v1, const Vertex &v2);
	friend bool operator<(const Vertex &v1, const Vertex &v2);
	// rst:	.. function:: bool isNull() const
	// rst:
	// rst:		:returns: whether this is a null descriptor or not.
	bool isNull() const;
	// rst:	.. function:: std::size_t getId() const
	// rst:
	// rst:		:returns: the index of the vertex. It will be in the range :math:`[0, numVertices + numEdges[`.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::size_t getId() const;
	// rst:	.. function:: std::shared_ptr<DG> getDG() const
	// rst:
	// rst:		:returns: the derivation graph the vertex belongs to.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::shared_ptr<DG> getDG() const;
	// rst:	.. function:: std::size_t inDegree() const
	// rst:
	// rst:		:returns: the in-degree of the vertex, including multiplicity of target multisets.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::size_t inDegree() const;
	// rst:	.. function:: InEdgeRange inEdges() const
	// rst:
	// rst:		:returns: a range of in-hyperedges for this vertex.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	InEdgeRange inEdges() const;
	// rst:	.. function:: std::size_t getOutDegree() const
	// rst:
	// rst:		:returns: the out-degree of the vertex, including multiplicity of source multisets.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::size_t outDegree() const;
	// rst:	.. function:: OutEdgeRange outEdges() const
	// rst:
	// rst:		:returns: a range of out-hyperedges for this vertex.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	OutEdgeRange outEdges() const;
	// rst:	.. function:: std::shared_ptr<graph::Graph> &getGraph() const
	// rst:
	// rst: 	:returns: the graph label of the vertex.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::shared_ptr<graph::Graph> getGraph() const;
private:
	std::shared_ptr<DG> g;
	std::size_t vId;
};
// rst-class-end:

// rst-class: dg::DG::HyperEdge
// rst:
// rst:		A descriptor of either a hyperedge in a derivation graph, or a null edge.
// rst-class-start:

class DG::HyperEdge {
	friend class lib::DG::Hyper;
	HyperEdge(std::shared_ptr<DG> g, std::size_t eId);
public:
	// rst:	.. function:: HyperEdge()
	// rst:
	// rst:		Constructs a null descriptor.
	HyperEdge();
	friend std::ostream &operator<<(std::ostream &s, const HyperEdge &e);
	friend bool operator==(const HyperEdge &e1, const HyperEdge &e2);
	friend bool operator!=(const HyperEdge &e1, const HyperEdge &e2);
	friend bool operator<(const HyperEdge &e1, const HyperEdge &e2);
	// rst:	.. function:: bool isNull() const
	// rst:
	// rst:		:returns: whether this is a null descriptor or not.
	bool isNull() const;
	// rst:	.. function:: std::size_t getId() const
	// rst:
	// rst:		:returns: the index of the hyperedge. It will be in the range :math:`[0, numVertices + numEdges[`.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::size_t getId() const;
	// rst:	.. function:: std::shared_ptr<DG> getDG() const
	// rst:
	// rst:		:returns: the derivation graph the hyperedge belongs to.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::shared_ptr<DG> getDG() const;
	// rst:	.. function:: std::size_t numSources() const
	// rst:
	// rst:		:returns: the number of sources of the hyperedge.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::size_t numSources() const;
	// rst:	.. function:: SourceRange sources() const
	// rst:
	// rst:		:returns: the sources of the hyperedge.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	SourceRange sources() const;
	// rst:	.. function:: std::size_t numTargets() const
	// rst:
	// rst:		:returns: the number of targets of the hyperedge.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::size_t numTargets() const;
	// rst:	.. function:: TargetRange targets() const
	// rst:
	// rst:		:returns: the targets of the hyperedge.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	TargetRange targets() const;
	// rst:	.. function:: RuleRange rules() const
	// rst:
	// rst: 	:returns: a range of the rules associated with the hyperedge.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	RuleRange rules() const;
	// rst: .. function:: HyperEdge getInverse() const
	// rst:
	// rst:		:returns: a descriptor for the inverse hyperedge of this one, if it exists.
	// rst:			Otherwise a null descriptor is returned.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	HyperEdge getInverse() const;
public:
	// rst: .. function:: void print(const graph::Printer &printer, const std::string &nomatchColour, const std::string &matchColour) const
	// rst:
	// rst:		Print the derivations represented by the hyperedge.
	// rst:		All possible Double-Pushout diagrams are printed.
	// rst:		The ``matchColour`` must be a valid colour for TikZ, which is applied to the rule
	// rst:		and its image in the bottom span.
	// rst:
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	void print(const graph::Printer &printer, const std::string &nomatchColour, const std::string &matchColour) const;
	// rst: .. function:: void printTransitionState() const
	// rst:               void printTransitionState(const graph::Printer &printer) const
	// rst:
	// rst:		Print the derivations represented by the hyperedge in style of a chemical transition state.
	// rst:		For all possible Double-Pushout diagrams a figure printed.
	// rst:
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	void printTransitionState() const;
	void printTransitionState(const graph::Printer &printer) const;
private:
	std::shared_ptr<DG> g;
	std::size_t eId;
};
// rst-class-end:


// VertexList
// -----------------------------------------------------------------------------

// rst-class: dg::DG::VertexIterator
// rst:
// rst:		An iterator for traversing all vertices in a derivation graph.
// rst:		It models a forward iterator.
// rst-class-start:

class DG::VertexIterator : public boost::iterator_facade<VertexIterator, Vertex, std::forward_iterator_tag, Vertex> {
	friend class DG;
	VertexIterator(std::shared_ptr<DG> g);
public:
	// rst:	.. function:: VertexIterator()
	// rst:
	// rst:		Construct a past-the-end iterator.
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
// rst-class-end:

// rst-class: dg::DG::VertexRange
// rst:
// rst:		A range of all vertices in a derivation graph.
// rst-class-start:

struct DG::VertexRange {
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
// rst-class-end:


// EdgeList
// -----------------------------------------------------------------------------

// rst-class: dg::DG::EdgeIterator
// rst:
// rst:		An iterator for traversing all edges in a graph.
// rst:		It models a forward iterator.
// rst-class-start:

class DG::EdgeIterator : public boost::iterator_facade<EdgeIterator, HyperEdge, std::forward_iterator_tag, HyperEdge> {
	friend class DG;
	EdgeIterator(std::shared_ptr<DG> g);
public:
	// rst:	.. function:: EdgeIterator()
	// rst:
	// rst:		Construct a past-the-end iterator.
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
// rst-class-end:

// rst-class: dg::DG::EdgeRange
// rst:
// rst:		A range of all edges in a derivation graph.
// rst-class-start:

struct DG::EdgeRange {
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
// rst-class-end:


// Bidirectional
// -----------------------------------------------------------------------------

// rst-class: dg::DG::InEdgeIterator
// rst:
// rst:		An iterator for enumerating all in-edges of a vertex.
// rst:		It models a forward iterator.
// rst-class-start:

class DG::InEdgeIterator : public boost::iterator_facade<InEdgeIterator, HyperEdge, std::forward_iterator_tag, HyperEdge> {
	friend class InEdgeRange;
	InEdgeIterator(std::shared_ptr<DG> g, std::size_t vId);
public:
	// rst:	.. function:: InEdgeIterator()
	// rst:
	// rst:		Construct a past-the-end iterator.
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
// rst-class-end:

// rst-class: dg::DG::InEdgeRange
// rst:
// rst:		A range of all in-edges of a vertex.
// rst-class-start:

struct DG::InEdgeRange {
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
// rst-class-end:


// rst-class: dg::DG::OutEdgeIterator
// rst:
// rst:		An iterator for enumerating all out-edges of a vertex.
// rst:		It models a forward iterator.
// rst-class-start:

class DG::OutEdgeIterator : public boost::iterator_facade<OutEdgeIterator, HyperEdge, std::forward_iterator_tag, HyperEdge> {
	friend class OutEdgeRange;
	OutEdgeIterator(std::shared_ptr<DG> g, std::size_t vId);
public:
	// rst:	.. function:: OutEdgeIterator()
	// rst:
	// rst:		Construct a past-the-end iterator.
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
// rst-class-end:

// rst-class: dg::DG::OutEdgeRange
// rst:
// rst:		A range of all out-edges of a vertex.
// rst-class-start:

struct DG::OutEdgeRange {
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
// rst-class-end:


// HyperBidirectional (sources and targets of hyperedges)
// -----------------------------------------------------------------------------

// rst-class: dg::DG::SourceIterator
// rst:
// rst:		An iterator for enumerating all sources of a hyperedge.
// rst:		It models a forward iterator.
// rst-class-start:

class DG::SourceIterator : public boost::iterator_facade<SourceIterator, Vertex, std::forward_iterator_tag, Vertex> {
	friend class SourceRange;
	SourceIterator(std::shared_ptr<DG> g, std::size_t eId);
public:
	// rst:	.. function:: SourceIterator()
	// rst:
	// rst:		Construct a past-the-end iterator.
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
// rst-class-end:

// rst-class: dg::DG::SourceRange
// rst:
// rst:		A range of all sources of a hyperedge.
// rst-class-start:

struct DG::SourceRange {
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
// rst-class-end:


// rst-class: dg::DG::TargetIterator
// rst:
// rst:		An iterator for enumerating all targets of a hyperedge.
// rst:		It models a forward iterator.
// rst-class-start:

class DG::TargetIterator : public boost::iterator_facade<TargetIterator, Vertex, std::forward_iterator_tag, Vertex> {
	friend class TargetRange;
	TargetIterator(std::shared_ptr<DG> g, std::size_t eId);
public:
	// rst:	.. function:: TargetIterator()
	// rst:
	// rst:		Construct a past-the-end iterator.
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
// rst-class-end:

// rst-class: dg::DG::TargetRange
// rst:
// rst:		A range of all sources of a hyperedge.
// rst-class-start:

struct DG::TargetRange {
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
// rst-class-end:


// Other Stuff
// -----------------------------------------------------------------------------

// rst-class: dg::DG::RuleIterator
// rst:
// rst:		An iterator for enumerating all rules of a hyperedge.
// rst:		It models a forward iterator.
// rst-class-start:

class DG::RuleIterator : public boost::iterator_facade<RuleIterator, std::shared_ptr<rule::Rule>, std::forward_iterator_tag, std::shared_ptr<rule::Rule> > {
	friend class RuleRange;
	RuleIterator(std::shared_ptr<DG> g, std::size_t eId);
public:
	// rst:	.. function:: RuleIterator()
	// rst:
	// rst:		Construct a past-the-end iterator.
	RuleIterator();
private:
	friend class boost::iterator_core_access;
	std::shared_ptr<rule::Rule> dereference() const;
	bool equal(const RuleIterator &iter) const;
	void increment();
private:
	std::shared_ptr<DG> g;
	std::size_t eId, i;
};
// rst-class-end:

// rst-class: dg::DG::RuleRange
// rst:
// rst:		A range of all rules of a hyperedge.
// rst-class-start:

struct DG::RuleRange {
	using iterator = RuleIterator;
	using const_iterator = iterator;
private:
	friend class HyperEdge;
	RuleRange(std::shared_ptr<DG> g, std::size_t eId);
public:
	RuleIterator begin() const;
	RuleIterator end() const;
	std::size_t size() const;
private:
	std::shared_ptr<DG> g;
	std::size_t eId;
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

} // namespace dg
} // namespace mod

#endif /* MOD_DG_GRAPHINTERFACE_H */