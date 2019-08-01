#ifndef MOD_LIB_DG_STRATEGY_FILTER_H
#define MOD_LIB_DG_STRATEGY_FILTER_H

#include <mod/dg/Strategies.hpp>
#include <mod/lib/DG/Strategies/Strategy.hpp>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

struct Filter : Strategy {
	Filter(std::shared_ptr<mod::Function<bool(std::shared_ptr<graph::Graph>, const dg::Strategy::GraphState&, bool)> > filterFunc, bool filterUniverse);
	Strategy *clone() const;
	void preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>) > add) const;

	void forEachRule(std::function<void(const lib::Rules::Real&) > f) const { }
	void printInfo(std::ostream &s) const;
	bool isConsumed(const lib::Graph::Single *g) const;
private:
	void executeImpl(std::ostream &s, const GraphState &input);
private:
	std::shared_ptr<mod::Function<bool(std::shared_ptr<graph::Graph>, const dg::Strategy::GraphState&, bool)> > filterFunc;
	bool filterUniverse;
};

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_DG_STRATEGY_FILTER_H */

