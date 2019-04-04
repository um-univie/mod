#ifndef MOD_LIB_GRAPHPIMPL_H
#define MOD_LIB_GRAPHPIMPL_H

#include <mod/Error.h>

#include <cassert>

namespace mod {
namespace lib {

#define MOD_GRAPHPIMPL_Define_Vertex(Graph, OwnerLowerCase, getMacroGraph, g, Owner) \
																																										 \
Graph::Vertex::Vertex(std::shared_ptr<Owner> g, std::size_t vId) : g(g), vId(vId) {  \
	assert(g);                                                                    \
	using boost::vertices;                                                        \
	const auto &graph = getMacroGraph;                                            \
	const auto &vs = vertices(graph);                                             \
	if(vId >= std::distance(vs.first, vs.second)) {                               \
		this->g = nullptr;                                                          \
		this->vId = 0;                                                              \
	}                                                                             \
}                                                                               \
																																								\
Graph::Vertex::Vertex() : vId(0) { }                                            \
																																								\
std::ostream &operator<<(std::ostream &s, const Graph::Vertex &v) {             \
	s << #Graph "Vertex(";                                                        \
	if(!v.get ## Owner()) s << "null";                                            \
	else s << *v.get ## Owner() << ", " << v.getId();                             \
	return s << ")";                                                              \
}                                                                               \
																																								\
bool operator==(const Graph::Vertex &v1, const Graph::Vertex &v2) {             \
	return v1.g == v2.g && v1.vId == v2.vId;                                      \
}                                                                               \
																																								\
bool operator!=(const Graph::Vertex &v1, const Graph::Vertex &v2) {             \
	return !(v1 == v2);                                                           \
}                                                                               \
																																								\
bool operator<(const Graph::Vertex &v1, const Graph::Vertex &v2) {              \
	return std::tie(v1.g, v1.vId) < std::tie(v2.g, v2.vId);                       \
}                                                                               \
																																								\
std::size_t Graph::Vertex::hash() const {                                       \
	if(g) return getId();                                                         \
  else return -1;                                                               \
}                                                                               \
                                                                                \
Graph::Vertex::operator bool() const {                                          \
	return !isNull();                                                             \
}                                                                               \
                                                                                \
bool Graph::Vertex::isNull() const {                                            \
	return *this == Graph::Vertex();                                              \
}                                                                               \
                                                                                \
std::size_t Graph::Vertex::getId() const {                                      \
	if(!g) throw LogicError("Can not get id on a null vertex.");                  \
	const auto &graph = getMacroGraph;                                            \
	using boost::vertices;                                                        \
	auto v = *std::next(vertices(graph).first, vId);                              \
	return get(boost::vertex_index_t(), graph, v);                                \
}                                                                               \
																																								\
std::shared_ptr<Owner> Graph::Vertex::get ## Owner() const {                    \
	if(!g) throw LogicError("Can not get " #OwnerLowerCase " on a null vertex."); \
	return g;                                                                     \
}


#define MOD_GRAPHPIMPL_Define_Vertex_Undirected(Graph, GraphLowerCase, getMacroGraph, g) \
																																								\
std::size_t Graph::Vertex::getDegree() const {                                  \
	if(!g) throw LogicError("Can not get degree on a null vertex.");              \
	const auto &graph = getMacroGraph;                                            \
	using boost::vertices;                                                        \
	auto v = *std::next(vertices(graph).first, vId);                              \
	return out_degree(v, graph);                                                  \
}                                                                               \
																																								\
Graph::IncidentEdgeRange Graph::Vertex::incidentEdges() const {                 \
	if(!g) throw LogicError("Can not get incident edges on a null vertex.");      \
	return IncidentEdgeRange(g, vId);                                             \
}


#define MOD_GRAPHPIMPL_Define_Vertex_Directed(Graph, GraphLowerCase, getMacroGraph, g) \
																																								\
std::size_t Graph::Vertex::inDegree() const {                                   \
	if(!g) throw LogicError("Can not get in-degree on a null vertex.");           \
	const auto &graph = getMacroGraph;                                            \
	using boost::vertices;                                                        \
	auto v = *std::next(vertices(graph).first, vId);                              \
	return in_degree(v, graph);                                                   \
}                                                                               \
																																								\
std::size_t Graph::Vertex::outDegree() const {                                  \
	if(!g) throw LogicError("Can not get out-degree on a null vertex.");          \
	const auto &graph = getMacroGraph;                                            \
	using boost::vertices;                                                        \
	auto v = *std::next(vertices(graph).first, vId);                              \
	return out_degree(v, graph);                                                  \
}



#define MOD_GRAPHPIMPL_Define_Indices(Graph, OwnerLowerCase, getMacroGraph, g, getGraph, Owner)  \
																																								\
/*----------------------------------------------------------------------------*/\
/* Edge                                                                       */\
/*----------------------------------------------------------------------------*/\
																							                                  \
Graph::Edge::Edge(std::shared_ptr<Owner> g, std::size_t vId, std::size_t eId) : g(g), vId(vId), eId(eId) { \
	assert(g);                                                                    \
	using boost::vertices;                                                        \
	const auto &graph = getMacroGraph;                                            \
	const auto &vs = vertices(graph);                                             \
	if(vId >= std::distance(vs.first, vs.second)                                  \
			|| eId >= out_degree(*std::next(vertices(graph).first, vId), graph)) {    \
		this->g = nullptr;                                                          \
		this->vId = 0;                                                              \
		this->eId = 0;                                                              \
	}                                                                             \
}                                                                               \
																																								\
Graph::Edge::Edge() : vId(0), eId(0) { }                                        \
																																								\
std::ostream &operator<<(std::ostream &s, const Graph::Edge &e) {               \
	s << #Graph "Edge(";                                                          \
	if(e.g) s << *e.getGraph() << ", " << e.source().getId() << ", " << e.target().getId(); \
	else s << "null";                                                             \
	return s << ")";                                                              \
}                                                                               \
																																								\
bool operator==(const Graph::Edge &e1, const Graph::Edge &e2) {                 \
	if(e1.g && e2.g)                                                              \
		return (e1.source() == e2.source() && e1.target() == e2.target())           \
				|| (e1.source() == e2.target() && e1.target() == e2.source());          \
	else if(!e1.g && !e2.g) return true;                                          \
	else return false;                                                            \
}                                                                               \
																																								\
bool operator!=(const Graph::Edge &e1, const Graph::Edge &e2) {                 \
	return !(e1 == e2);                                                           \
}                                                                               \
																																								\
bool operator<(const Graph::Edge &e1, const Graph::Edge &e2) {                  \
	return std::tie(e1.g, e1.vId, e1.eId) < std::tie(e2.g, e2.vId, e2.eId);       \
}                                                                               \
																																								\
Graph::Edge::operator bool() const {                                            \
	return !isNull();                                                             \
}                                                                               \
                                                                                \
bool Graph::Edge::isNull() const {                                              \
	return *this == Graph::Edge();                                                \
}                                                                               \
																																								\
std::shared_ptr<Owner> Graph::Edge::getGraph() const {                          \
	if(!g) throw LogicError("Can not get graph on a null edge.");                 \
	return g;                                                                     \
}                                                                               \
																																								\
Graph::Vertex Graph::Edge::source() const {                                     \
	if(!g) throw LogicError("Can not get source on a null edge.");                \
	return Vertex(g, vId);                                                        \
}                                                                               \
																																								\
Graph::Vertex Graph::Edge::target() const {                                     \
	if(!g) throw LogicError("Can not get target on a null edge.");                \
	const auto &graph = getMacroGraph;                                            \
	using boost::vertices;                                                        \
	auto v = *std::next(vertices(graph).first, vId);                              \
	auto e = *std::next(out_edges(v, graph).first, eId);                          \
	using boost::target;                                                          \
	auto vTar = target(e, graph);                                                 \
	const auto &vs = vertices(graph);                                             \
	auto vTarIter = std::find(vs.first, vs.second, vTar);                         \
	return Vertex(g, std::distance(vs.first, vTarIter));                          \
}                                                                               \
																																								\
/*----------------------------------------------------------------------------*/\
/* VertexIterator                                                             */\
/*----------------------------------------------------------------------------*/\
																							                                  \
Graph::VertexIterator::VertexIterator(std::shared_ptr<Owner> g) : g(g), vId(0) {\
	assert(g);                                                                    \
	using boost::vertices;                                                        \
	const auto &graph = getMacroGraph;                                            \
	const auto &vs = vertices(graph);                                             \
	if(0 == std::distance(vs.first, vs.second))                                   \
		this->g = nullptr;                                                          \
}                                                                               \
																																								\
Graph::VertexIterator::VertexIterator() : g(nullptr), vId(0) { }                \
																																								\
Graph::Vertex Graph::VertexIterator::dereference() const {                      \
	return Graph::Vertex(g, vId);                                                 \
}                                                                               \
																																								\
bool Graph::VertexIterator::equal(const VertexIterator &iter) const {           \
	if(g) return g == iter.g && vId == iter.vId;                                  \
	else return g == iter.g;                                                      \
}                                                                               \
																																								\
void Graph::VertexIterator::increment() {                                       \
	++this->vId;                                                                  \
	using boost::vertices;                                                        \
	const auto &graph = getMacroGraph;                                            \
	const auto &vs = vertices(graph);                                             \
	if(vId >= std::distance(vs.first, vs.second)) {                               \
		this->g = nullptr;                                                          \
		this->vId = 0;                                                              \
	}                                                                             \
}                                                                               \
																																								\
/*----------------------------------------------------------------------------*/\
/* EdgeIterator												                                        */\
/*----------------------------------------------------------------------------*/\
																							                                  \
Graph::EdgeIterator::EdgeIterator(std::shared_ptr<Owner> g) : g(g), vId(0), eId(0) { \
	assert(g);                                                                    \
	advanceToValid();                                                             \
}                                                                               \
																																								\
Graph::EdgeIterator::EdgeIterator() : g(nullptr), vId(0), eId(0) { }            \
																																								\
Graph::Edge Graph::EdgeIterator::dereference() const {                          \
	return Edge(g, vId, eId);                                                     \
}                                                                               \
																																								\
bool Graph::EdgeIterator::equal(const EdgeIterator &iter) const {               \
	if(g) return g == iter.g && vId == iter.vId && eId == iter.eId;               \
	else return g == iter.g;                                                      \
}                                                                               \
																																								\
void Graph::EdgeIterator::increment() {                                         \
	eId++;                                                                        \
	advanceToValid();                                                             \
}                                                                               \
																																								\
void Graph::EdgeIterator::advanceToValid() {                                    \
	assert(g);                                                                    \
	using boost::vertices;                                                        \
	const auto &graph = getMacroGraph;                                            \
	const auto &vs = vertices(graph);                                             \
	for(; vId < std::distance(vs.first, vs.second); vId++, eId = 0) {             \
		using boost::vertices;                                                                    \
		for(auto v = *std::next(vertices(graph).first, vId); eId < out_degree(v, graph); eId++) { \
			auto e = *std::next(out_edges(v, graph).first, eId);                                    \
			auto vTar = target(e, graph);                                                           \
			if(get(boost::vertex_index_t(), graph, vTar) > get(boost::vertex_index_t(), graph, v))  \
				return;                                                                               \
		}                                                                                         \
	}                                                                                           \
	g = nullptr;                                                                                \
	vId = 0;                                                                                    \
	eId = 0;                                                                                    \
}                                                                                             \
																																								\
/*----------------------------------------------------------------------------*/\
/* IncidentEdgeIterator                                                       */\
/*----------------------------------------------------------------------------*/\
																							                                  \
Graph::IncidentEdgeIterator::IncidentEdgeIterator(std::shared_ptr<Owner> g, std::size_t vId) : g(g), vId(vId), eId(0) { \
	assert(g);                                                                    \
	const auto &graph = getMacroGraph;                                            \
	using boost::vertices;                                                        \
	auto v = *std::next(vertices(graph).first, vId);                              \
	if(out_degree(v, graph) == 0) {                                               \
		this->g = nullptr;                                                          \
		this->vId = 0;                                                              \
	}                                                                             \
}                                                                               \
																																								\
Graph::IncidentEdgeIterator::IncidentEdgeIterator() : g(nullptr), vId(0), eId(0) { } \
																																								\
Graph::Edge Graph::IncidentEdgeIterator::dereference() const {                  \
	return Graph::Edge(g, vId, eId);                                              \
}                                                                               \
																																								\
bool Graph::IncidentEdgeIterator::equal(const IncidentEdgeIterator &iter) const { \
	if(g) return g == iter.g && vId == iter.vId && eId == iter.eId;               \
	else return g == iter.g;                                                      \
}                                                                               \
																																								\
void Graph::IncidentEdgeIterator::increment() {                                 \
	++this->eId;                                                                  \
	const auto &graph = getMacroGraph;                                            \
	using boost::vertices;                                                        \
	auto v = *std::next(vertices(graph).first, vId);                              \
	if(eId >= out_degree(v, graph)) {                                             \
		this->g = nullptr;                                                          \
		this->vId = 0;                                                              \
	}                                                                             \
}

} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPHPIMPL_H */

