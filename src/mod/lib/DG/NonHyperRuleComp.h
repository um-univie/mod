#ifndef MOD_LIB_DG_NONHYPERRULECOMP_H
#define	MOD_LIB_DG_NONHYPERRULECOMP_H

#include <mod/dg/Strategies.h>
#include <mod/lib/DG/NonHyper.h>

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
	std::string getType() const;
	void printStrategyInfo(std::ostream &s) const;
	const Strategies::GraphState &getOutput() const;
private:
	void calculateImpl();
	void listImpl(std::ostream &s) const;
private:
	struct ExecutionEnv;
	std::unique_ptr<ExecutionEnv> env;
	std::unique_ptr<Strategies::Strategy> strategy;
	std::unique_ptr<Strategies::GraphState> input;
private: // state for computation
	std::vector<std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > > leftPredicates;
	std::vector<std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > > rightPredicates;
	bool doExit;
};

} // namespace DG
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_DG_NONHYPERRULECOMP_H */