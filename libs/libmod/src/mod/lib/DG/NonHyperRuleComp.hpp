#ifndef MOD_LIB_DG_NONHYPERRULECOMP_H
#define MOD_LIB_DG_NONHYPERRULECOMP_H

#include <mod/dg/Strategies.hpp>
#include <mod/lib/DG/NonHyper.hpp>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {
class GraphState;
class Add;
class Execute;
class Filter;
class LeftPredicate;
class RightPredicate;
class Rule;
} // namespace Strategies

struct NonHyperRuleComp : public NonHyper {
	NonHyperRuleComp(const std::vector<std::shared_ptr<graph::Graph> > &graphDatabase,
			Strategies::Strategy *strategy, LabelSettings labelSettings, bool ignoreRuleLabelTypes);
	~NonHyperRuleComp();
	virtual std::string getType() const override;
	void printStrategyInfo(std::ostream &s) const;
	const Strategies::GraphState &getOutput() const;
private:
	virtual void calculateImpl(bool printInfo) override;
	virtual void listImpl(std::ostream &s) const override;
private:
	struct ExecutionEnv;
	std::unique_ptr<ExecutionEnv> env;
	std::unique_ptr<Strategies::Strategy> strategy;
	std::unique_ptr<Strategies::GraphState> input;
};

} // namespace DG
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_DG_NONHYPERRULECOMP_H */