#ifndef MOD_LIB_DG_STRATEGIES_RULE_H
#define	MOD_LIB_DG_STRATEGIES_RULE_H

#include <mod/lib/DG/Strategies/Strategy.hpp>

#include <unordered_set>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

struct Rule : Strategy {
	Rule(std::shared_ptr<rule::Rule> r);
	Rule(const lib::Rules::Real *r);
	virtual Strategy *clone() const override;
	virtual void preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>) > add) const override;
	virtual void forEachRule(std::function<void(const lib::Rules::Real&)> f) const override;
	virtual void printInfo(std::ostream &s) const override;
	virtual bool isConsumed(const lib::Graph::Single *g) const override;
private:
	virtual void executeImpl(std::ostream &s, const GraphState &input) override;
private:
	std::shared_ptr<rule::Rule> r;
	const lib::Rules::Real *rRaw;
	std::unordered_set<const lib::Graph::Single*> consumedGraphs; // all those from lhs of derivations
};

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_DG_STRATEGIES_RULE_H */