#ifndef MOD_LIB_DG_STRATEGIES_REPEAT_H
#define   MOD_LIB_DG_STRATEGIES_REPEAT_H

#include <mod/lib/DG/Strategies/Strategy.hpp>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

struct Repeat : Strategy {
	// pre: limit > 0
	Repeat(Strategy *strat, int limit);
	virtual ~Repeat() override;
	virtual Strategy *clone() const override;
	virtual void preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>, IsomorphismPolicy)> add) const override;
	virtual void forEachRule(std::function<void(const lib::Rules::Real &)> f) const override;
	virtual void printInfo(PrintSettings settings) const override;
	virtual const GraphState &getOutput() const override;
	virtual bool isConsumed(const lib::Graph::Single *g) const override;
private:
	virtual void setExecutionEnvImpl() override;
	virtual void executeImpl(PrintSettings settings, const GraphState &input) override;
private:
	Strategy *strat;
	int limit;
	std::vector<Strategy *> subStrats;
};

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif   /* MOD_LIB_DG_STRATEGIES_REPEAT_H */