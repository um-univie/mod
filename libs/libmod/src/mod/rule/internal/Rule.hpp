#ifndef MOD_RULE_INTERNAL_RULE_HPP
#define MOD_RULE_INTERNAL_RULE_HPP

#include <mod/BuildConfig.hpp>
#include <mod/rule/Rule.hpp>
#include <mod/lib/Rules/LabelledRule.hpp>

namespace mod::rule::internal {

MOD_DECL lib::Rules::LabelledRule makeLabelledRule();
MOD_DECL lib::Rules::GraphType &getGraph(lib::Rules::LabelledRule &r);

MOD_DECL std::unique_ptr<lib::Rules::PropString> makePropStringCore(const lib::Rules::LabelledRule &rule);
MOD_DECL void add(lib::Rules::PropString &pString, lib::DPO::CombinedRule::CombinedVertex v,
                  const std::string &valueLeft, const std::string &valueRight);
MOD_DECL void add(lib::Rules::PropString &pString, lib::DPO::CombinedRule::CombinedEdge e,
                  const std::string &valueLeft, const std::string &valueRight);
MOD_DECL void setRight(lib::Rules::PropString &pString, lib::DPO::CombinedRule::CombinedEdge e,
                       const std::string &value);
MOD_DECL lib::Rules::PropMolecule makePropMoleculeCore(const lib::Rules::LabelledRule &rule,
                                                       const lib::Rules::PropString &str);

MOD_DECL std::shared_ptr<Rule> makeRule(lib::Rules::LabelledRule &&r);

MOD_DECL const std::string &getStringLeft(lib::DPO::CombinedRule::CombinedVertex v,
                                          const lib::Rules::PropString &str);
MOD_DECL const std::string &getStringRight(lib::DPO::CombinedRule::CombinedVertex v,
                                           const lib::Rules::PropString &str);
MOD_DECL BondType getMoleculeLeft(lib::DPO::CombinedRule::CombinedEdge e,
                                  const lib::Rules::PropMolecule &mol);
MOD_DECL BondType getMoleculeRight(lib::DPO::CombinedRule::CombinedEdge e,
                                   const lib::Rules::PropMolecule &mol);

} // namespace mod::rule::internal

#endif // MOD_RULE_INTERNAL_RULE_HPP
