#ifndef MOD_LIB_DG_STRATEGIES_TAKE_H
#define MOD_LIB_DG_STRATEGIES_TAKE_H

#include <mod/lib/DG/Strategies/Strategy.hpp>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

struct Take : Strategy {
	Take(unsigned int limit, bool doUniverse);
	virtual ~Take() override;
	virtual Strategy *clone() const override;
	virtual void preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>, IsomorphismPolicy)> add) const override;
	virtual void forEachRule(std::function<void(const lib::Rules::Real &)> f) const override {}
	virtual void printInfo(PrintSettings settings) const override;
	virtual bool isConsumed(const Graph::Single *g) const override;
private:
	virtual void executeImpl(PrintSettings settings, const GraphState &input) override;
private:
	const unsigned int limit;
	const bool doUniverse;
};

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_DG_STRATEGIES_TAKE_H */