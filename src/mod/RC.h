#ifndef MOD_RC_H
#define MOD_RC_H

#include <mod/Config.h>

#include <boost/mpl/bool.hpp> // TODO: remove when Boost 1.61 is required
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/variant.hpp>

#include <memory>
#include <unordered_set>
#include <vector>

namespace mod {
class Graph;
class Rule;
namespace RCExp {
class Expression;
} // namespace RCEcp

// rst: This file contains the core functionality for composing rules and creating rules from graphs.

// rst-class: RCEvaluator
// rst:
// rst:		This class can evaluate rule composition expressions. During evaluation an expression graph is recorded.
// rst: 
// rst:		The expression graph is a directed hypergraph :math:`(V, E)`, with each vertex representing a rule.
// rst:		Each edge represent all compositions calculated for a unique input. That is every edge :math:`e\in E` is on the form
// rst:		:math:`((u, v), R_e)` with :math:`(u, v)\in V\times V` as an *ordered* pair of rules and :math:`R_e\subseteq V` is the set
// rst:		of all resulting rules found.
// rst:
// rst:		The graph is visualised as a bipartite graph with point-shaped vertices representing the hyperedges. The in-edges to these
// rst:		hyperedge vertices are labelled with 1 and 2.
// rst:
// rst-class-start:

class RCEvaluator {
	RCEvaluator(const std::unordered_set<std::shared_ptr<Rule> > &database);
	RCEvaluator(const RCEvaluator&) = delete;
	RCEvaluator &operator=(const RCEvaluator&) = delete;
	RCEvaluator(RCEvaluator&&) = delete;
	RCEvaluator &operator=(RCEvaluator&&) = delete;
public:
	~RCEvaluator();
	// rst: .. function:: const std::unordered_set<std::shared_ptr<Rule> > &getRuleDatabase() const
	// rst: 
	// rst:		:returns: the set of unique rules known by the evaluator.
	const std::unordered_set<std::shared_ptr<Rule> > &getRuleDatabase() const;
	// rst: .. function:: const std::unordered_set<std::shared_ptr<Rule> > &getProducts() const
	// rst:
	// rst:		:returns: the set of unique rules this evaluator has constructed.
	const std::unordered_set<std::shared_ptr<Rule> > &getProducts() const;
	// rst: .. function:: std::unordered_set<std::shared_ptr<Rule> > eval(const RCExp::Expression &exp)
	// rst:
	// rst:		Evaluates a rule composition expression. Any created rule is replaced by a rule in the database if they are isomorphic.
	// rst:
	// rst:		:returns: the result of the expression.
	std::unordered_set<std::shared_ptr<Rule> > eval(const RCExp::Expression &exp);
	// rst: .. function:: void print() const
	// rst:
	// rst:		Print the graph representing all expressions evaluated so far.
	void print() const;
private:
	struct Pimpl;
	std::unique_ptr<Pimpl> p;
public:
	// rst: .. function:: static std::shared_ptr<RCEvaluator> create(const std::unordered_set<std::shared_ptr<Rule> > &database)
	// rst: 
	// rst:		:returns: a new instance of an evaluator with its database initialised with the given rules.
	// rst: 
	// rst:		.. note:: The caller is responsible for ensuring the given rules are unique.
	static std::shared_ptr<RCEvaluator> create(const std::unordered_set<std::shared_ptr<Rule> > &database);
};
// rst-class-end:

namespace RCExp {
// rst: RCExp
// rst: ---------------------------
// rst:
// rst: In this namespace the data structures and operators for representing rule composition expressions are defined.
// rst: An expression, ``RCExp``, can be evaluated through the method ``RCEvaluator::eval``. The result of an expression
// rst: is a set of rules.
// rst:
class Expression;

// Nullary/unary
//------------------------------------------------------------------------------

// rst-class: RCExp::Union
// rst:
// rst:		Return the union of the subexpressions. I.e., flatten the subresult lists into a single list.
// rst:
// rst-class-start:

struct Union {
	Union(std::vector<Expression> exps);
	friend std::ostream &operator<<(std::ostream &s, const Union &par);
	const std::vector<Expression> &getExpressions() const;
private:
	std::vector<Expression> exps;
};
// rst-class-end:

// rst-class: RCExp::Bind
// rst:
// rst:		Return the singleton list with the rule :math:`(\emptyset, \emptyset, G)` for the given graph :math:`G`.
// rst:
// rst-class-start:

struct Bind {
	Bind(std::shared_ptr<Graph> g);
	friend std::ostream &operator<<(std::ostream &s, const Bind &b);
	std::shared_ptr<Graph> getGraph() const;
private:
	std::shared_ptr<Graph> g;
};
// rst-class-end:

// rst-class: RCExp::Id
// rst:
// rst:		Return the singleton list with the rule :math:`(G, G, G)` for the given graph :math:`G`.
// rst:
// rst-class-start:

struct Id {
	Id(std::shared_ptr<Graph> g);
	friend std::ostream &operator<<(std::ostream &s, const Id &i);
	std::shared_ptr<Graph> getGraph() const;
private:
	std::shared_ptr<Graph> g;
};
// rst-class-end:

// rst-class: RCExp::Unbind
// rst:
// rst:		Return the singleton list with the rule :math:`(G, \emptyset, \emptyset)` for the given graph :math:`G`.
// rst:
// rst-class-start:

struct Unbind {
	Unbind(std::shared_ptr<Graph> g);
	friend std::ostream &operator<<(std::ostream &s, const Unbind &u);
	std::shared_ptr<Graph> getGraph() const;
private:
	std::shared_ptr<Graph> g;
};
// rst-class-end:

// Expression
//------------------------------------------------------------------------------

class ComposeCommon;
class ComposeParallel;
class ComposeSub;
class ComposeSuper;

// rst-class: RCExp::Expression
// rst:
// rst:		A generic rule composition expression.
// rst:
// rst-class-start:

struct Expression {
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
	friend std::ostream &operator<<(std::ostream &s, const Expression &exp);
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


// rst-class: RCExp::ComposeBase
// rst:
// rst:		The base class for the composition of two rule :math:`(L_1, K_1, R_1)` and :math:`(L_2, K_2, R_2)`.
// rst:
// rst-class-start:

class ComposeBase {
protected:
	ComposeBase(Expression first, Expression second, bool discardNonchemical);
public:
	virtual ~ComposeBase();
	friend std::ostream &operator<<(std::ostream &s, const ComposeBase &compose);
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

// rst-class: RCExp::ComposeCommon
// rst:
// rst:		Compose the rules by all common subgraphs of :math:`R_1` and :math:`L_2`,
// rst:		possibly limited to connected subgraphs or to the subgraphs of maximum size.
// rst:
// rst-class-start:

struct ComposeCommon : public ComposeBase {
	ComposeCommon(Expression first, Expression second, bool discardNonchemical, bool maximum, bool connected);
	bool getMaxmimum() const;
	bool getConnected() const;
private:
	std::ostream &print(std::ostream &s) const;
private:
	bool maximum, connected;
};
// rst-class-end:

// rst-class: RCExp::ComposeParallel
// rst:
// rst:		Compose the rules by the empty graph, i.e., create a rule representing the parallel application of two input rules.
// rst:
// rst-class-start:

struct ComposeParallel : public ComposeBase {
	ComposeParallel(Expression first, Expression second, bool discardNonchemical);
private:
	std::ostream &print(std::ostream &s) const;
};
// rst-class-end:

// rst-class: RCExp::ComposeSub
// rst:
// rst:		Compose the rules such that overlapping connected components of :math:`R_1` and :math:`L_2` have the :math:`L_2` component as a subgraph of :math:`R_1`.
// rst:		The overlap is `partial` if not every connected component of :math:`L_2` is participating in the common subgraph.
// rst:
// rst-class-start:

struct ComposeSub : public ComposeBase {
	ComposeSub(Expression first, Expression second, bool discardNonchemical, bool allowPartial);
	bool getAllowPartial() const;
private:
	std::ostream &print(std::ostream &s) const;
private:
	bool allowPartial;
};
// rst-class-end:

// rst-class: RCExp::ComposeSuper
// rst:
// rst:		Compose the rules such that overlapping connected components of :math:`R_1` and :math:`L_2` have the :math:`R_1` component as a subgraph of :math:`L_2`.
// rst:		The overlap is `partial` if not every connected component of :math:`R_1` is participating in the common subgraph.
// rst:
// rst-class-start:

struct ComposeSuper : public ComposeBase {
	ComposeSuper(Expression first, Expression second, bool discardNonchemical, bool allowPartial, bool enforceConstraints);
	bool getAllowPartial() const;
	bool getEnforceConstraints() const;
private:
	std::ostream &print(std::ostream &s) const;
private:
	bool allowPartial, enforceConstraints;
};
// rst-class-end:

} // namespace RCExp
} // namespace mod

#endif /* MOD_RC_H */