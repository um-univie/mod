#ifndef MOD_LIB_DG_STRATEGIES_DERIVATIONPREDICATES_H
#define	MOD_LIB_DG_STRATEGIES_DERIVATIONPREDICATES_H

#include <mod/dg/Strategies.hpp>
#include <mod/lib/DG/Strategies/Strategy.hpp>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

struct DerivationPredicate : Strategy {
protected:
	DerivationPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > predicate, Strategy *strat);
public:
	virtual ~DerivationPredicate() override;
	virtual void preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>, IsomorphismPolicy)> add) const override;
	virtual void forEachRule(std::function<void(const lib::Rules::Real&)> f) const override;
	virtual void printInfo(PrintSettings settings) const override;
	virtual const GraphState &getOutput() const override;
	virtual bool isConsumed(const lib::Graph::Single *g) const override;
protected:
	virtual void printName(std::ostream &s) const = 0;
	virtual void pushPredicate(std::shared_ptr<mod::Function<bool(const mod::Derivation&)> > pred) = 0;
	virtual void popPredicate() = 0;
private:
	virtual void setExecutionEnvImpl() override;
	virtual void executeImpl(PrintSettings settings, const GraphState &input) override;
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

