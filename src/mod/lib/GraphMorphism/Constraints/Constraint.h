#ifndef MOD_LIB_GRAPHMORPHISM_CONSTRAINT_H
#define MOD_LIB_GRAPHMORPHISM_CONSTRAINT_H

#include <mod/lib/GraphMorphism/Constraints/Visitor.h>

#include <cassert>
#include <memory>

#include <iostream>

namespace mod {
namespace lib {
namespace GraphMorphism {
namespace Constraints {
// inspired by https://stackoverflow.com/questions/7876168/using-the-visitor-pattern-with-template-derived-classes

template<typename Graph>
struct Constraint {
	virtual ~Constraint() = default;
	virtual void accept(BaseVisitorNonConst<Graph> &v) = 0;
	virtual void accept(BaseVisitor<Graph> &v) const = 0;
	virtual std::unique_ptr<Constraint<Graph> > clone() const = 0;
	virtual std::string name() const = 0;
	virtual bool supportsTerm() const = 0;
protected:

	template<typename C>
	static void acceptDispatch(C &c, BaseVisitorNonConst<Graph> &visitor) {
		dynamic_cast<Visitor<C>&> (visitor) (c);
	}

	template<typename C>
	static void acceptDispatch(C &c, BaseVisitor<Graph> &visitor) {
		dynamic_cast<Visitor<C>&> (visitor) (c);
	}

#define MOD_VISITABLE()                                                         \
	virtual void accept(BaseVisitorNonConst<Graph> &v) override {                 \
		Constraint<Graph>::acceptDispatch(*this, v);                                \
	}                                                                             \
	virtual void accept(BaseVisitor<Graph> &v) const override {                   \
		Constraint<Graph>::acceptDispatch(*this, v);                                \
	}
};

enum class Operator {
	EQ, LT, GT, LEQ, GEQ
};

} // namespace Constraints
} // namespace GraphMorphism
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPHMORPHISM_CONSTRAINT_H */