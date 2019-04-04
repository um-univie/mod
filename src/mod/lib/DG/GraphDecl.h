#ifndef MOD_LIB_DG_GRAPHDECL_H
#define MOD_LIB_DG_GRAPHDECL_H

#include <mod/lib/Graph/Multiset.h>

#include <boost/graph/adjacency_list.hpp>

namespace mod {
namespace lib {
namespace Graph {
struct Single;
} // namespace Graph
namespace Rules {
struct Real;
} // namespace Rules
namespace DG {
// NonHyper
struct NonHyperVProp;
struct NonHyperEProp;
using NonHyperGraphType = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, NonHyperVProp, NonHyperEProp>;
using NonHyperVertex = boost::graph_traits<NonHyperGraphType>::vertex_descriptor;
using NonHyperEdge = boost::graph_traits<NonHyperGraphType>::edge_descriptor;
using GraphMultiset = lib::Graph::Multiset<const lib::Graph::Single>;

struct NonHyperVProp {
	NonHyperVProp() = default;
public:
	GraphMultiset graphs;
};

struct NonHyperEProp {
	std::vector<const lib::Rules::Real*> rules;
	boost::optional<NonHyperEdge> reverse;
};

// Hyper
struct HyperVProp;
using HyperGraphType = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, HyperVProp>;
using HyperVertex = boost::graph_traits<HyperGraphType>::vertex_descriptor;
using HyperEdge = boost::graph_traits<HyperGraphType>::edge_descriptor;

enum class HyperVertexKind {
	Vertex, Edge
};

struct HyperVProp {
	HyperVertexKind kind;
	const lib::Graph::Single *graph; // != nullptr <=> the vertex is a hyper vertex
	std::vector<const lib::Rules::Real*> rules; // not empty => the vertex is a hyper edge, but it may be empty otherwise
	NonHyperVertex inVertex, outVertex; // only defined for kind == Edge
	HyperVertex reverse; // == null_vertex() <=> no reverse, only defined for kind == Edge
};

} // namespace DG
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_DG_GRAPHDECL_H */