#ifndef MOD_DERIVATION_HPP
#define MOD_DERIVATION_HPP

#include <mod/BuildConfig.hpp>
#include <mod/graph/ForwardDecl.hpp>
#include <mod/rule/ForwardDecl.hpp>

#include <memory>
#include <vector>

namespace mod {
struct Derivations;

// rst-class: Derivation
// rst:
// rst:		This class is a simple wrapper for passing data around.
// rst:		It contains two multisets of graphs :math:`G` and :math:`H`,
// rst:		and optionally a rule :math:`p`.
// rst:		An object may thus implicitly store a set of direct derivations
// rst:		:math:`G\Rightarrow^p H`, though the validity of the data is not checked.
// rst:
// rst-class-start:
struct MOD_DECL Derivation {
	// rst: .. type:: GraphList = std::vector<std::shared_ptr<graph::Graph>>
	// rst:
	// rst:		Representation of a multiset of graphs.
	using GraphList = std::vector<std::shared_ptr<graph::Graph>>;
public:
	~Derivation();
	MOD_DECL friend std::ostream &operator<<(std::ostream &s, const Derivation &d);
public:
	// rst: .. member:: GraphList left
	// rst:
	// rst:		Represents the multi-set of left graphs :math:`G`.
	GraphList left;
	// rst: .. member:: std::shared_ptr<rule::Rule> r
	// rst:
	// rst:		Represents the transformation rule :math:`p`, or no rule at all.
	std::shared_ptr<rule::Rule> r;
	// rst: .. member:: GraphList right
	// rst:
	// rst:		Represents the multi-set of right graphs :math:`H`.
	GraphList right;
public:
	// rst: .. function:: operator Derivations() const
	// rst:
	// rst:		:returns: the :cpp:class:`Derivations` object equivalent to this object.
	operator Derivations() const;
};
// rst-class-end:


// rst-class: Derivations
// rst:
// rst:		This class is a simple wrapper for passing data around.
// rst:		It contains two multisets of graphs :math:`G` and :math:`H`,
// rst:		and (possibly empty) set of rules.
// rst:		An object may thus implicitly store a set of direct derivations
// rst:		:math:`G\Rightarrow^p H` for each rule :math:`p`,
// rst:		though the validity of the data is not checked.
// rst:
// rst-class-start:
struct MOD_DECL Derivations {
	// rst: .. type:: GraphList = std::vector<std::shared_ptr<graph::Graph>>
	// rst:
	// rst:		Representation of a multiset of graphs.
	using GraphList = std::vector<std::shared_ptr<graph::Graph>>;
public:
	~Derivations();
	MOD_DECL friend std::ostream &operator<<(std::ostream &s, const Derivations &d);
public:
	// rst: .. member:: GraphList left
	// rst:
	// rst:		Represents the multi-set of left graphs :math:`G`.
	GraphList left;
	// rst: .. member:: std::vector<std::shared_ptr<rule::Rule>> rules
	// rst:
	// rst:		Represents a (possibly empty) set of transformation rules.
	std::vector<std::shared_ptr<rule::Rule>> rules;
	// rst: .. member:: GraphList right
	// rst:
	// rst:		Represents the multi-set of right graphs :math:`H`.
	GraphList right;
};
// rst-class-end:

} // namespace mod

#endif // MOD_DERIVATION_HPP