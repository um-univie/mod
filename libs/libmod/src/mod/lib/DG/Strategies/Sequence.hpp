#ifndef MOD_LIB_DG_STRATEGIES_SEQUENCE_H
#define	MOD_LIB_DG_STRATEGIES_SEQUENCE_H

#include <mod/lib/DG/Strategies/Strategy.hpp>

#include <vector>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

struct Sequence : Strategy {
	Sequence(const std::vector<Strategy*> &strats);
	~Sequence();
	Strategy *clone() const;
	void preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>) > add) const;
	void forEachRule(std::function<void(const lib::Rules::Real&)> f) const;
	void printInfo(std::ostream &s) const;
	const GraphState &getOutput() const;
	bool isConsumed(const lib::Graph::Single *g) const;
private:
	void setExecutionEnvImpl();
	void executeImpl(std::ostream &s, const GraphState &input);
private:
	std::vector<Strategy*> strats;
};

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_DG_STRATEGIES_SEQUENCE_H */

