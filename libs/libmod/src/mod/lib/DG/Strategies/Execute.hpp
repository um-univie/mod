#ifndef MOD_LIB_DG_STRATEGIES_EXECUTE_H
#define MOD_LIB_DG_STRATEGIES_EXECUTE_H

#include <mod/dg/Strategies.hpp>
#include <mod/lib/DG/Strategies/Strategy.hpp>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

struct Execute : Strategy {
	Execute(std::shared_ptr<mod::Function<void(const dg::Strategy::GraphState&)> > func);
	Strategy *clone() const;
	void preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>) > add) const;

	void forEachRule(std::function<void(const lib::Rules::Real&) > f) const { }
	void printInfo(std::ostream &s) const;
	const GraphState &getOutput() const;
	bool isConsumed(const Graph::Single *g) const;
private:
	void executeImpl(std::ostream &s, const GraphState &input);
private:
	std::shared_ptr<mod::Function<void(const dg::Strategy::GraphState&)> > func;
};

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_DG_STRATEGIES_EXECUTE_H */

