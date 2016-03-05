#ifndef MOD_LIB_DG_GRAPHDECL_H
#define	MOD_LIB_DG_GRAPHDECL_H

#include <boost/graph/adjacency_list.hpp>

namespace mod {
namespace lib {
namespace Graph {
struct Base;
struct Single;
} // namespace Graph
namespace Rule {
struct Base;
} // namespace Rule
namespace DG {
// NonHyper
struct NonHyperVProp;
struct NonHyperEProp;
using NonHyperGraphType = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, NonHyperVProp, NonHyperEProp>;
using NonHyperVertex = boost::graph_traits<NonHyperGraphType>::vertex_descriptor;
using NonHyperEdge = boost::graph_traits<NonHyperGraphType>::edge_descriptor;

struct NonHyperVProp {

	NonHyperVProp() : graph(nullptr) { }
	const lib::Graph::Base *graph;
};

struct NonHyperEProp {
	std::vector<const lib::Rule::Base*> rules;
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
	std::vector<const lib::Rule::Base*> rules; // not empty <=> the vertex is a hyper edge
	NonHyperVertex inVertex, outVertex; // only defined for kind == RuleKind
	HyperVertex reverse; // == null_vertex() <=> no reverse, only defined for kind == RuleKind
};

} // namespace DG
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_DG_GRAPHDECL_H */