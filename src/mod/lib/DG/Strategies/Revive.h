#ifndef MOD_LIB_DG_STRATEGIES_REVIVE_H
#define	MOD_LIB_DG_STRATEGIES_REVIVE_H

#include <mod/lib/DG/Strategies/Strategy.h>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

struct Revive : Strategy {
	Revive(Strategy *strat);
	~Revive();
	Strategy *clone() const;
	void printInfo(std::ostream &s) const;
	bool isConsumed(const lib::Graph::Single *g) const;
private:
	void setExecutionEnvImpl();
	void executeImpl(std::ostream &s, const GraphState &input);
private:
	Strategy *strat;
	std::vector<const lib::Graph::Single*> revivedGraphs;
};

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_DG_STRATEGIES_REVIVE_H */

