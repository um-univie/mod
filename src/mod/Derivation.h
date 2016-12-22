#ifndef MOD_DERIVATION_H
#define MOD_DERIVATION_H

#include <memory>
#include <vector>

namespace mod {
class Graph;
class Rule;

// rst-class: Derivation
// rst:
// rst:		This class represents a derivation :math:`G\Rightarrow^p H`, with :math:`G` and :math:`H` being multisets of graphs.
// rst:
// rst-class-start:

struct Derivation {
	// rst: .. type:: std::vector<std::shared_ptr<Graph> > GraphList
	// rst:
	// rst:		Representation of a multiset of graphs.
	using GraphList = std::vector<std::shared_ptr<Graph> >;
public:
	friend std::ostream &operator<<(std::ostream &s, const Derivation &d);
public:
	// rst: .. member:: GraphList left
	// rst:
	// rst:		Represents the multi-set of left graphs :math:`G` in a derivation :math:`G\Rightarrow^p H`.
	GraphList left;
	// rst: .. member:: std::shared_ptr<Rule> rule
	// rst:
	// rst:		Represents the transformation rule :math:`p` in a derivation :math:`G\Rightarrow^p H`.
	std::shared_ptr<Rule> rule;
	// rst: .. member:: GraphList right
	// rst:
	// rst:		Represents the multi-set of right graphs :math:`H` in a derivation :math:`G\Rightarrow^p H`.
	GraphList right;
};
// rst-class-end:

} // namespace mod

#endif /* MOD_DERIVATION_H */