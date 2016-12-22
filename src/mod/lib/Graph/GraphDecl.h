#ifndef MOD_LIB_GRAPH_GRAPHDECL_H
#define MOD_LIB_GRAPH_GRAPHDECL_H

#include <jla_boost/graph/EdgeIndexedAdjacencyList.hpp>

namespace mod {
namespace lib {
namespace Graph {

using GraphType = jla_boost::EdgeIndexedAdjacencyList<boost::undirectedS>;
using Vertex = boost::graph_traits<GraphType>::vertex_descriptor;
using Edge = boost::graph_traits<GraphType>::edge_descriptor;

} // namespace Graph
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPH_GRAPHDECL_H */
