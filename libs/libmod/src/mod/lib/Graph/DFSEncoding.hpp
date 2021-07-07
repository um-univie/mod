#ifndef MOD_LIB_GRAPH_DFSENCODING_HPP
#define MOD_LIB_GRAPH_DFSENCODING_HPP

#include <mod/lib/IO/Graph.hpp>
#include <mod/lib/IO/Result.hpp>

#include <string>

namespace mod::lib::Graph {
struct PropString;
} // namespace mod::lib::Graph
namespace mod::lib::Graph::DFSEncoding {

lib::IO::Result<lib::IO::Graph::Read::Data> parse(const std::string &dfs);
std::pair<std::string, bool> write(const GraphType &g, const PropString &pString, bool withIds);

} // namespace mod::lib::Graph::DFSEncoding

#endif // MOD_LIB_GRAPH_DFSENCODING_HPP