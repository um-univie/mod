#ifndef MOD_LIB_DG_STRATEGIES_STRATEGY_H
#define MOD_LIB_DG_STRATEGIES_STRATEGY_H

#include <mod/DGStrat.h>

#include <mod/lib/DG/NonHyper.h>
#include <mod/lib/Graph/Base.h>

#include <iosfwd>
#include <vector>

namespace mod {
class Graph;
class Rule;
namespace lib {
namespace Graph {
class Base;
class Merge;
class Single;
} // namespace Graph
namespace Rule {
class Base;
} // namespace Rule
namespace DG {
namespace Strategies {
class GraphState;

struct ExecutionEnv {

	virtual ~ExecutionEnv() { };
	virtual bool addGraph(std::shared_ptr<mod::Graph> g) = 0;
	virtual bool addGraphAsVertex(std::shared_ptr<mod::Graph> g) = 0;
	virtual bool doExit() const = 0;
	// the right side is always empty
	virtual bool checkLeftPredicate(const mod::Derivation &d) const = 0;
	// but here everything is defined
	virtual bool checkRightPredicate(const mod::Derivation &d) const = 0;
	virtual std::shared_ptr<mod::Graph> checkIfNew(std::unique_ptr<lib::Graph::GraphType> g, std::unique_ptr<lib::Graph::PropStringType> pString) const = 0;
	virtual void giveProductStatus(std::shared_ptr<mod::Graph> g) = 0;
	virtual bool addProduct(std::shared_ptr<mod::Graph> g) = 0;
	virtual const lib::Graph::Merge * addToMergeStore(const lib::Graph::Merge *g) = 0;
	virtual bool isDerivation(const lib::Graph::Base *left, const lib::Graph::Base *right, const lib::Rule::Base *r) const = 0;
	virtual bool suggestDerivation(const lib::Graph::Base *left, const lib::Graph::Base *right, const lib::Rule::Base *r) = 0;
	virtual void pushLeftPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > pred) = 0;
	virtual void pushRightPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > pred) = 0;
	virtual void popLeftPredicate() = 0;
	virtual void popRightPredicate() = 0;
public:
	virtual void fillDerivationRefs(std::vector<mod::DerivationRef> &refs) const = 0;
};

struct Strategy {
	Strategy(unsigned int maxComponents);
	Strategy(const Strategy&) = delete;
	Strategy &operator=(const Strategy&) = delete;
	virtual ~Strategy();
	virtual Strategy *clone() const = 0;
	void setExecutionEnv(ExecutionEnv &env);
	unsigned int getMaxComponents() const;
	void execute(std::ostream &s, const GraphState &input);
	virtual void printInfo(std::ostream &s) const = 0;
	virtual const GraphState &getOutput() const;
	virtual bool isConsumed(const lib::Graph::Single *g) const = 0;
protected:
	ExecutionEnv &getExecutionEnv();
	void printBaseInfo(std::ostream &s) const;
private:
	virtual void setExecutionEnvImpl();
	virtual void executeImpl(std::ostream &s, const GraphState &input) = 0;
private:
	ExecutionEnv *env;
	const unsigned int maxComponents;
protected:
	const GraphState *input;
	GraphState *output;
protected:
	static unsigned int calcMaxNumComponents(const std::vector<Strategy*>& strats);
};

extern unsigned int indentLevel;
std::ostream &indent(std::ostream &s);

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_DG_STRATEGIES_STRATEGY_H */
