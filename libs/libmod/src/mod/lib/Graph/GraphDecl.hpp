#ifndef MOD_LIB_GRAPH_GRAPHDECL_HPP
#define MOD_LIB_GRAPH_GRAPHDECL_HPP

#include <jla_boost/graph/EdgeIndexedAdjacencyList.hpp>

namespace mod::lib::Graph {

using GraphType = jla_boost::EdgeIndexedAdjacencyList<boost::undirectedS>;
using Vertex = boost::graph_traits<GraphType>::vertex_descriptor;
using Edge = boost::graph_traits<GraphType>::edge_descriptor;

} // namespace mod::lib::Graph

#endif // MOD_LIB_GRAPH_GRAPHDECL_HPP