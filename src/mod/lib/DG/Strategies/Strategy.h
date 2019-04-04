#ifndef MOD_LIB_DG_STRATEGIES_STRATEGY_H
#define MOD_LIB_DG_STRATEGIES_STRATEGY_H

#include <mod/dg/Strategies.h>
#include <mod/lib/DG/NonHyper.h>

#include <iosfwd>
#include <vector>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {
class GraphState;

struct ExecutionEnv {

	ExecutionEnv(LabelSettings labelSettings) : labelSettings(labelSettings) { }

	virtual ~ExecutionEnv() { };
	virtual bool tryAddGraph(std::shared_ptr<graph::Graph> g) = 0;
	virtual bool addGraph(std::shared_ptr<graph::Graph> g) = 0;
	virtual bool addGraphAsVertex(std::shared_ptr<graph::Graph> g) = 0;
	virtual bool doExit() const = 0;
	// the right side is always empty
	virtual bool checkLeftPredicate(const mod::Derivation &d) const = 0;
	// but here everything is defined
	virtual bool checkRightPredicate(const mod::Derivation &d) const = 0;
	virtual std::shared_ptr<graph::Graph> checkIfNew(std::unique_ptr<lib::Graph::Single> g) const = 0;
	virtual void giveProductStatus(std::shared_ptr<graph::Graph> g) = 0;
	virtual bool addProduct(std::shared_ptr<graph::Graph> g) = 0;
	virtual bool isDerivation(const GraphMultiset &gmsSrc, const GraphMultiset &gmsTar, const lib::Rules::Real *r) const = 0;
	virtual bool suggestDerivation(const GraphMultiset &gmsSrc, const GraphMultiset &gmsTar, const lib::Rules::Real *r) = 0;
	virtual void pushLeftPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > pred) = 0;
	virtual void pushRightPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > pred) = 0;
	virtual void popLeftPredicate() = 0;
	virtual void popRightPredicate() = 0;
public:
	const LabelSettings labelSettings;
};

struct Strategy {
	Strategy(unsigned int maxComponents);
	Strategy(const Strategy&) = delete;
	Strategy &operator=(const Strategy&) = delete;
	virtual ~Strategy();
	virtual Strategy *clone() const = 0;
	void setExecutionEnv(ExecutionEnv &env);
	virtual void preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>) > add) const = 0;
	virtual void forEachRule(std::function<void(const lib::Rules::Real&)> f) const = 0;
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
