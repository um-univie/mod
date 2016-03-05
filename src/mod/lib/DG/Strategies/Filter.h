#ifndef MOD_LIB_DG_STRATEGY_FILTER_H
#define	MOD_LIB_DG_STRATEGY_FILTER_H

#include <mod/DGStrat.h>
#include <mod/lib/DG/Strategies/Strategy.h>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

struct Filter : Strategy {
	Filter(std::shared_ptr<mod::Function<bool(std::shared_ptr<mod::Graph>, const mod::DGStrat::GraphState&, bool)> > filterFunc, bool filterUniverse);
	Strategy *clone() const;
	void printInfo(std::ostream &s) const;
	bool isConsumed(const lib::Graph::Single *g) const;
private:
	void executeImpl(std::ostream &s, const GraphState &input);
private:
	std::shared_ptr<mod::Function<bool(std::shared_ptr<mod::Graph>, const mod::DGStrat::GraphState&, bool)> > filterFunc;
	bool filterUniverse;
};

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_DG_STRATEGY_FILTER_H */

