#ifndef MOD_RULE_COMPOSITIONEXPR_H
#define MOD_RULE_COMPOSITIONEXPR_H

#include <mod/BuildConfig.hpp>
#include <mod/graph/ForwardDecl.hpp>
#include <mod/rule/ForwardDecl.hpp>

#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/variant.hpp>

#include <vector>

namespace mod {
namespace rule {

namespace RCExp {
// rst: RCExp
// rst: ---------------------------
// rst:
// rst: In this namespace the data structures and operators for representing rule composition expressions are defined.
// rst: An expression, ``RCExp``, can be evaluated through the method ``Composer::eval``. The result of an expression
// rst: is a set of rules.
// rst:

// Nullary/unary
//------------------------------------------------------------------------------

// rst-class: rule::RCExp::Union
// rst:
// rst:		Return the union of the subexpressions. I.e., flatten the subresult lists into a single list.
// rst:
// rst-class-start:

struct MOD_DECL Union {
	Union(std::vector<Expression> exps);
	MOD_DECL friend std::ostream &operator<<(std::ostream &s, const Union &par);
	const std::vector<Expression> &getExpressions() const;
private:
	std::vector<Expression> exps;
};
// rst-class-end:

// rst-class: rule::RCExp::Bind
// rst:
// rst:		Return the singleton list with the rule :math:`(\emptyset, \emptyset, G)` for the given graph :math:`G`.
// rst:
// rst-class-start:

struct MOD_DECL Bind {
	Bind(std::shared_ptr<graph::Graph> g);
	MOD_DECL friend std::ostream &operator<<(std::ostream &s, const Bind &b);
	std::shared_ptr<graph::Graph> getGraph() const;
private:
	std::shared_ptr<graph::Graph> g;
};
// rst-class-end:

// rst-class: rule::RCExp::Id
// rst:
// rst:		Return the singleton list with the rule :math:`(G, G, G)` for the given graph :math:`G`.
// rst:
// rst-class-start:

struct MOD_DECL Id {
	Id(std::shared_ptr<graph::Graph> g);
	MOD_DECL friend std::ostream &operator<<(std::ostream &s, const Id &i);
	std::shared_ptr<graph::Graph> getGraph() const;
private:
	std::shared_ptr<graph::Graph> g;
};
// rst-class-end:

// rst-class: rule::RCExp::Unbind
// rst:
// rst:		Return the singleton list with the rule :math:`(G, \emptyset, \emptyset)` for the given graph :math:`G`.
// rst:
// rst-class-start:

struct MOD_DECL Unbind {
	Unbind(std::shared_ptr<graph::Graph> g);
	MOD_DECL friend std::ostream &operator<<(std::ostream &s, const Unbind &u);
	std::shared_ptr<graph::Graph> getGraph() const;
private:
	std::shared_ptr<graph::Graph> g;
};
// rst-class-end:

// Expression
//------------------------------------------------------------------------------

class ComposeCommon;

class ComposeParallel;

class ComposeSub;

class ComposeSuper;

// rst-class: rule::RCExp::Expression
// rst:
// rst:		A generic rule composition expression.
// rst:
// rst-class-start:

struct MOD_DECL Expression {
	Expression(std::shared_ptr<Rule> r);
	Expression(Union u);
	Expression(Bind bind);
	Expression(Id id);
	Expression(Unbind unbind);
	Expression(ComposeCommon compose);
	Expression(ComposeParallel compose);
	Expression(ComposeSub compose);
	Expression(ComposeSuper compose);

	template<typename Visitor>
	typename Visitor::result_type applyVisitor(Visitor visitor) const {
		return boost::apply_visitor(visitor, data);
	}

	MOD_DECL friend std::ostream &operator<<(std::ostream &s, const Expression &exp);
private:
	boost::variant<std::shared_ptr<Rule>, Union, Bind, Id, Unbind,
			boost::recursive_wrapper<ComposeCommon>,
			boost::recursive_wrapper<ComposeParallel>,
			boost::recursive_wrapper<ComposeSub>,
			boost::recursive_wrapper<ComposeSuper>
	> data;
};
// rst-class-end:

// Binary
//------------------------------------------------------------------------------


// rst-class: rule::RCExp::ComposeBase
// rst:
// rst:		The base class for the composition of two rule :math:`(L_1, K_1, R_1)` and :math:`(L_2, K_2, R_2)`.
// rst:
// rst-class-start:

class MOD_DECL ComposeBase {
protected:
	ComposeBase(Expression first, Expression second, bool discardNonchemical);
public:
	virtual ~ComposeBase();
	MOD_DECL friend std::ostream &operator<<(std::ostream &s, const ComposeBase &compose);
	const Expression &getFirst() const;
	const Expression &getSecond() const;
	bool getDiscardNonchemical() const;
private:
	virtual std::ostream &print(std::ostream &s) const = 0;
private:
	Expression first, second;
	bool discardNonchemical;
};
// rst-class-end:

// rst-class: rule::RCExp::ComposeCommon
// rst:
// rst:		Compose the rules by all common subgraphs of :math:`R_1` and :math:`L_2`,
// rst:		possibly limited to connected subgraphs or to the subgraphs of maximum size.
// rst:
// rst-class-start:

struct MOD_DECL ComposeCommon : public ComposeBase {
	ComposeCommon(Expression first, Expression second, bool discardNonchemical, bool maximum, bool connected);
	bool getMaxmimum() const;
	bool getConnected() const;
private:
	std::ostream &print(std::ostream &s) const;
private:
	bool maximum, connected;
};
// rst-class-end:

// rst-class: rule::RCExp::ComposeParallel
// rst:
// rst:		Compose the rules by the empty graph, i.e., create a rule representing the parallel application of two input rules.
// rst:
// rst-class-start:

struct MOD_DECL ComposeParallel : public ComposeBase {
	ComposeParallel(Expression first, Expression second, bool discardNonchemical);
private:
	std::ostream &print(std::ostream &s) const;
};
// rst-class-end:

// rst-class: rule::RCExp::ComposeSub
// rst:
// rst:		Compose the rules such that overlapping connected components of :math:`R_1` and :math:`L_2` have the :math:`L_2` component as a subgraph of :math:`R_1`.
// rst:		The overlap is `partial` if not every connected component of :math:`L_2` is participating in the common subgraph.
// rst:
// rst-class-start:

struct MOD_DECL ComposeSub : public ComposeBase {
	ComposeSub(Expression first, Expression second, bool discardNonchemical, bool allowPartial);
	bool getAllowPartial() const;
private:
	std::ostream &print(std::ostream &s) const;
private:
	bool allowPartial;
};
// rst-class-end:

// rst-class: rule::RCExp::ComposeSuper
// rst:
// rst:		Compose the rules such that overlapping connected components of :math:`R_1` and :math:`L_2` have the :math:`R_1` component as a subgraph of :math:`L_2`.
// rst:		The overlap is `partial` if not every connected component of :math:`R_1` is participating in the common subgraph.
// rst:
// rst-class-start:

struct MOD_DECL ComposeSuper : public ComposeBase {
	ComposeSuper(Expression first,
					 Expression second,
					 bool discardNonchemical,
					 bool allowPartial,
					 bool enforceConstraints);
	bool getAllowPartial() const;
	bool getEnforceConstraints() const;
private:
	std::ostream &print(std::ostream &s) const;
private:
	bool allowPartial, enforceConstraints;
};
// rst-class-end:

} // namespace RCExp

} // namespace rule
} // namespace mod

#endif /* MOD_RULE_COMPOSITIONEXPR_H */

