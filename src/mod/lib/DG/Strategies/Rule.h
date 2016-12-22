#ifndef MOD_LIB_DG_STRATEGIES_RULE_H
#define	MOD_LIB_DG_STRATEGIES_RULE_H

#include <mod/lib/DG/Strategies/Strategy.h>



#include <unordered_set>

namespace mod {
class Rule;
namespace lib {
namespace Rules {
class Real;
} // namespace Rules
namespace DG {
namespace Strategies {

struct Rule : Strategy {
	Rule(std::shared_ptr<mod::Rule> r);
	Rule(const lib::Rules::Real *r);
	Strategy *clone() const override;
	void printInfo(std::ostream &s) const override;
	bool isConsumed(const lib::Graph::Single *g) const override;
private:
	void executeImpl(std::ostream &s, const GraphState &input) override;
private:
	std::shared_ptr<mod::Rule> r;
	const lib::Rules::Real *rRaw;
	std::unordered_set<const lib::Graph::Single*> consumedGraphs; // all those from lhs of derivations
};

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_DG_STRATEGIES_RULE_H */