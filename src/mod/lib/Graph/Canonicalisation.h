#ifndef MOD_LIB_GRAPH_CANONICALISATION_H
#define MOD_LIB_GRAPH_CANONICALISATION_H

#include <mod/lib/Graph/Single.h>

namespace mod {
namespace lib {
namespace Graph {

std::tuple<std::vector<int>, std::unique_ptr<Single::CanonForm>, std::unique_ptr<Single::AutGroup> >
getCanonForm(const Single &g, LabelType labelType, bool withStereo);

bool canonicalCompare(const Single &g1, const Single &g2, LabelType labelType, bool withStereo);

} // namespace Graph
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPH_CANONICALISATION_H */