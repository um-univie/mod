#ifndef MOD_LIB_GRAPH_DFSENCODING_H
#define	MOD_LIB_GRAPH_DFSENCODING_H

#include <mod/lib/IO/Graph.hpp>

#include <iosfwd>
#include <string>

namespace mod {
namespace lib {
namespace Graph {
struct PropString;
namespace DFSEncoding {

lib::IO::Graph::Read::Data parse(const std::string &dfs, std::ostream &s);
std::pair<std::string, bool> write(const GraphType &g, const PropString &pString, bool withIds);

} // namespace DFSEncoding
} // namespace Graph
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_GRAPH_DFSENCODING_H */
