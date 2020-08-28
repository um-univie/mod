#ifndef MOD_LIB_GRAPH_PROPERTY_H
#define MOD_LIB_GRAPH_PROPERTY_H

#include <mod/Error.hpp>
#include <mod/lib/Graph/GraphDecl.hpp>
#include <mod/lib/LabelledGraph.hpp>
#include <mod/lib/IO/IO.hpp>

#include <vector>

namespace mod {
namespace lib {
namespace Graph {

template<typename Derived, typename VertexType, typename EdgeType>
struct Prop {
	using Vertex = typename boost::graph_traits<GraphType>::vertex_descriptor;
	using Edge = typename boost::graph_traits<GraphType>::edge_descriptor;
public:
	Prop(const Prop &) = delete;
	Prop(Prop &&) = delete;
	Prop &operator=(const Prop &) = delete;
	Prop &operator=(Prop &&) = delete;
public:
	void verify(const GraphType *g) const;
	explicit Prop(const GraphType &g);
	Prop(const Prop &other, const GraphType &g);
	void addVertex(Vertex v, const VertexType &label);
	void addEdge(Edge e, const EdgeType &label);
	const VertexType &operator[](Vertex v) const;
	const EdgeType &operator[](Edge e) const;
	Derived &getDerived();
	const Derived &getDerived() const;
protected:
	const GraphType &g;
	std::vector<VertexType> vertexState;
	std::vector<EdgeType> edgeState;
public:
	using Handler = IdentityPropertyHandler;
};

template<typename Derived, typename VertexType, typename EdgeType, typename VertexOrEdge>
auto get(const Prop<Derived, VertexType, EdgeType> &p, VertexOrEdge ve) -> decltype(p[ve]) {
	return p[ve];
}

//------------------------------------------------------------------------------
// Implementation
//------------------------------------------------------------------------------

template<typename Derived, typename VertexType, typename EdgeType>
void Prop<Derived, VertexType, EdgeType>::verify(const GraphType *g) const {
	if(g != &this->g) {
		IO::log() << "Different graphs: g = " << (std::uintptr_t) g << ", &this->g = " << (std::uintptr_t) &this->g
		          << std::endl;
		MOD_ABORT;
	}
	if(num_vertices(this->g) != vertexState.size()) {
		IO::log() << "Different sizes: num_vertices(this->g) = " << num_vertices(this->g) << ", vertexLabels.size() = "
		          << vertexState.size() << std::endl;
		MOD_ABORT;
	}
	if(num_edges(this->g) != edgeState.size()) {
		IO::log() << "Different sizes: num_edges(this->g) = " << num_edges(this->g) << ", edgeLabels.size() = "
		          << edgeState.size() << std::endl;
		MOD_ABORT;
	}
}

template<typename Derived, typename VertexType, typename EdgeType>
Prop<Derived, VertexType, EdgeType>::Prop(const GraphType &g) : g(g) {}

template<typename Derived, typename VertexType, typename EdgeType>
Prop<Derived, VertexType, EdgeType>::Prop(const Prop &other, const GraphType &g)
		: g(g), vertexState(other.vertexState), edgeState(other.edgeState) {}

template<typename Derived, typename VertexType, typename EdgeType>
void Prop<Derived, VertexType, EdgeType>::addVertex(Vertex v, const VertexType &value) {
	assert(num_vertices(g) == vertexState.size() + 1);
	assert(get(boost::vertex_index_t(), g, v) == vertexState.size());
	vertexState.push_back(value);
	verify(&g);
}

template<typename Derived, typename VertexType, typename EdgeType>
void Prop<Derived, VertexType, EdgeType>::addEdge(Edge e, const EdgeType &value) {
	assert(num_edges(g) == edgeState.size() + 1);
	assert(get(boost::edge_index_t(), g, e) == edgeState.size());
	edgeState.push_back(value);
	verify(&g);
}

template<typename Derived, typename VertexType, typename EdgeType>
const VertexType &Prop<Derived, VertexType, EdgeType>::operator[](Vertex v) const {
	return vertexState[get(boost::vertex_index_t(), g, v)];
}

template<typename Derived, typename VertexType, typename EdgeType>
const EdgeType &Prop<Derived, VertexType, EdgeType>::operator[](Edge e) const {
	assert(get(boost::edge_index_t(), g, e) < edgeState.size());
	return edgeState[get(boost::edge_index_t(), g, e)];
}

template<typename Derived, typename VertexType, typename EdgeType>
Derived &Prop<Derived, VertexType, EdgeType>::getDerived() {
	return static_cast<Derived &> (*this);
}

template<typename Derived, typename VertexType, typename EdgeType>
const Derived &Prop<Derived, VertexType, EdgeType>::getDerived() const {
	return static_cast<const Derived &> (*this);
}

} // namespace Graph
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPH_PROPERTY_H */
