#ifndef MOD_GRAPH_H
#define MOD_GRAPH_H

#include <mod/Config.h>

#include <boost/iterator/iterator_facade.hpp>

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace mod {
struct AtomId;
struct Charge;
enum class BondType;
template<typename Sig> class Function;
class GraphPrinter;
namespace lib {
namespace Graph {
class Single;
} // namespace Graph
namespace IO {
namespace Graph {
namespace Write {
class Options;
} // namespace Write
} // namespace Graph
} // namespace IO
} // namespace lib

// rst-class: Graph
// rst:
// rst:		This class models an undirected graph with labels on vertices and edges,
// rst:		without loops and without parallel edges.
// rst:		Certain labels are regarded as models of chemical atoms and bonds.
// rst:		See :ref:`mol-enc` for more information on this.
// rst:
// rst-class-start:

struct Graph {
	class Edge;
	class VertexIterator;
	class VertexRange;
	class EdgeIterator;
	class IncidentEdgeIterator;
	class IncidentEdgeRange;

	// rst-nested: Graph::Vertex
	// rst:
	// rst:		A descriptor of either a vertex in a graph, or a null vertex.
	// rst-nested-start:

	class Vertex {
		friend class Edge;
		friend class VertexIterator;
		friend class VertexRange;
		Vertex(std::shared_ptr<Graph> g, std::size_t vId);
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
		// rst:			:returns: the index of the vertex. It will be in the range :math:`[0, numVertices[`.
		// rst:			:throws: :cpp:class:`LogicError` if it is a null descriptor.
		std::size_t getId() const;
		// rst:		.. function:: std::shared_ptr<Graph> getGraph() const
		// rst:
		// rst:			:returns: the graph the vertex belongs to.
		// rst:			:throws: :cpp:class:`LogicError` if it is a null descriptor.
		std::shared_ptr<Graph> getGraph() const;
		// rst:		.. function:: std::size_t getDegree() const
		// rst:
		// rst:			:returns: the degree of the vertex.
		// rst:			:throws: :cpp:class:`LogicError` if it is a null descriptor.
		std::size_t getDegree() const;
		// rst:		.. function:: IncidentEdgeRange incidentEdges() const
		// rst:
		// rst:			:returns: a range of incident edges to this vertex.
		// rst:			:throws: :cpp:class:`LogicError` if it is a null descriptor.
		IncidentEdgeRange incidentEdges() const;
		// rst:		.. function:: const std::string &getStringLabel() const
		// rst:
		// rst: 		:returns: the string label of the vertex.
		// rst: 		:throws: :cpp:class:`LogicError` if it is a null descriptor.
		const std::string &getStringLabel() const;
		// rst:		.. function:: AtomId getAtomId() const
		// rst:
		// rst:			:returns: the atom id of the vertex.
		// rst: 		:throws: :cpp:class:`LogicError` if it is a null descriptor.
		AtomId getAtomId() const;
		// rst:		.. function:: Charge getCharge() const
		// rst:
		// rst:			:returns: the charge of the vertex.
		// rst: 		:throws: :cpp:class:`LogicError` if it is a null descriptor.
		Charge getCharge() const;
	private:
		std::shared_ptr<Graph> g;
		std::size_t vId;
	};
	// rst-nested-end:

	// rst-nested: Graph::Edge
	// rst:
	// rst:		A descriptor of either an edge in a graph, or a null edge.
	// rst-nested-start:

	class Edge {
		friend class EdgeIterator;
		friend class IncidentEdgeIterator;
		Edge(std::shared_ptr<Graph> g, std::size_t vId, std::size_t eId);
	public:
		// rst:		.. function:: Edge()
		// rst:
		// rst:			Constructs a null descriptor.
		Edge();
		friend std::ostream &operator<<(std::ostream &s, const Edge &e);
		friend bool operator==(const Edge &e1, const Edge &e2);
		friend bool operator!=(const Edge &e1, const Edge &e2);
		// rst:		.. function:: std::shared_ptr<Graph> getGraph() const
		// rst:
		// rst:			:returns: the graph the edge belongs to.
		// rst:			:throws: :cpp:class:`LogicError` if it is a null descriptor.
		std::shared_ptr<Graph> getGraph() const;
		// rst:		.. function:: Vertex source() const
		// rst:
		// rst:			:returns: the source vertex of the edge.
		// rst: 		:throws: :cpp:class:`LogicError` if it is a null descriptor.
		Vertex source() const;
		// rst:		.. function:: Vertex target() const
		// rst:
		// rst:			:returns: the target vertex of the edge.
		// rst: 		:throws: :cpp:class:`LogicError` if it is a null descriptor.
		Vertex target() const;
		// rst:		.. function:: const std::string &getStringLabel() const
		// rst:
		// rst: 		:returns: the string label of the edge.
		// rst: 		:throws: :cpp:class:`LogicError` if it is a null descriptor.
		const std::string &getStringLabel() const;
		// rst:		.. function:: BondType getBondType() const
		// rst:
		// rst:			:returns: the bond type of the edge.
		// rst: 		:throws: :cpp:class:`LogicError` if it is a null descriptor.
		BondType getBondType() const;
	private:
		std::shared_ptr<Graph> g;
		std::size_t vId, eId;
	};
	// rst-nested-end:
public: // VertexList
	// rst-nested: Graph::VertexIterator
	// rst:
	// rst:		An iterator for traversing all vertices in a graph.
	// rst:		It models a forward iterator.
	// rst-nested-start:

	class VertexIterator : public boost::iterator_facade<VertexIterator, Vertex, std::forward_iterator_tag, Vertex> {
		friend class Graph;
		VertexIterator(std::shared_ptr<Graph> g);
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
		std::shared_ptr<Graph> g;
		std::size_t vId;
	};
	// rst-nested-end:

	// rst-nested: Graph::VertexRange
	// rst:
	// rst:		A range of all vertices in a graph.
	// rst-nested-begin:

	struct VertexRange {
		using iterator = VertexIterator;
		using const_iterator = iterator;
	private:
		friend class Graph;
		VertexRange(std::shared_ptr<Graph> g);
	public:
		VertexIterator begin() const;
		VertexIterator end() const;
		Vertex operator[](std::size_t i) const;
	private:
		std::shared_ptr<Graph> g;
	};
	// rst-nested-end:

public: // EdgeList
	// rst-nested: Graph::EdgeIterator
	// rst:
	// rst:		An iterator for traversing all edges in a graph.
	// rst:		It models a forward iterator.
	// rst-nested-start:

	class EdgeIterator : public boost::iterator_facade<EdgeIterator, Edge, std::forward_iterator_tag, Edge> {
		friend class Graph;
		EdgeIterator(std::shared_ptr<Graph> g);
	public:
		// rst:		.. function:: EdgeIterator()
		// rst:
		// rst:			Construct a past-the-end iterator.
		EdgeIterator();
	private:
		friend class boost::iterator_core_access;
		Edge dereference() const;
		bool equal(const EdgeIterator &iter) const;
		void increment();
		void advanceToValid();
	private:
		std::shared_ptr<Graph> g;
		std::size_t vId, eId;
	};
	// rst-nested-end:

	// rst-nested: Graph::EdgeRange
	// rst:
	// rst:		A range of all edges in a graph.
	// rst-nested-begin:

	struct EdgeRange {
		using iterator = EdgeIterator;
		using const_iterator = iterator;
	private:
		friend class Graph;
		EdgeRange(std::shared_ptr<Graph> g);
	public:
		EdgeIterator begin() const;
		EdgeIterator end() const;
	private:
		std::shared_ptr<Graph> g;
	};
	// rst-nested-end:
public: // Incidence
	// rst-nested: Graph::IncidnetEdgeIterator
	// rst:
	// rst:		An iterator for traversing all edges in a graph.
	// rst:		It models a forward iterator.
	// rst-nested-start:

	class IncidentEdgeIterator : public boost::iterator_facade<IncidentEdgeIterator, Edge, std::forward_iterator_tag, Edge> {
		friend class Graph;
		IncidentEdgeIterator(std::shared_ptr<Graph> g, std::size_t vId);
	public:
		// rst:		.. function:: IncidentEdgeIterator()
		// rst:
		// rst:			Construct a past-the-end iterator.
		IncidentEdgeIterator();
	private:
		friend class boost::iterator_core_access;
		Edge dereference() const;
		bool equal(const IncidentEdgeIterator &iter) const;
		void increment();
	private:
		std::shared_ptr<Graph> g;
		std::size_t vId, eId;
	};
	// rst-nested-end:

	// rst-nested: Graph::IncidentEdgeRange
	// rst:
	// rst:		A range of all incident edges to a vertex in a graph.
	// rst-nested-begin:

	struct IncidentEdgeRange {
		using iterator = IncidentEdgeIterator;
		using const_iterator = iterator;
	private:
		friend class Vertex;
		IncidentEdgeRange(std::shared_ptr<Graph> g, std::size_t vId);
	public:
		IncidentEdgeIterator begin() const;
		IncidentEdgeIterator end() const;
	private:
		std::shared_ptr<Graph> g;
		std::size_t vId;
	};
	// rst-nested-end:
private: // The actual class interface
	Graph(std::unique_ptr<lib::Graph::Single> g);
	Graph(const Graph&) = delete;
	Graph &operator=(const Graph&) = delete;
public:
	~Graph();
	// rst: .. function:: unsigned int getId() const
	// rst:
	// rst:		:returns: the unique instance id among :class:`Graph` objects.
	std::size_t getId() const;
	friend std::ostream &operator<<(std::ostream &s, const mod::Graph &g);
	lib::Graph::Single &getGraph() const;
public: // graph interface
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
public:
	// rst: .. function:: std::pair<std::string, std::string> print() const
	// rst:               std::pair<std::string, std::string> print(const GraphPrinter &first, const GraphPrinter &second) const
	// rst:
	// rst:		Print the graph, using either the default options or the options in ``first`` and ``second``.
	// rst:
	// rst:		:returns: a pair of names for the PDF-files that will be compiled in post-processing.
	std::pair<std::string, std::string> print() const;
	std::pair<std::string, std::string> print(const GraphPrinter &first, const GraphPrinter &second) const;
	// rst: .. function:: std::string getGMLString(bool withCoords = false) const
	// rst:
	// rst:		:returns: the :ref:`GML <graph-gml>` representation of the graph,
	// rst:		          optionally with generated 2D coordinates.
	// rst:		:throws: :any:`LogicError` when coordinates are requested, but
	// rst:		         none can be generated.
	std::string getGMLString(bool withCoords = false) const;
	// rst: .. function:: std::string printGML(bool withCoords = false) const
	// rst:
	// rst:		Print the :ref:`GML <graph-gml>` representation of the graph,
	// rst:		optionally with generated 2D coordinates.
	// rst:
	// rst:		:returns: the filename of the printed GML file.
	// rst:		:throws: :any:`LogicError` when coordinates are requested, but
	// rst:		         none can be generated.
	std::string printGML(bool withCoords = false) const;
	// rst: .. function:: const std::string &getName() const
	// rst:               void setName(std::string name) const
	// rst:
	// rst:		Access the name of the graph.
	const std::string &getName() const;
	void setName(std::string name) const;
	// rst: .. function:: const std::string &getSmiles() const
	// rst:
	// rst:		:returns: the canonical :ref:`SMILES string <graph-smiles>` of the graph, if it's a molecule.
	const std::string &getSmiles() const;
	// rst: .. function:: const std::string &getGraphDFS() const
	// rst:
	// rst:		:returns: the :ref:`GraphDFS <graph-graphDFS>` string of the graph.
	const std::string &getGraphDFS() const;
	// rst: .. function:: const std::string &getLinearEncoding() const
	// rst:
	// rst:		:returns: the :ref:`SMILES <graph-smiles>` string if the graph is a molecule, otherwise the :ref:`GraphDFS <graph-graphDFS>` string.
	const std::string &getLinearEncoding() const;
	// rst: .. function:: bool getIsMolecule() const
	// rst:
	// rst:		:returns: whether or not the graph models a molecule. See :ref:`mol-enc`.
	bool getIsMolecule() const;
	// rst: .. function:: double getEnergy() const
	// rst:
	// rst:		:returns: some energy value if the graph is a molecule.
	// rst:			The energy is calculated using Open Babel, unless already calculated or cached by :cpp:func:`Graph::cacheEnergy`.
	double getEnergy() const;
	// rst: .. function:: void cacheEnergy(double value) const
	// rst:
	// rst:		If the graph models a molecule, sets the energy to a given value.
	// rst:
	// rst:		:throws: :class:`LogicError` if the graph is not a molecule.
	void cacheEnergy(double value) const;
	// rst: .. function:: double getMolarMass() const
	// rst:
	// rst:		:returns: the molar mass of the graph, if it is a molecule.
	// rst:			The molar mass is calculated by Open Babel.
	double getMolarMass() const;
	// rst: .. function:: unsigned int vLabelCount(const std::string &label) const
	// rst:
	// rst:		:returns: the number of vertices in the graph with the given label.
	unsigned int vLabelCount(const std::string &label) const;
	// rst: .. function:: unsigned int eLabelCount(const std::string &label) const
	// rst:
	// rst:		:returns: the number of edges in the graph with the given label.
	unsigned int eLabelCount(const std::string &label) const;
	// rst: .. function:: std::size_t isomorphism(std::shared_ptr<Graph> g, std::size_t maxNumMatches) const
	// rst:
	// rst:		:returns: the number of isomorphisms found from this graph to ``g``, but at most ``maxNumMatches``.
	std::size_t isomorphism(std::shared_ptr<Graph> g, std::size_t maxNumMatches) const;
	// rst: .. function:: std::size_t monomorphism(std::shared_ptr<Graph> g, std::size_t maxNumMatches) const
	// rst:
	// rst:		:returns: the number of monomorphisms from this graph to ``g``, though at most ``maxNumMatches``.
	std::size_t monomorphism(std::shared_ptr<Graph> g, std::size_t maxNumMatches) const;
	// rst: .. function:: std::shared_ptr<Graph> makePermutation() const
	// rst:
	// rst:		:returns: a graph isomorphic to this, but with the vertex indices randomly permuted.
	std::shared_ptr<Graph> makePermutation() const;
	// rst: .. function:: void setImage(std::shared_ptr<Function<std::string()> > image)
	// rst:
	// rst:		Set a custom depiction for the graph. The depiction file used will be the string
	// rst:		returned by the given function, with ``.pdf`` appended.
	// rst:		The function will only be called once.
	// rst:		Give ``nullptr`` to use auto-generated depiction.
	void setImage(std::shared_ptr<Function<std::string()> > image);
	// rst: .. function:: std::shared_ptr<Function<std::string()> > getImage() const
	// rst:
	// rst:		:returns: the current custom depiction file function.
	std::shared_ptr<Function<std::string()> > getImage() const;
	// rst: .. function:: void setImageCommand(std::string cmd)
	// rst:
	// rst:		Set a command to be run in post-processing if a custom depiction is set.
	// rst:		The command is only run once.
	void setImageCommand(std::string cmd);
	// rst: .. function:: std::string getImageCommand() const
	// rst:
	// rst:		:returns: the current post-processing command.
	std::string getImageCommand() const;
private:
	struct Pimpl;
	std::unique_ptr<Pimpl> p;
public:
	// rst: .. function:: static std::shared_ptr<Graph> graphGMLString(const std::string &data)
	// rst:
	// rst:		:returns: a graph created from the given :ref:`GML <graph-gml>` data.
	// rst:		:throws: :class:`InputError` on bad input.
	static std::shared_ptr<Graph> graphGMLString(const std::string &data);
	// rst: .. function:: static std::shared_ptr<Graph> graphGML(const std::string &file)
	// rst:
	// rst:		:returns: a graph loaded from the given :ref:`GML <graph-gml>` file.
	// rst:		:throws: :class:`InputError` on bad input.
	static std::shared_ptr<Graph> graphGML(const std::string &file);
	// rst: .. function:: static std::shared_ptr<Graph> graphDFS(const std::string &graphDFS)
	// rst:
	// rst:		:returns: a graph loaded from the given :ref:`GraphDFS <graph-graphDFS>` string.
	// rst:		:throws: :class:`InputError` on bad input.
	static std::shared_ptr<Graph> graphDFS(const std::string &graphDFS);
	// rst: .. function:: static std::shared_ptr<Graph> smiles(const std::string &smiles)
	// rst:
	// rst:		:returns: a graph representing a molecule, loaded from the given :ref:`SMILES <graph-smiles>` string.
	// rst:		:throws: :class:`InputError` on bad input.
	static std::shared_ptr<Graph> smiles(const std::string &smiles);
	// rst: .. function:: static std::shared_ptr<Graph> makeGraph(std::unique_ptr<lib::Graph::Single> g)
	// rst:
	// rst:		:returns: a graph wrapping the given internal graph object.
	static std::shared_ptr<Graph> makeGraph(std::unique_ptr<lib::Graph::Single> g);
};
// rst-class-end:

struct GraphLess {

	bool operator()(std::shared_ptr<Graph> g1, std::shared_ptr<Graph> g2) const {
		return g1->getId() < g2->getId();
	}
};

// rst-class: GraphPrinter
// rst:
// rst: 	This class is used to configure how graphs are visualised.
// rst:
// rst: 	.. warning:: Some of these options greatly alter how graphs are depicted
// rst: 		and the result may not accurately represent the underlying graph,
// rst: 		and may make non-molecules look like molecules.
// rst:
// rst-class-start:

struct GraphPrinter {
	// rst: .. function GraphPrinter()
	// rst:
	// rst:		The default constructor enables edges as bonds and raised charges.
	GraphPrinter();
	GraphPrinter(const GraphPrinter&) = delete;
	GraphPrinter &operator=(const GraphPrinter&) = delete;
	~GraphPrinter();
	const lib::IO::Graph::Write::Options &getOptions() const;
	// rst: .. function:: void setMolDefault()
	// rst:
	// rst:		Shortcut for enabling all but thickening and index printing.
	void setMolDefault();
	// rst: .. function:: void setReactionDefault()
	// rst:
	// rst:		Shortcut for enabling all but thickening, index printing and simplification of carbon atoms.
	void setReactionDefault();
	// rst: .. function:: void disableAll()
	// rst:
	// rst: 	Disable all special printing features.
	void disableAll();
	// rst: .. function:: void enableAll()
	// rst:
	// rst: 	Enable all special printing features, except typewriter font.
	void enableAll();
	// rst: .. function:: void setEdgesAsBonds(bool value)
	// rst:               bool getEdgesAsBonds() const
	// rst:
	// rst:		Control whether edges with special labels are drawn as chemical bonds.
	void setEdgesAsBonds(bool value);
	bool getEdgesAsBonds() const;
	// rst: .. function:: void setCollapseHydrogens(bool value)
	// rst:               bool getCollapseHydrogens() const
	// rst:
	// rst:		Control whether vertices representing hydrogen atoms are collapsed into their neighbours labels.
	void setCollapseHydrogens(bool value);
	bool getCollapseHydrogens() const;
	// rst: .. function:: void setRaiseCharges(bool value)
	// rst:               bool getRaiseCharges() const
	// rst:
	// rst:		Control whether a vertex label suffix encoding a charge is written as a superscript to the rest of the label.
	void setRaiseCharges(bool value);
	bool getRaiseCharges() const;
	// rst: .. function:: void setSimpleCarbons(bool value)
	// rst:               bool getSimpleCarbons() const
	// rst:
	// rst:		Control whether some vertices encoding carbon atoms are depicted without any label.
	void setSimpleCarbons(bool value);
	bool getSimpleCarbons() const;
	// rst: .. function:: void setThick(bool value)
	// rst:               bool getThick() const
	// rst:
	// rst:		Control whether all edges are drawn thicker than normal and all labels are written in bold.
	void setThick(bool value);
	bool getThick() const;
	// rst: .. function:: void setWithColour(bool value)
	// rst:               bool getWithColour() const
	// rst:
	// rst:		Control whether colour is applied to certain elements of the graph which are molecule-like.
	void setWithColour(bool value);
	bool getWithColour() const;
	// rst: .. function:: void setWithIndex(bool value)
	// rst:               bool getWithIndex() const
	// rst:
	// rst:		Control whether the underlying indices of the vertices are printed.
	void setWithIndex(bool value);
	bool getWithIndex() const;
	// rst: .. function:: void setWithTexttt(bool value)
	// rst:               bool getWithTexttt() const
	// rst:
	// rst:		Control whether the vertex and edge labels are written with typewriter font.
	void setWithTexttt(bool value);
	bool getWithTexttt() const;
private:
	std::unique_ptr<lib::IO::Graph::Write::Options> options;
};
// rst-class-end:

//------------------------------------------------------------------------------
// Implementation Details
//------------------------------------------------------------------------------

// VertexRange
//------------------------------------------------------------------------------

inline Graph::VertexRange::VertexRange(std::shared_ptr<Graph> g) : g(g) { }

inline Graph::VertexIterator Graph::VertexRange::begin() const {
	return VertexIterator(g);
}

inline Graph::VertexIterator Graph::VertexRange::end() const {
	return VertexIterator();
}

inline Graph::Vertex Graph::VertexRange::operator[](std::size_t i) const {
	return Vertex(g, i); // it will convert to a null descriptor if out of range
}

// EdgeRange
//------------------------------------------------------------------------------

inline Graph::EdgeRange::EdgeRange(std::shared_ptr<Graph> g) : g(g) { }

inline Graph::EdgeIterator Graph::EdgeRange::begin() const {
	return EdgeIterator(g);
}

inline Graph::EdgeIterator Graph::EdgeRange::end() const {
	return EdgeIterator();
}

// IncidentEdgeRange
//------------------------------------------------------------------------------

inline Graph::IncidentEdgeRange::IncidentEdgeRange(std::shared_ptr<Graph> g, std::size_t vId) : g(g), vId(vId) { }

inline Graph::IncidentEdgeIterator Graph::IncidentEdgeRange::begin() const {
	return IncidentEdgeIterator(g, vId);
}

inline Graph::IncidentEdgeIterator Graph::IncidentEdgeRange::end() const {
	return IncidentEdgeIterator();
}

} // namespace mod

#endif /* MOD_GRAPH_H */
