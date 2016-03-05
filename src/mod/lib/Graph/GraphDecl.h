#ifndef MOD_LIB_GRAPH_GRAPHDECL_H
#define MOD_LIB_GRAPH_GRAPHDECL_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

namespace mod {
namespace lib {
namespace Graph {

using GraphType = boost::adjacency_list< boost::vecS, boost::vecS, boost::undirectedS>;
using Vertex = boost::graph_traits<GraphType>::vertex_descriptor;
using Edge = boost::graph_traits<GraphType>::edge_descriptor;

} // namespace Graph
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPH_GRAPHDECL_H */
