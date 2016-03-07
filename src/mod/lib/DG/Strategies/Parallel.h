#ifndef MOD_LIB_DG_STRATEGIES_PARALLEL_H
#define	MOD_LIB_DG_STRATEGIES_PARALLEL_H

#include <mod/lib/DG/Strategies/Strategy.h>

#include <vector>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

struct Parallel : Strategy {
	Parallel(const std::vector<Strategy*> &strats);
	~Parallel();
	Strategy *clone() const;
	void printInfo(std::ostream &s) const;
	bool isConsumed(const lib::Graph::Single *g) const;
private:
	void setExecutionEnvImpl();
	void executeImpl(std::ostream &s, const GraphState &input);
private:
	std::vector<Strategy*> strats;
};

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_DG_STRATEGIES_PARALLEL_H */