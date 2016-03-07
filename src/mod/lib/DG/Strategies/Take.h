#ifndef MOD_LIB_DG_STRATEGIES_TAKE_H
#define	MOD_LIB_DG_STRATEGIES_TAKE_H

#include <mod/lib/DG/Strategies/Strategy.h>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

struct Take : Strategy {
	Take(unsigned int limit, bool doUniverse);
	~Take();
	Strategy *clone() const;
	void printInfo(std::ostream &s) const;
	bool isConsumed(const Graph::Single *g) const;
private:
	void executeImpl(std::ostream &s, const GraphState &input);
private:
	const unsigned int limit;
	const bool doUniverse;
};

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_DG_STRATEGIES_TAKE_H */