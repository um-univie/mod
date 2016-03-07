#ifndef MOD_LIB_DG_STRATEGIES_DERIVATIONPREDICATES_H
#define	MOD_LIB_DG_STRATEGIES_DERIVATIONPREDICATES_H

#include <mod/DGStrat.h>
#include <mod/lib/DG/Strategies/Strategy.h>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

struct DerivationPredicate : Strategy {
protected:
	DerivationPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > predicate, Strategy *strat);
public:
	virtual ~DerivationPredicate();
	void printInfo(std::ostream &s) const;
	const GraphState &getOutput() const;
	bool isConsumed(const lib::Graph::Single *g) const;
protected:
	virtual void printName(std::ostream &s) const = 0;
	virtual void pushPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > pred) = 0;
	virtual void popPredicate() = 0;
private:
	void setExecutionEnvImpl();
	void executeImpl(std::ostream &s, const GraphState &input);
protected:
	std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > predicate;
	Strategy *strat;
};

struct LeftPredicate : DerivationPredicate {
	LeftPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > predicate, Strategy *strat);
	Strategy *clone() const;
private:
	void printName(std::ostream &s) const;
	void pushPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > pred);
	void popPredicate();
};

struct RightPredicate : DerivationPredicate {
	RightPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > predicate, Strategy *strat);
	Strategy *clone() const;
private:
	void printName(std::ostream &s) const;
	void pushPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > pred);
	void popPredicate();
};

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_DG_STRATEGIES_DERIVATIONPREDICATES_H */

