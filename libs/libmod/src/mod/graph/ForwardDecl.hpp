#ifndef MOD_GRAPH_FORWARDDECL_HPP
#define MOD_GRAPH_FORWARDDECL_HPP

namespace mod::graph {
struct Graph;
struct GraphLess;
struct Printer;
} // namespace mod::graph
namespace mod::lib {
template<typename LGraph>
struct LabelledUnionGraph;
} // namespace mod::lib
namespace mod::lib::Graph {
struct LabelledGraph;
struct Single;
} // namespace mod::lib::Graph
namespace mod::lib::IO::Graph::Write {
struct Options;
} // namespace mod::lib::IO::Graph::Write

#endif // MOD_GRAPH_FORWARDDECL_HPP