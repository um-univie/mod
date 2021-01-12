#ifndef MOD_RULE_COMPOSITION_H
#define MOD_RULE_COMPOSITION_H

#include <mod/BuildConfig.hpp>
#include <mod/Config.hpp>
#include <mod/graph/ForwardDecl.hpp>
#include <mod/rule/ForwardDecl.hpp>

#include <memory>
#include <unordered_set>

namespace mod::rule {

// rst-class: rule::Composer
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
class MOD_DECL Composer {
	Composer(const std::unordered_set<std::shared_ptr<Rule> > &database, LabelSettings labelSettings);
	Composer(Composer &&) = delete;
	Composer &operator=(Composer &&) = delete;
public:
	~Composer();
	// rst: .. function:: const std::unordered_set<std::shared_ptr<Rule> > &getRuleDatabase() const
	// rst: 
	// rst:		:returns: the set of unique rules known by the evaluator.
	const std::unordered_set<std::shared_ptr<Rule>> &getRuleDatabase() const;
	// rst: .. function:: const std::unordered_set<std::shared_ptr<Rule> > &getProducts() const
	// rst:
	// rst:		:returns: the set of unique rules this evaluator has constructed.
	const std::unordered_set<std::shared_ptr<Rule>> &getProducts() const;
	// rst: .. function:: std::vector<std::shared_ptr<Rule>> eval(const RCExp::Expression &exp, int verbosity)
	// rst:
	// rst:		Evaluates a rule composition expression.
	// rst:		Any created rule is replaced by a rule in the database if they are isomorphic.
	// rst:
	// rst:		A rule may appear multiple times in the result if multiple overlaps resulted in the same composed rule.
	// rst:
	// rst:		The :var:`verbosity` has the following meaning:
	// rst:
	// rst:		- 0 (or less): no information is printed.
	// rst:		- 10: Print information about morphism generation for rule composition.
	// rst:		- 20: Print rule composition information.
	// rst:
	// rst:		:returns: the result of the expression.
	std::vector<std::shared_ptr<Rule>> eval(const RCExp::Expression &exp, int verbosity);
	// rst: .. function:: void print() const
	// rst:
	// rst:		Print the graph representing all expressions evaluated so far.
	void print() const;
private:
	struct Pimpl;
	std::unique_ptr<Pimpl> p;
public:
	// rst: .. function:: static std::shared_ptr<Composer> create(const std::unordered_set<std::shared_ptr<Rule> > &database, LabelSettings labelSettings)
	// rst: 
	// rst:		:returns: a new instance of an evaluator with its database initialised with the given rules.
	// rst: 
	// rst:		.. note:: The caller is responsible for ensuring the given rules are unique.
	static std::shared_ptr<Composer> create(const std::unordered_set<std::shared_ptr<Rule> > &database,
														 LabelSettings labelSettings);
};
// rst-class-end:

} // namespace mod::rule

#endif /* MOD_RULE_COMPOSITION_H */