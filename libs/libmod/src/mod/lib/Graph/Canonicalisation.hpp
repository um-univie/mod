#ifndef MOD_LIB_GRAPH_CANONICALISATION_HPP
#define MOD_LIB_GRAPH_CANONICALISATION_HPP

#include <mod/lib/Graph/Single.hpp>

namespace mod::lib::Graph {

std::tuple<std::vector<int>, std::unique_ptr<Single::CanonForm>, std::unique_ptr<Single::AutGroup>>
getCanonForm(const Single &g, LabelType labelType, bool withStereo);

bool canonicalCompare(const Single &g1, const Single &g2, LabelType labelType, bool withStereo);

} // namespace mod::lib::Graph

#endif // MOD_LIB_GRAPH_CANONICALISATION_HPP