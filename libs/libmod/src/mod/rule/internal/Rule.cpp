#include "Rule.hpp"

#include <mod/lib/Rules/LabelledRule.hpp>
#include <mod/lib/Rules/Real.hpp>

namespace mod::rule::internal {

lib::Rules::LabelledRule makeLabelledRule() {
	return lib::Rules::LabelledRule();
}

lib::Rules::LabelledRule::GraphType &getGraph(lib::Rules::LabelledRule &r) {
	return get_graph(r);
}

std::unique_ptr<lib::Rules::PropString> makePropStringCore(const lib::Rules::LabelledRule &rule) {
	return std::make_unique<lib::Rules::PropString>(rule.getRule());
}

void add(lib::Rules::PropString &pString, lib::DPO::CombinedRule::CombinedVertex v,
         const std::string &valueLeft, const std::string &valueRight) {
	pString.add(v, valueLeft, valueRight);
}

void add(lib::Rules::PropString &pString, lib::DPO::CombinedRule::CombinedEdge e,
         const std::string &valueLeft, const std::string &valueRight) {
	pString.add(e, valueLeft, valueRight);
}

void setRight(lib::Rules::PropString &pString, lib::DPO::CombinedRule::CombinedEdge e, const std::string &value) {
	pString.setRight(e, value);
}

lib::Rules::PropMolecule
makePropMoleculeCore(const lib::Rules::LabelledRule &rule, const lib::Rules::PropString &str) {
	return lib::Rules::PropMolecule(rule.getRule(), str);
}

std::shared_ptr<Rule> makeRule(lib::Rules::LabelledRule &&r) {
	auto rLib = std::make_unique<mod::lib::Rules::Real>(std::move(r), std::nullopt);
	return mod::rule::Rule::makeRule(std::move(rLib));
}

const std::string &getStringLeft(lib::DPO::CombinedRule::CombinedVertex v,
                                 const lib::Rules::PropString &str) {
	return str.getLeft()[v];
}

const std::string &getStringRight(lib::DPO::CombinedRule::CombinedVertex v,
                                  const lib::Rules::PropString &str) {
	return str.getRight()[v];
}

BondType getMoleculeLeft(lib::DPO::CombinedRule::CombinedEdge e,
                         const lib::Rules::PropMolecule &mol) {
	return mol.getLeft()[e];
}

BondType getMoleculeRight(lib::DPO::CombinedRule::CombinedEdge e,
                          const lib::Rules::PropMolecule &mol) {
	return mol.getRight()[e];
}

} // namespace mod::rule::internal