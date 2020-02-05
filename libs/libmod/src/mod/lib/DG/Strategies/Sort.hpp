#ifndef MOD_LIB_DG_STRATEGIES_SORT_H
#define MOD_LIB_DG_STRATEGIES_SORT_H

#include <mod/dg/Strategies.hpp>
#include <mod/lib/DG/Strategies/Strategy.hpp>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

struct Sort : Strategy {
	Sort(std::shared_ptr<mod::Function<bool(std::shared_ptr<graph::Graph>, std::shared_ptr<graph::Graph>, const dg::Strategy::GraphState&)> > less, bool doUniverse);
	virtual Strategy *clone() const override;
	virtual void preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>, IsomorphismPolicy)> add) const override;
	virtual void forEachRule(std::function<void(const lib::Rules::Real&) > f) const override { }
	virtual void printInfo(PrintSettings settings) const override;
	virtual bool isConsumed(const lib::Graph::Single *g) const override;
private:
	virtual void executeImpl(PrintSettings settings, const GraphState &input) override;
private:
	std::shared_ptr<mod::Function<bool(std::shared_ptr<graph::Graph>, std::shared_ptr<graph::Graph>, const dg::Strategy::GraphState&)> > less;
	const bool doUniverse;
};

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_DG_STRATEGIES_SORT_H */