#ifndef MOD_RULEGRAPHINTERFACE_H
#define MOD_RULEGRAPHINTERFACE_H

// rst: This header contains the defintinos for the graph interface for :cpp:class:`mod::Rule`.
// rst:

#include <mod/Rule.h>

#include <boost/iterator/iterator_facade.hpp>

namespace mod {
struct AtomId;
struct Charge;
enum class BondType;

// rst: ========================================================================
// rst: Left
// rst: ========================================================================
// rst:
// rst-class: Rule::LeftGraph
// rst:
// rst:		A proxy object representing the left graph of the rule.
// rst-class-begin:

struct Rule::LeftGraph {
	class Vertex;
	class Edge;
	class VertexIterator;
	class VertexRange;
	class EdgeIterator;
	class EdgeRange;
	class IncidentEdgeIterator;
	class IncidentEdgeRange;
private:
	friend class Rule;
	LeftGraph(std::shared_ptr<Rule> r);
public:
	// rst: .. function:: std::shared_ptr<Rule> getRule() const
	// rst:
	// rst:		:returns: the rule where the graph belongs to.
	std::shared_ptr<Rule> getRule() const;
	// rst: .. function:: std::size_t numVertices() const
	// rst:
	// rst:		:returns: the number of vertices in the graph.
	std::size_t numVertices() const;
	// rst: .. function:: VertexRange vertices() const
	// rst:
	// rst:		:returns: a range of all vertices in the graph.
	VertexRange vertices() const;
	// rst: .. function:: std::size_t numEdges() const
	// rst:
	// rst:		:returns: the number of edges in the graph.
	std::size_t numEdges() const;
	// rst: .. function:: EdgeRange edges() const
	// rst:
	// rst:		:returns: a range of all edges in the graph.
	EdgeRange edges() const;
private:
	std::shared_ptr<Rule> r;
};
// rst-class-end:



// Graph
// -----------------------------------------------------------------------------

// rst-class: Rule::LeftGraph::Vertex
// rst:
// rst:		A descriptor of either a vertex in a rule, or a null vertex.
// rst-class-start:

class Rule::LeftGraph::Vertex {
	friend class Rule::Vertex;
	friend class LeftGraph;
	friend class Edge;
	friend class VertexIterator;
	friend class VertexRange;
	Vertex(std::shared_ptr<Rule> r, std::size_t vId);
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
	// rst:		:returns: the index of the vertex. It will be in the range :math:`[0, numVertices[`.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::size_t getId() const;
	// rst:	.. function:: std::shared_ptr<Rule> getRule() const
	// rst:
	// rst:		:returns: the rule the vertex belongs to.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::shared_ptr<Rule> getRule() const;
	// rst: .. function:: Rule::Vertex getCore() const
	// rst:
	// rst:		:returns: the descriptor for this vertex in the core graph.
	Rule::Vertex getCore() const;
	// rst:	.. function:: std::size_t getDegree() const
	// rst:
	// rst:		:returns: the degree of the vertex.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::size_t getDegree() const;
	// rst:	.. function:: IncidentEdgeRange incidentEdges() const
	// rst:
	// rst:		:returns: a range of incident edges to this vertex.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	IncidentEdgeRange incidentEdges() const;
	// rst:	.. function:: const std::string &getStringLabel() const
	// rst:
	// rst: 	:returns: the string label of the vertex.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	const std::string &getStringLabel() const;
	// rst:	.. function:: AtomId getAtomId() const
	// rst:
	// rst:		:returns: the atom id of the vertex.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	AtomId getAtomId() const;
	// rst:	.. function:: Charge getCharge() const
	// rst:
	// rst:		:returns: the charge of the vertex.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	Charge getCharge() const;
	// rst:	.. function:: bool getRadical() const
	// rst:
	// rst:		:returns: the radical status of the vertex.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	bool getRadical() const;
private:
	std::shared_ptr<Rule> r;
	std::size_t vId;
};
// rst-class-end:

// rst-class: Rule::LeftGraph::Edge
// rst:
// rst:		A descriptor of either an edge in a rule, or a null edge.
// rst-class-start:

class Rule::LeftGraph::Edge {
	friend class Rule::Edge;
	friend class EdgeIterator;
	friend class IncidentEdgeIterator;
	Edge(std::shared_ptr<Rule> r, std::size_t vId, std::size_t eId);
public:
	// rst:	.. function:: Edge()
	// rst:
	// rst:		Constructs a null descriptor.
	Edge();
	friend std::ostream &operator<<(std::ostream &s, const Edge &e);
	friend bool operator==(const Edge &e1, const Edge &e2);
	friend bool operator!=(const Edge &e1, const Edge &e2);
	friend bool operator<(const Edge &e1, const Edge &e2);
	// rst:	.. function:: bool isNull() const
	// rst:
	// rst:		:returns: whether this is a null descriptor or not.
	bool isNull() const;
	// rst:	.. function:: std::shared_ptr<Rule> getRule() const
	// rst:
	// rst:		:returns: the rule the edge belongs to.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::shared_ptr<Rule> getRule() const;
	// rst: .. function:: Rule::edge getCore() const
	// rst:
	// rst:		:returns: the descriptor for this edge in the core graph.
	Rule::Edge getCore() const;
	// rst:	.. function:: Vertex source() const
	// rst:
	// rst:		:returns: the source vertex of the edge.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	Vertex source() const;
	// rst:	.. function:: Vertex target() const
	// rst:
	// rst:		:returns: the target vertex of the edge.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	Vertex target() const;
	// rst:	.. function:: const std::string &getStringLabel() const
	// rst:
	// rst: 	:returns: the string label of the edge.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	const std::string &getStringLabel() const;
	// rst:	.. function:: BondType getBondType() const
	// rst:
	// rst:		:returns: the bond type of the edge.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	BondType getBondType() const;
private:
	std::shared_ptr<Rule> r;
	std::size_t vId, eId;
};
// rst-class-end:


// VertexList
// -----------------------------------------------------------------------------

// rst-class: Rule::LeftGraph::VertexIterator
// rst:
// rst:		An iterator for traversing all vertices in a rule.
// rst:		It models a forward iterator.
// rst-class-start:

class Rule::LeftGraph::VertexIterator : public boost::iterator_facade<VertexIterator, Vertex, std::forward_iterator_tag, Vertex> {
	friend class Rule;
	VertexIterator(std::shared_ptr<Rule> r);
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
	std::shared_ptr<Rule> r;
	std::size_t vId;
};
// rst-class-end:

// rst-class: Rule::LeftGraph::VertexRange
// rst:
// rst:		A range of all vertices in a rule.
// rst-class-begin:

struct Rule::LeftGraph::VertexRange {
	using iterator = VertexIterator;
	using const_iterator = iterator;
private:
	friend class Rule;
	VertexRange(std::shared_ptr<Rule> r);
public:
	VertexIterator begin() const;
	VertexIterator end() const;
private:
	std::shared_ptr<Rule> r;
};
// rst-class-end:


// EdgeList
// -----------------------------------------------------------------------------

// rst-class: Rule::LeftGraph::EdgeIterator
// rst:
// rst:		An iterator for traversing all edges in a rule.
// rst:		It models a forward iterator.
// rst-class-start:

class Rule::LeftGraph::EdgeIterator : public boost::iterator_facade<EdgeIterator, Edge, std::forward_iterator_tag, Edge> {
	friend class Rule;
	EdgeIterator(std::shared_ptr<Rule> r);
public:
	// rst:	.. function:: EdgeIterator()
	// rst:
	// rst:		Construct a past-the-end iterator.
	EdgeIterator();
private:
	friend class boost::iterator_core_access;
	Edge dereference() const;
	bool equal(const EdgeIterator &iter) const;
	void increment();
	void advanceToValid();
private:
	std::shared_ptr<Rule> r;
	std::size_t vId, eId;
};
// rst-class-end:

// rst-class: Rule::LeftGraph::EdgeRange
// rst:
// rst:		A range of all edges in a rule.
// rst-class-begin:

struct Rule::LeftGraph::EdgeRange {
	using iterator = EdgeIterator;
	using const_iterator = iterator;
private:
	friend class Rule;
	EdgeRange(std::shared_ptr<Rule> r);
public:
	EdgeIterator begin() const;
	EdgeIterator end() const;
private:
	std::shared_ptr<Rule> r;
};
// rst-class-end:


// Incidence
// -----------------------------------------------------------------------------

// rst-class: Rule::LeftGraph::IncidnetEdgeIterator
// rst:
// rst:		An iterator for traversing all edges in a rule.
// rst:		It models a forward iterator.
// rst-class-start:

class Rule::LeftGraph::IncidentEdgeIterator : public boost::iterator_facade<IncidentEdgeIterator, Edge, std::forward_iterator_tag, Edge> {
	friend class Rule;
	IncidentEdgeIterator(std::shared_ptr<Rule> r, std::size_t vId);
public:
	// rst:	.. function:: IncidentEdgeIterator()
	// rst:
	// rst:		Construct a past-the-end iterator.
	IncidentEdgeIterator();
private:
	friend class boost::iterator_core_access;
	Edge dereference() const;
	bool equal(const IncidentEdgeIterator &iter) const;
	void increment();
private:
	std::shared_ptr<Rule> r;
	std::size_t vId, eId;
};
// rst-class-end:

// rst-class: Rule::LeftGraph::IncidentEdgeRange
// rst:
// rst:		A range of all incident edges to a vertex in a rule.
// rst-class-begin:

struct Rule::LeftGraph::IncidentEdgeRange {
	using iterator = IncidentEdgeIterator;
	using const_iterator = iterator;
private:
	friend class Vertex;
	IncidentEdgeRange(std::shared_ptr<Rule> r, std::size_t vId);
public:
	IncidentEdgeIterator begin() const;
	IncidentEdgeIterator end() const;
private:
	std::shared_ptr<Rule> r;
	std::size_t vId;
};
// rst-class-end:


// rst:
// rst: ========================================================================
// rst: Context
// rst: ========================================================================
// rst:
// rst-class: Rule::ContextGraph
// rst:
// rst:		A proxy object representing the context graph of the rule.
// rst-class-begin:

struct Rule::ContextGraph {
	class Vertex;
	class Edge;
	class VertexIterator;
	class VertexRange;
	class EdgeIterator;
	class EdgeRange;
	class IncidentEdgeIterator;
	class IncidentEdgeRange;
private:
	friend class Rule;
	ContextGraph(std::shared_ptr<Rule> r);
public:
	// rst: .. function:: std::shared_ptr<Rule> getRule() const
	// rst:
	// rst:		:returns: the rule where the graph belongs to.
	std::shared_ptr<Rule> getRule() const;
	// rst: .. function:: std::size_t numVertices() const
	// rst:
	// rst:		:returns: the number of vertices in the graph.
	std::size_t numVertices() const;
	// rst: .. function:: VertexRange vertices() const
	// rst:
	// rst:		:returns: a range of all vertices in the graph.
	VertexRange vertices() const;
	// rst: .. function:: std::size_t numEdges() const
	// rst:
	// rst:		:returns: the number of edges in the graph.
	std::size_t numEdges() const;
	// rst: .. function:: EdgeRange edges() const
	// rst:
	// rst:		:returns: a range of all edges in the graph.
	EdgeRange edges() const;
private:
	std::shared_ptr<Rule> r;
};
// rst-class-end:


// Graph
// -----------------------------------------------------------------------------

// rst-class: Rule::ContextGraph::Vertex
// rst:
// rst:		A descriptor of either a vertex in a rule, or a null vertex.
// rst-class-start:

class Rule::ContextGraph::Vertex {
	friend class Rule;
	friend class Edge;
	friend class VertexIterator;
	friend class VertexRange;
	Vertex(std::shared_ptr<Rule> r, std::size_t vId);
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
	// rst:		:returns: the index of the vertex. It will be in the range :math:`[0, numVertices[`.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::size_t getId() const;
	// rst:	.. function:: std::shared_ptr<Rule> getRule() const
	// rst:
	// rst:		:returns: the rule the vertex belongs to.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::shared_ptr<Rule> getRule() const;
	// rst: .. function:: Rule::Vertex getCore() const
	// rst:
	// rst:		:returns: the descriptor for this vertex in the core graph.
	Rule::Vertex getCore() const;
	// rst:	.. function:: std::size_t getDegree() const
	// rst:
	// rst:		:returns: the degree of the vertex.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::size_t getDegree() const;
	// rst:	.. function:: IncidentEdgeRange incidentEdges() const
	// rst:
	// rst:		:returns: a range of incident edges to this vertex.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	IncidentEdgeRange incidentEdges() const;
private:
	std::shared_ptr<Rule> r;
	std::size_t vId;
};
// rst-class-end:

// rst-class: Rule::ContextGraph::Edge
// rst:
// rst:		A descriptor of either an edge in a rule, or a null edge.
// rst-class-start:

class Rule::ContextGraph::Edge {
	friend class Rule::Edge;
	friend class EdgeIterator;
	friend class IncidentEdgeIterator;
	Edge(std::shared_ptr<Rule> r, std::size_t vId, std::size_t eId);
public:
	// rst:	.. function:: Edge()
	// rst:
	// rst:		Constructs a null descriptor.
	Edge();
	friend std::ostream &operator<<(std::ostream &s, const Edge &e);
	friend bool operator==(const Edge &e1, const Edge &e2);
	friend bool operator!=(const Edge &e1, const Edge &e2);
	friend bool operator<(const Edge &e1, const Edge &e2);
	// rst:	.. function:: bool isNull() const
	// rst:
	// rst:		:returns: whether this is a null descriptor or not.
	bool isNull() const;
	// rst:	.. function:: std::shared_ptr<Rule> getRule() const
	// rst:
	// rst:		:returns: the rule the edge belongs to.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::shared_ptr<Rule> getRule() const;
	// rst: .. function:: Rule::Edge getCore() const
	// rst:
	// rst:		:returns: the descriptor for this edge in the core graph.
	Rule::Edge getCore() const;
	// rst:	.. function:: Vertex source() const
	// rst:
	// rst:		:returns: the source vertex of the edge.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	Vertex source() const;
	// rst:	.. function:: Vertex target() const
	// rst:
	// rst:		:returns: the target vertex of the edge.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	Vertex target() const;
private:
	std::shared_ptr<Rule> r;
	std::size_t vId, eId;
};
// rst-class-end:


// VertexList
// -----------------------------------------------------------------------------

// rst-class: Rule::ContextGraph::VertexIterator
// rst:
// rst:		An iterator for traversing all vertices in a rule.
// rst:		It models a forward iterator.
// rst-class-start:

class Rule::ContextGraph::VertexIterator : public boost::iterator_facade<VertexIterator, Vertex, std::forward_iterator_tag, Vertex> {
	friend class Rule;
	VertexIterator(std::shared_ptr<Rule> r);
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
	std::shared_ptr<Rule> r;
	std::size_t vId;
};
// rst-class-end:

// rst-class: Rule::ContextGraph::VertexRange
// rst:
// rst:		A range of all vertices in a rule.
// rst-class-begin:

struct Rule::ContextGraph::VertexRange {
	using iterator = VertexIterator;
	using const_iterator = iterator;
private:
	friend class Rule;
	VertexRange(std::shared_ptr<Rule> r);
public:
	VertexIterator begin() const;
	VertexIterator end() const;
private:
	std::shared_ptr<Rule> r;
};
// rst-class-end:


// EdgeList
// -----------------------------------------------------------------------------

// rst-class: Rule::ContextGraph::EdgeIterator
// rst:
// rst:		An iterator for traversing all edges in a rule.
// rst:		It models a forward iterator.
// rst-class-start:

class Rule::ContextGraph::EdgeIterator : public boost::iterator_facade<EdgeIterator, Edge, std::forward_iterator_tag, Edge> {
	friend class Rule;
	EdgeIterator(std::shared_ptr<Rule> r);
public:
	// rst:	.. function:: EdgeIterator()
	// rst:
	// rst:		Construct a past-the-end iterator.
	EdgeIterator();
private:
	friend class boost::iterator_core_access;
	Edge dereference() const;
	bool equal(const EdgeIterator &iter) const;
	void increment();
	void advanceToValid();
private:
	std::shared_ptr<Rule> r;
	std::size_t vId, eId;
};
// rst-class-end:

// rst-class: Rule::ContextGraph::EdgeRange
// rst:
// rst:		A range of all edges in a rule.
// rst-class-begin:

struct Rule::ContextGraph::EdgeRange {
	using iterator = EdgeIterator;
	using const_iterator = iterator;
private:
	friend class Rule;
	EdgeRange(std::shared_ptr<Rule> r);
public:
	EdgeIterator begin() const;
	EdgeIterator end() const;
private:
	std::shared_ptr<Rule> r;
};
// rst-class-end:


// Incidence
// -----------------------------------------------------------------------------

// rst-class: Rule::ContextGraph::IncidnetEdgeIterator
// rst:
// rst:		An iterator for traversing all edges in a rule.
// rst:		It models a forward iterator.
// rst-class-start:

class Rule::ContextGraph::IncidentEdgeIterator : public boost::iterator_facade<IncidentEdgeIterator, Edge, std::forward_iterator_tag, Edge> {
	friend class Rule;
	IncidentEdgeIterator(std::shared_ptr<Rule> r, std::size_t vId);
public:
	// rst:	.. function:: IncidentEdgeIterator()
	// rst:
	// rst:		Construct a past-the-end iterator.
	IncidentEdgeIterator();
private:
	friend class boost::iterator_core_access;
	Edge dereference() const;
	bool equal(const IncidentEdgeIterator &iter) const;
	void increment();
private:
	std::shared_ptr<Rule> r;
	std::size_t vId, eId;
};
// rst-class-end:

// rst-class: Rule::ContextGraph::IncidentEdgeRange
// rst:
// rst:		A range of all incident edges to a vertex in a rule.
// rst-class-begin:

struct Rule::ContextGraph::IncidentEdgeRange {
	using iterator = IncidentEdgeIterator;
	using const_iterator = iterator;
private:
	friend class Vertex;
	IncidentEdgeRange(std::shared_ptr<Rule> r, std::size_t vId);
public:
	IncidentEdgeIterator begin() const;
	IncidentEdgeIterator end() const;
private:
	std::shared_ptr<Rule> r;
	std::size_t vId;
};
// rst-class-end:


// rst:
// rst: ========================================================================
// rst: Right
// rst: ========================================================================
// rst:
// rst-class: Rule::RightGraph
// rst:
// rst:		A proxy object representing the right graph of the rule.
// rst-class-begin:

struct Rule::RightGraph {
	class Vertex;
	class Edge;
	class VertexIterator;
	class VertexRange;
	class EdgeIterator;
	class EdgeRange;
	class IncidentEdgeIterator;
	class IncidentEdgeRange;
private:
	friend class Rule;
	RightGraph(std::shared_ptr<Rule> r);
public:
	// rst: .. function:: std::shared_ptr<Rule> getRule() const
	// rst:
	// rst:		:returns: the rule where the graph belongs to.
	std::shared_ptr<Rule> getRule() const;
	// rst: .. function:: std::size_t numVertices() const
	// rst:
	// rst:		:returns: the number of vertices in the graph.
	std::size_t numVertices() const;
	// rst: .. function:: VertexRange vertices() const
	// rst:
	// rst:		:returns: a range of all vertices in the graph.
	VertexRange vertices() const;
	// rst: .. function:: std::size_t numEdges() const
	// rst:
	// rst:		:returns: the number of edges in the graph.
	std::size_t numEdges() const;
	// rst: .. function:: EdgeRange edges() const
	// rst:
	// rst:		:returns: a range of all edges in the graph.
	EdgeRange edges() const;
private:
	std::shared_ptr<Rule> r;
};
// rst-class-end:


// Graph
// -----------------------------------------------------------------------------

// rst-class: Rule::RightGraph::Vertex
// rst:
// rst:		A descriptor of either a vertex in a rule, or a null vertex.
// rst-class-start:

class Rule::RightGraph::Vertex {
	friend class Rule;
	friend class Edge;
	friend class VertexIterator;
	friend class VertexRange;
	Vertex(std::shared_ptr<Rule> r, std::size_t vId);
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
	// rst:		:returns: the index of the vertex. It will be in the range :math:`[0, numVertices[`.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::size_t getId() const;
	// rst:	.. function:: std::shared_ptr<Rule> getRule() const
	// rst:
	// rst:		:returns: the rule the vertex belongs to.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::shared_ptr<Rule> getRule() const;
	// rst: .. function:: Rule::Vertex getCore() const
	// rst:
	// rst:		:returns: the descriptor for this vertex in the core graph.
	Rule::Vertex getCore() const;
	// rst:	.. function:: std::size_t getDegree() const
	// rst:
	// rst:		:returns: the degree of the vertex.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::size_t getDegree() const;
	// rst:	.. function:: IncidentEdgeRange incidentEdges() const
	// rst:
	// rst:		:returns: a range of incident edges to this vertex.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	IncidentEdgeRange incidentEdges() const;
	// rst:	.. function:: const std::string &getStringLabel() const
	// rst:
	// rst: 	:returns: the string label of the vertex.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	const std::string &getStringLabel() const;
	// rst:	.. function:: AtomId getAtomId() const
	// rst:
	// rst:		:returns: the atom id of the vertex.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	AtomId getAtomId() const;
	// rst:	.. function:: Charge getCharge() const
	// rst:
	// rst:		:returns: the charge of the vertex.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	Charge getCharge() const;
	// rst:	.. function:: bool getRadical() const
	// rst:
	// rst:		:returns: the radical status of the vertex.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	bool getRadical() const;
private:
	std::shared_ptr<Rule> r;
	std::size_t vId;
};
// rst-class-end:

// rst-class: Rule::RightGraph::Edge
// rst:
// rst:		A descriptor of either an edge in a rule, or a null edge.
// rst-class-start:

class Rule::RightGraph::Edge {
	friend class Rule::Edge;
	friend class EdgeIterator;
	friend class IncidentEdgeIterator;
	Edge(std::shared_ptr<Rule> r, std::size_t vId, std::size_t eId);
public:
	// rst:	.. function:: Edge()
	// rst:
	// rst:		Constructs a null descriptor.
	Edge();
	friend std::ostream &operator<<(std::ostream &s, const Edge &e);
	friend bool operator==(const Edge &e1, const Edge &e2);
	friend bool operator!=(const Edge &e1, const Edge &e2);
	friend bool operator<(const Edge &e1, const Edge &e2);
	// rst:	.. function:: bool isNull() const
	// rst:
	// rst:		:returns: whether this is a null descriptor or not.
	bool isNull() const;
	// rst:	.. function:: std::shared_ptr<Rule> getRule() const
	// rst:
	// rst:		:returns: the rule the edge belongs to.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::shared_ptr<Rule> getRule() const;
	// rst: .. function:: Rule::Edge getCore() const
	// rst:
	// rst:		:returns: the descriptor for this edge in the core graph.
	Rule::Edge getCore() const;
	// rst:	.. function:: Vertex source() const
	// rst:
	// rst:		:returns: the source vertex of the edge.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	Vertex source() const;
	// rst:	.. function:: Vertex target() const
	// rst:
	// rst:		:returns: the target vertex of the edge.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	Vertex target() const;
	// rst:	.. function:: const std::string &getStringLabel() const
	// rst:
	// rst: 	:returns: the string label of the edge.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	const std::string &getStringLabel() const;
	// rst:	.. function:: BondType getBondType() const
	// rst:
	// rst:		:returns: the bond type of the edge.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	BondType getBondType() const;
private:
	std::shared_ptr<Rule> r;
	std::size_t vId, eId;
};
// rst-class-end:


// VertexList
// -----------------------------------------------------------------------------

// rst-class: Rule::RightGraph::VertexIterator
// rst:
// rst:		An iterator for traversing all vertices in a rule.
// rst:		It models a forward iterator.
// rst-class-start:

class Rule::RightGraph::VertexIterator : public boost::iterator_facade<VertexIterator, Vertex, std::forward_iterator_tag, Vertex> {
	friend class Rule;
	VertexIterator(std::shared_ptr<Rule> r);
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
	std::shared_ptr<Rule> r;
	std::size_t vId;
};
// rst-class-end:

// rst-class: Rule::RightGraph::VertexRange
// rst:
// rst:		A range of all vertices in a rule.
// rst-class-begin:

struct Rule::RightGraph::VertexRange {
	using iterator = VertexIterator;
	using const_iterator = iterator;
private:
	friend class Rule;
	VertexRange(std::shared_ptr<Rule> r);
public:
	VertexIterator begin() const;
	VertexIterator end() const;
private:
	std::shared_ptr<Rule> r;
};
// rst-class-end:


// EdgeList
// -----------------------------------------------------------------------------

// rst-class: Rule::RightGraph::EdgeIterator
// rst:
// rst:		An iterator for traversing all edges in a rule.
// rst:		It models a forward iterator.
// rst-class-start:

class Rule::RightGraph::EdgeIterator : public boost::iterator_facade<EdgeIterator, Edge, std::forward_iterator_tag, Edge> {
	friend class Rule;
	EdgeIterator(std::shared_ptr<Rule> r);
public:
	// rst:	.. function:: EdgeIterator()
	// rst:
	// rst:		Construct a past-the-end iterator.
	EdgeIterator();
private:
	friend class boost::iterator_core_access;
	Edge dereference() const;
	bool equal(const EdgeIterator &iter) const;
	void increment();
	void advanceToValid();
private:
	std::shared_ptr<Rule> r;
	std::size_t vId, eId;
};
// rst-class-end:

// rst-class: Rule::RightGraph::EdgeRange
// rst:
// rst:		A range of all edges in a rule.
// rst-class-begin:

struct Rule::RightGraph::EdgeRange {
	using iterator = EdgeIterator;
	using const_iterator = iterator;
private:
	friend class Rule;
	EdgeRange(std::shared_ptr<Rule> r);
public:
	EdgeIterator begin() const;
	EdgeIterator end() const;
private:
	std::shared_ptr<Rule> r;
};
// rst-class-end:


// Incidence
// -----------------------------------------------------------------------------

// rst-class: Rule::RightGraph::IncidnetEdgeIterator
// rst:
// rst:		An iterator for traversing all edges in a rule.
// rst:		It models a forward iterator.
// rst-class-start:

class Rule::RightGraph::IncidentEdgeIterator : public boost::iterator_facade<IncidentEdgeIterator, Edge, std::forward_iterator_tag, Edge> {
	friend class Rule;
	IncidentEdgeIterator(std::shared_ptr<Rule> r, std::size_t vId);
public:
	// rst:	.. function:: IncidentEdgeIterator()
	// rst:
	// rst:		Construct a past-the-end iterator.
	IncidentEdgeIterator();
private:
	friend class boost::iterator_core_access;
	Edge dereference() const;
	bool equal(const IncidentEdgeIterator &iter) const;
	void increment();
private:
	std::shared_ptr<Rule> r;
	std::size_t vId, eId;
};
// rst-class-end:

// rst-class: Rule::RightGraph::IncidentEdgeRange
// rst:
// rst:		A range of all incident edges to a vertex in a rule.
// rst-class-begin:

struct Rule::RightGraph::IncidentEdgeRange {
	using iterator = IncidentEdgeIterator;
	using const_iterator = iterator;
private:
	friend class Vertex;
	IncidentEdgeRange(std::shared_ptr<Rule> r, std::size_t vId);
public:
	IncidentEdgeIterator begin() const;
	IncidentEdgeIterator end() const;
private:
	std::shared_ptr<Rule> r;
	std::size_t vId;
};
// rst-class-end:


// rst:
// rst: ========================================================================
// rst: Core
// rst: ========================================================================
// rst:

// Graph
// -----------------------------------------------------------------------------

// rst-class: Rule::Vertex
// rst:
// rst:		A descriptor of either a vertex in a rule, or a null vertex.
// rst-class-start:

class Rule::Vertex {
	friend class Rule;
	friend class Edge;
	friend class VertexIterator;
	friend class VertexRange;
	Vertex(std::shared_ptr<Rule> r, std::size_t vId);
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
	// rst:		:returns: the index of the vertex. It will be in the range :math:`[0, numVertices[`.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::size_t getId() const;
	// rst:	.. function:: std::shared_ptr<Rule> getRule() const
	// rst:
	// rst:		:returns: the rule the vertex belongs to.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::shared_ptr<Rule> getRule() const;
	// rst: .. function:: LeftGraph::Vertex getLeft() const
	// rst:
	// rst:		:returns: a null descriptor if this vertex is not in the left graph,
	// rst:			otherwise the descriptor of this vertex in the left graph.
	LeftGraph::Vertex getLeft() const;
	// rst: .. function:: ContextGraph::Vertex getContext() const
	// rst:
	// rst:		:returns: a null descriptor if this vertex is not in the context graph,
	// rst:			otherwise the descriptor of this vertex in the context graph.
	ContextGraph::Vertex getContext() const;
	// rst: .. function:: RightGraph::Vertex getRight() const
	// rst:
	// rst:		:returns: a null descriptor if this vertex is not in the right graph,
	// rst:			otherwise the descriptor of this vertex in the right graph.
	RightGraph::Vertex getRight() const;
	// rst:	.. function:: std::size_t getDegree() const
	// rst:
	// rst:		:returns: the degree of the vertex.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::size_t getDegree() const;
	// rst:	.. function:: IncidentEdgeRange incidentEdges() const
	// rst:
	// rst:		:returns: a range of incident edges to this vertex.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	IncidentEdgeRange incidentEdges() const;
private:
	std::shared_ptr<Rule> r;
	std::size_t vId;
};
// rst-class-end:

// rst-class: Rule::Edge
// rst:
// rst:		A descriptor of either an edge in a rule, or a null edge.
// rst-class-start:

class Rule::Edge {
	friend class Rule::LeftGraph::Edge;
	friend class Rule::ContextGraph::Edge;
	friend class Rule::RightGraph::Edge;
	friend class EdgeIterator;
	friend class IncidentEdgeIterator;
	Edge(std::shared_ptr<Rule> r, std::size_t vId, std::size_t eId);
public:
	// rst:	.. function:: Edge()
	// rst:
	// rst:		Constructs a null descriptor.
	Edge();
	friend std::ostream &operator<<(std::ostream &s, const Edge &e);
	friend bool operator==(const Edge &e1, const Edge &e2);
	friend bool operator!=(const Edge &e1, const Edge &e2);
	friend bool operator<(const Edge &e1, const Edge &e2);
	// rst:	.. function:: bool isNull() const
	// rst:
	// rst:		:returns: whether this is a null descriptor or not.
	bool isNull() const;
	// rst:	.. function:: std::shared_ptr<Rule> getRule() const
	// rst:
	// rst:		:returns: the rule the edge belongs to.
	// rst:		:throws: :cpp:class:`LogicError` if it is a null descriptor.
	std::shared_ptr<Rule> getRule() const;
	// rst: .. function:: LeftGraph::Edge getLeft() const
	// rst:
	// rst:		:returns: a null descriptor if this edge is not in the left graph,
	// rst:			otherwise the descriptor of this edge in the left graph.
	LeftGraph::Edge getLeft() const;
	// rst: .. function:: ContextGraph::Edge getContext() const
	// rst:
	// rst:		:returns: a null descriptor if this edge is not in the context graph,
	// rst:			otherwise the descriptor of this edge in the context graph.
	ContextGraph::Edge getContext() const;
	// rst: .. function:: RightGraph::Edge getRight() const
	// rst:
	// rst:		:returns: a null descriptor if this edge is not in the right graph,
	// rst:			otherwise the descriptor of this edge in the right graph.
	RightGraph::Edge getRight() const;
	// rst:	.. function:: Vertex source() const
	// rst:
	// rst:		:returns: the source vertex of the edge.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	Vertex source() const;
	// rst:	.. function:: Vertex target() const
	// rst:
	// rst:		:returns: the target vertex of the edge.
	// rst: 	:throws: :cpp:class:`LogicError` if it is a null descriptor.
	Vertex target() const;
private:
	std::shared_ptr<Rule> r;
	std::size_t vId, eId;
};
// rst-class-end:


// VertexList
// -----------------------------------------------------------------------------

// rst-class: Rule::VertexIterator
// rst:
// rst:		An iterator for traversing all vertices in a rule.
// rst:		It models a forward iterator.
// rst-class-start:

class Rule::VertexIterator : public boost::iterator_facade<VertexIterator, Vertex, std::forward_iterator_tag, Vertex> {
	friend class Rule;
	VertexIterator(std::shared_ptr<Rule> r);
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
	std::shared_ptr<Rule> r;
	std::size_t vId;
};
// rst-class-end:

// rst-class: Rule::VertexRange
// rst:
// rst:		A range of all vertices in a rule.
// rst-class-begin:

struct Rule::VertexRange {
	using iterator = VertexIterator;
	using const_iterator = iterator;
private:
	friend class Rule;
	VertexRange(std::shared_ptr<Rule> r);
public:
	VertexIterator begin() const;
	VertexIterator end() const;
	Vertex operator[](std::size_t i) const;
private:
	std::shared_ptr<Rule> r;
};
// rst-class-end:


// EdgeList
// -----------------------------------------------------------------------------

// rst-class: Rule::EdgeIterator
// rst:
// rst:		An iterator for traversing all edges in a rule.
// rst:		It models a forward iterator.
// rst-class-start:

class Rule::EdgeIterator : public boost::iterator_facade<EdgeIterator, Edge, std::forward_iterator_tag, Edge> {
	friend class Rule;
	EdgeIterator(std::shared_ptr<Rule> r);
public:
	// rst:	.. function:: EdgeIterator()
	// rst:
	// rst:		Construct a past-the-end iterator.
	EdgeIterator();
private:
	friend class boost::iterator_core_access;
	Edge dereference() const;
	bool equal(const EdgeIterator &iter) const;
	void increment();
	void advanceToValid();
private:
	std::shared_ptr<Rule> r;
	std::size_t vId, eId;
};
// rst-class-end:

// rst-class: Rule::EdgeRange
// rst:
// rst:		A range of all edges in a rule.
// rst-class-begin:

struct Rule::EdgeRange {
	using iterator = EdgeIterator;
	using const_iterator = iterator;
private:
	friend class Rule;
	EdgeRange(std::shared_ptr<Rule> r);
public:
	EdgeIterator begin() const;
	EdgeIterator end() const;
private:
	std::shared_ptr<Rule> r;
};
// rst-class-end:


// Incidence
// -----------------------------------------------------------------------------

// rst-class: Rule::IncidnetEdgeIterator
// rst:
// rst:		An iterator for traversing all edges in a rule.
// rst:		It models a forward iterator.
// rst-class-start:

class Rule::IncidentEdgeIterator : public boost::iterator_facade<IncidentEdgeIterator, Edge, std::forward_iterator_tag, Edge> {
	friend class Rule;
	IncidentEdgeIterator(std::shared_ptr<Rule> r, std::size_t vId);
public:
	// rst:	.. function:: IncidentEdgeIterator()
	// rst:
	// rst:		Construct a past-the-end iterator.
	IncidentEdgeIterator();
private:
	friend class boost::iterator_core_access;
	Edge dereference() const;
	bool equal(const IncidentEdgeIterator &iter) const;
	void increment();
private:
	std::shared_ptr<Rule> r;
	std::size_t vId, eId;
};
// rst-class-end:

// rst-class: Rule::IncidentEdgeRange
// rst:
// rst:		A range of all incident edges to a vertex in a rule.
// rst-class-begin:

struct Rule::IncidentEdgeRange {
	using iterator = IncidentEdgeIterator;
	using const_iterator = iterator;
private:
	friend class Vertex;
	IncidentEdgeRange(std::shared_ptr<Rule> r, std::size_t vId);
public:
	IncidentEdgeIterator begin() const;
	IncidentEdgeIterator end() const;
private:
	std::shared_ptr<Rule> r;
	std::size_t vId;
};
// rst-class-end:


//##############################################################################
// Implementation Details
//##############################################################################

//==============================================================================
// Left
//==============================================================================

inline Rule::LeftGraph::LeftGraph(std::shared_ptr<Rule> r) : r(r) { }

inline std::shared_ptr<Rule> Rule::LeftGraph::getRule() const {
	return r;
}

inline Rule::LeftGraph::VertexRange Rule::LeftGraph::vertices() const {
	return VertexRange(r);
}

inline Rule::LeftGraph::EdgeRange Rule::LeftGraph::edges() const {
	return EdgeRange(r);
}

// VertexRange
//------------------------------------------------------------------------------

inline Rule::LeftGraph::VertexRange::VertexRange(std::shared_ptr<Rule> r) : r(r) { }

inline Rule::LeftGraph::VertexIterator Rule::LeftGraph::VertexRange::begin() const {
	return VertexIterator(r);
}

inline Rule::LeftGraph::VertexIterator Rule::LeftGraph::VertexRange::end() const {
	return VertexIterator();
}

// EdgeRange
//------------------------------------------------------------------------------

inline Rule::LeftGraph::EdgeRange::EdgeRange(std::shared_ptr<Rule> r) : r(r) { }

inline Rule::LeftGraph::EdgeIterator Rule::LeftGraph::EdgeRange::begin() const {
	return EdgeIterator(r);
}

inline Rule::LeftGraph::EdgeIterator Rule::LeftGraph::EdgeRange::end() const {
	return EdgeIterator();
}

// IncidentEdgeRange
//------------------------------------------------------------------------------

inline Rule::LeftGraph::IncidentEdgeRange::IncidentEdgeRange(std::shared_ptr<Rule> r, std::size_t vId) : r(r), vId(vId) { }

inline Rule::LeftGraph::IncidentEdgeIterator Rule::LeftGraph::IncidentEdgeRange::begin() const {
	return IncidentEdgeIterator(r, vId);
}

inline Rule::LeftGraph::IncidentEdgeIterator Rule::LeftGraph::IncidentEdgeRange::end() const {
	return IncidentEdgeIterator();
}

//==============================================================================
// Context
//==============================================================================

inline Rule::ContextGraph::ContextGraph(std::shared_ptr<Rule> r) : r(r) { }

inline std::shared_ptr<Rule> Rule::ContextGraph::getRule() const {
	return r;
}

inline Rule::ContextGraph::VertexRange Rule::ContextGraph::vertices() const {
	return VertexRange(r);
}

inline Rule::ContextGraph::EdgeRange Rule::ContextGraph::edges() const {
	return EdgeRange(r);
}

// VertexRange
//------------------------------------------------------------------------------

inline Rule::ContextGraph::VertexRange::VertexRange(std::shared_ptr<Rule> r) : r(r) { }

inline Rule::ContextGraph::VertexIterator Rule::ContextGraph::VertexRange::begin() const {
	return VertexIterator(r);
}

inline Rule::ContextGraph::VertexIterator Rule::ContextGraph::VertexRange::end() const {
	return VertexIterator();
}

// EdgeRange
//------------------------------------------------------------------------------

inline Rule::ContextGraph::EdgeRange::EdgeRange(std::shared_ptr<Rule> r) : r(r) { }

inline Rule::ContextGraph::EdgeIterator Rule::ContextGraph::EdgeRange::begin() const {
	return EdgeIterator(r);
}

inline Rule::ContextGraph::EdgeIterator Rule::ContextGraph::EdgeRange::end() const {
	return EdgeIterator();
}

// IncidentEdgeRange
//------------------------------------------------------------------------------

inline Rule::ContextGraph::IncidentEdgeRange::IncidentEdgeRange(std::shared_ptr<Rule> r, std::size_t vId) : r(r), vId(vId) { }

inline Rule::ContextGraph::IncidentEdgeIterator Rule::ContextGraph::IncidentEdgeRange::begin() const {
	return IncidentEdgeIterator(r, vId);
}

inline Rule::ContextGraph::IncidentEdgeIterator Rule::ContextGraph::IncidentEdgeRange::end() const {
	return IncidentEdgeIterator();
}

//==============================================================================
// Right
//==============================================================================

inline Rule::RightGraph::RightGraph(std::shared_ptr<Rule> r) : r(r) { }

inline std::shared_ptr<Rule> Rule::RightGraph::getRule() const {
	return r;
}

inline Rule::RightGraph::VertexRange Rule::RightGraph::vertices() const {
	return VertexRange(r);
}

inline Rule::RightGraph::EdgeRange Rule::RightGraph::edges() const {
	return EdgeRange(r);
}


// VertexRange
//------------------------------------------------------------------------------

inline Rule::RightGraph::VertexRange::VertexRange(std::shared_ptr<Rule> r) : r(r) { }

inline Rule::RightGraph::VertexIterator Rule::RightGraph::VertexRange::begin() const {
	return VertexIterator(r);
}

inline Rule::RightGraph::VertexIterator Rule::RightGraph::VertexRange::end() const {
	return VertexIterator();
}

// EdgeRange
//------------------------------------------------------------------------------

inline Rule::RightGraph::EdgeRange::EdgeRange(std::shared_ptr<Rule> r) : r(r) { }

inline Rule::RightGraph::EdgeIterator Rule::RightGraph::EdgeRange::begin() const {
	return EdgeIterator(r);
}

inline Rule::RightGraph::EdgeIterator Rule::RightGraph::EdgeRange::end() const {
	return EdgeIterator();
}


// IncidentEdgeRange
//------------------------------------------------------------------------------

inline Rule::RightGraph::IncidentEdgeRange::IncidentEdgeRange(std::shared_ptr<Rule> r, std::size_t vId) : r(r), vId(vId) { }

inline Rule::RightGraph::IncidentEdgeIterator Rule::RightGraph::IncidentEdgeRange::begin() const {
	return IncidentEdgeIterator(r, vId);
}

inline Rule::RightGraph::IncidentEdgeIterator Rule::RightGraph::IncidentEdgeRange::end() const {
	return IncidentEdgeIterator();
}

//==============================================================================
// Core
//==============================================================================

// VertexRange
//------------------------------------------------------------------------------

inline Rule::VertexRange::VertexRange(std::shared_ptr<Rule> r) : r(r) { }

inline Rule::VertexIterator Rule::VertexRange::begin() const {
	return VertexIterator(r);
}

inline Rule::VertexIterator Rule::VertexRange::end() const {
	return VertexIterator();
}

inline Rule::Vertex Rule::VertexRange::operator[](std::size_t i) const {
	return Vertex(r, i); // it will convert to a null descriptor if out of range
}

// EdgeRange
//------------------------------------------------------------------------------

inline Rule::EdgeRange::EdgeRange(std::shared_ptr<Rule> r) : r(r) { }

inline Rule::EdgeIterator Rule::EdgeRange::begin() const {
	return EdgeIterator(r);
}

inline Rule::EdgeIterator Rule::EdgeRange::end() const {
	return EdgeIterator();
}

// IncidentEdgeRange
//------------------------------------------------------------------------------

inline Rule::IncidentEdgeRange::IncidentEdgeRange(std::shared_ptr<Rule> r, std::size_t vId) : r(r), vId(vId) { }

inline Rule::IncidentEdgeIterator Rule::IncidentEdgeRange::begin() const {
	return IncidentEdgeIterator(r, vId);
}

inline Rule::IncidentEdgeIterator Rule::IncidentEdgeRange::end() const {
	return IncidentEdgeIterator();
}

} // namespace mod

#endif /* MOD_RULEGRAPHINTERFACE_H */