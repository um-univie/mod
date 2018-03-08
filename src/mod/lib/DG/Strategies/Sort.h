#ifndef MOD_LIB_DG_STRATEGIES_SORT_H
#define MOD_LIB_DG_STRATEGIES_SORT_H

#include <mod/dg/Strategies.h>
#include <mod/lib/DG/Strategies/Strategy.h>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

struct Sort : Strategy {
	Sort(std::shared_ptr<mod::Function<bool(std::shared_ptr<graph::Graph>, std::shared_ptr<graph::Graph>, const dg::Strategy::GraphState&)> > less, bool doUniverse);
	Strategy *clone() const;
	void preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>) > add) const;

	void forEachRule(std::function<void(const lib::Rules::Real&) > f) const { }
	void printInfo(std::ostream &s) const;
	bool isConsumed(const lib::Graph::Single *g) const;
private:
	void executeImpl(std::ostream &s, const GraphState &input);
private:
	std::shared_ptr<mod::Function<bool(std::shared_ptr<graph::Graph>, std::shared_ptr<graph::Graph>, const dg::Strategy::GraphState&)> > less;
	const bool doUniverse;
};

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_DG_STRATEGIES_SORT_H */