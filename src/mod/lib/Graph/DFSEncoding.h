#ifndef MOD_LIB_GRAPH_DFSENCODING_H
#define	MOD_LIB_GRAPH_DFSENCODING_H

#include <mod/lib/Graph/GraphDecl.h>
#include <mod/lib/IO/Graph.h>

#include <iosfwd>
#include <string>

namespace mod {
namespace lib {
namespace Graph {
struct PropString;
namespace DFSEncoding {

lib::IO::Graph::Read::Data parse(const std::string &dfs, std::ostream &s);
std::pair<std::string, bool> write(const GraphType &g, const PropString &labelState);

} // namespace DFSEncoding
} // namespace Graph
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_GRAPH_DFSENCODING_H */
