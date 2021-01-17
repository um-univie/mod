#ifndef MOD_RULE_INTERNAL_RULE_HPP
#define MOD_RULE_INTERNAL_RULE_HPP

#include <mod/BuildConfig.hpp>
#include <mod/rule/Rule.hpp>
#include <mod/lib/Rules/LabelledRule.hpp>

namespace mod::rule::internal {

MOD_DECL lib::Rules::LabelledRule makeLabelledRule();
MOD_DECL lib::Rules::GraphType &getGraph(lib::Rules::LabelledRule &r);

MOD_DECL std::unique_ptr<lib::Rules::PropStringCore> makePropStringCore(const lib::Rules::GraphType &g);
MOD_DECL void add(lib::Rules::PropStringCore &pString, boost::graph_traits<lib::Rules::GraphType>::vertex_descriptor v,
                  const std::string &valueLeft, const std::string &valueRight);
MOD_DECL void add(lib::Rules::PropStringCore &pString, boost::graph_traits<lib::Rules::GraphType>::edge_descriptor e,
                  const std::string &valueLeft, const std::string &valueRight);
MOD_DECL void setRight(lib::Rules::PropStringCore &pString,
                       boost::graph_traits<lib::Rules::GraphType>::edge_descriptor e, const std::string &value);
MOD_DECL lib::Rules::PropMoleculeCore
makePropMoleculeCore(const lib::Rules::GraphType &g, const lib::Rules::PropStringCore &str);

MOD_DECL std::shared_ptr<Rule> makeRule(lib::Rules::LabelledRule &&r);

} // namespace mod::rule::internal

#endif // MOD_RULE_INTERNAL_RULE_HPP
