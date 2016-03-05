#ifndef MOD_LIB_DG_STRATEGIES_EXECUTE_H
#define	MOD_LIB_DG_STRATEGIES_EXECUTE_H

#include <mod/DGStrat.h>
#include <mod/lib/DG/Strategies/Strategy.h>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

struct Execute : Strategy {
	Execute(std::shared_ptr<mod::Function<void(const mod::DGStrat::GraphState&)> > func);
	Strategy *clone() const;
	void printInfo(std::ostream &s) const;
	const GraphState &getOutput() const;
	bool isConsumed(const Graph::Single *g) const;
private:
	void executeImpl(std::ostream &s, const GraphState &input);
private:
	std::shared_ptr<mod::Function<void(const mod::DGStrat::GraphState&)> > func;
};

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_DG_STRATEGIES_EXECUTE_H */

