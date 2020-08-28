#ifndef MOD_LIB_DG_STRATEGIES_PARALLEL_H
#define	MOD_LIB_DG_STRATEGIES_PARALLEL_H

#include <mod/lib/DG/Strategies/Strategy.hpp>

#include <vector>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

struct Parallel : Strategy {
	// pre: !strats.empty()
	Parallel(const std::vector<Strategy*> &strats);
	virtual ~Parallel() override;
	virtual Strategy *clone() const override;
	virtual void preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>, IsomorphismPolicy)> add) const override;
	virtual void forEachRule(std::function<void(const lib::Rules::Real&)> f) const override;
	virtual void printInfo(PrintSettings settings) const override;
	virtual bool isConsumed(const lib::Graph::Single *g) const override;
private:
	virtual void setExecutionEnvImpl() override;
	virtual void executeImpl(PrintSettings settings, const GraphState &input) override;
private:
	std::vector<Strategy*> strats;
};

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_DG_STRATEGIES_PARALLEL_H */