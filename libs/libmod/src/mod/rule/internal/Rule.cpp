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

std::unique_ptr<lib::Rules::PropStringCore> makePropStringCore(const lib::Rules::GraphType &g) {
	return std::make_unique<lib::Rules::PropStringCore>(g);
}

void add(lib::Rules::PropStringCore &pString, boost::graph_traits<lib::Rules::GraphType>::vertex_descriptor v,
         const std::string &valueLeft, const std::string &valueRight) {
	pString.add(v, valueLeft, valueRight);
}

void add(lib::Rules::PropStringCore &pString, boost::graph_traits<lib::Rules::GraphType>::edge_descriptor e,
         const std::string &valueLeft, const std::string &valueRight) {
	pString.add(e, valueLeft, valueRight);
}

void setRight(lib::Rules::PropStringCore &pString,
              boost::graph_traits<lib::Rules::GraphType>::edge_descriptor e, const std::string &value) {
	pString.setRight(e, value);
}

MOD_DECL lib::Rules::PropMoleculeCore
makePropMoleculeCore(const lib::Rules::GraphType &g, const lib::Rules::PropStringCore &str) {
	return lib::Rules::PropMoleculeCore(g, str);
}

std::shared_ptr<Rule> makeRule(lib::Rules::LabelledRule &&r) {
	auto rLib = std::make_unique<mod::lib::Rules::Real>(std::move(r), boost::none);
	return mod::rule::Rule::makeRule(std::move(rLib));
}

} // namespace mod::rule::internal