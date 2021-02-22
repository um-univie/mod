#ifndef MOD_LIB_RULES_APPLICATION_COMPONENTMATCHDB_UTIL_HPP
#define MOD_LIB_RULES_APPLICATION_COMPONENTMATCHDB_UTIL_HPP

#include <mod/lib/Graph/LabelledGraph.hpp>
#include <mod/lib/Rules/LabelledRule.hpp>
#include <mod/lib/Rules/Application/ComponentMatch.hpp>
#include <mod/Config.hpp>

namespace mod::lib::Rules::Application::ComponentMatchDB {

bool isMonomorphic(const LabelledRule& rule, size_t componentIndex,
                   const Graph::LabelledGraph& host,
                   const LabelSettings& ls);

std::vector<ComponentMatch::Morphism> enumerateMonomorphisms(const LabelledRule& rule,
                                                          size_t componentIndex,
                                                          const Graph::LabelledGraph& host,
                                                          const LabelSettings& ls);

}

#endif
