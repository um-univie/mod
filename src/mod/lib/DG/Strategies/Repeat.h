#ifndef MOD_LIB_DG_STRATEGIES_REPEAT_H
#define	MOD_LIB_DG_STRATEGIES_REPEAT_H

#include <mod/lib/DG/Strategies/Strategy.h>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

struct Repeat : Strategy {
	Repeat(Strategy *strat, std::size_t limit);
	~Repeat();
	Strategy *clone() const;
	void printInfo(std::ostream &s) const;
	const GraphState &getOutput() const;
	bool isConsumed(const lib::Graph::Single *g) const;
private:
	void setExecutionEnvImpl();
	void executeImpl(std::ostream &s, const GraphState &input);
private:
	Strategy *strat;
	std::size_t limit;
	std::vector<Strategy*> subStrats;
};

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_DG_STRATEGIES_REPEAT_H */