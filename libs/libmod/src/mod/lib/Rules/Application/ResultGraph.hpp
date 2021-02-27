#ifndef MOD_LIB_RULES_APPLICATION_RESULT_GRAPH_HPP
#define MOD_LIB_RULES_APPLICATION_RESULT_GRAPH_HPP

#include <mod/lib/Rules/LabelledRule.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/LabelledUnionGraph.hpp>
#include <jla_boost/graph/morphism/models/Vector.hpp>
#include <mod/lib/Graph/Properties/String.hpp>
#include <mod/lib/Graph/Properties/Stereo.hpp>

#include <iostream>

namespace mod::lib::Rules::Application {


std::vector<std::unique_ptr<Graph::Single>> applyMatch(const LabelledRule& rule,
                                             const LabelledUnionGraph<Graph::LabelledGraph>& host,
                                             const jla_boost::GraphMorphism::InvertibleVectorVertexMap<Rules::LabelledRule::LeftGraphType, LabelledUnionGraph<Graph::LabelledGraph>::GraphType>&  morphism);

}

#endif
