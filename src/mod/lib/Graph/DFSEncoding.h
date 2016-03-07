#ifndef MOD_LIB_GRAPH_DFSENCODING_H
#define	MOD_LIB_GRAPH_DFSENCODING_H

#include <mod/lib/Graph/Base.h>

#include <iosfwd>
#include <string>

namespace mod {
namespace lib {
namespace Graph {
namespace DFSEncoding {

std::pair<std::unique_ptr<GraphType>, std::unique_ptr<PropStringType> > parse(const std::string &dfs, std::ostream &s);
std::pair<std::string, bool> write(const GraphType &g, const PropStringType &labelState);

} // namespace DFSEncoding
} // namespace Graph
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_GRAPH_DFSENCODING_H */
