#ifndef MOD_LIB_DG_STRATEGIES_RULE_HPP
#define MOD_LIB_DG_STRATEGIES_RULE_HPP

#include <mod/lib/DG/Strategies/Strategy.hpp>

#include <unordered_set>

namespace mod::lib::DG::Strategies {

struct Rule : Strategy {
	Rule(std::shared_ptr<rule::Rule> r);
	Rule(const lib::Rules::Real *r);
	virtual std::unique_ptr<Strategy> clone() const override;
	virtual void preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>, IsomorphismPolicy)> add) const override;
	virtual void forEachRule(std::function<void(const lib::Rules::Real &)> f) const override;
	virtual void printInfo(PrintSettings settings) const override;
	virtual bool isConsumed(const lib::Graph::Single *g) const override;
private:
	virtual void executeImpl(PrintSettings settings, const GraphState &input) override;
private:
	std::shared_ptr<rule::Rule> r;
	const lib::Rules::Real *rRaw;
	std::unordered_set<const lib::Graph::Single *> consumedGraphs; // all those from lhs of derivations
};

} // namespace mod::lib::DG::Strategies

#endif // MOD_LIB_DG_STRATEGIES_RULE_HPP