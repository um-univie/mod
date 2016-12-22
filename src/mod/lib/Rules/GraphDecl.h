#ifndef MOD_LIB_RULES_GRAPHDECL_H
#define MOD_LIB_RULES_GRAPHDECL_H

#include <jla_boost/graph/dpo/Rule.hpp>
#include <jla_boost/graph/EdgeIndexedAdjacencyList.hpp>

namespace mod {
namespace lib {
namespace Rules {

using jla_boost::GraphDPO::Membership;

struct VProp {
	Membership membership;
};

struct EProp {
	Membership membership;
};

using GraphType = jla_boost::EdgeIndexedAdjacencyList<boost::undirectedS, VProp, EProp>;
using Vertex = boost::graph_traits<GraphType>::vertex_descriptor;
using Edge = boost::graph_traits<GraphType>::edge_descriptor;

struct MembershipPropertyMap {

	MembershipPropertyMap(const GraphType &g) : g(g) { }
public:
	const GraphType &g;
};

inline MembershipPropertyMap makeMembershipPropertyMap(const GraphType &g) {
	return MembershipPropertyMap(g);
}

inline Membership get(MembershipPropertyMap m, Vertex v) {
	return get(&VProp::membership, m.g, v);
}

inline Membership get(MembershipPropertyMap m, Edge e) {
	return get(&EProp::membership, m.g, e);
}

} // namespace Rules
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RULES_GRAPHDECL_H */