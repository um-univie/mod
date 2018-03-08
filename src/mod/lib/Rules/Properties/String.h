#ifndef MOD_LIB_RULES_PROP_STRING_H
#define MOD_LIB_RULES_PROP_STRING_H

#include <mod/lib/GraphMorphism/Constraints/Constraint.h>
#include <mod/lib/Rules/GraphDecl.h>
#include <mod/lib/Rules/Properties/Property.h>

namespace mod {
namespace lib {
struct StringStore;
namespace Rules {
struct PropTermCore;

struct PropStringCore : PropCore<PropStringCore, GraphType, std::string, std::string> {
	using ConstraintPtr = std::unique_ptr<GraphMorphism::Constraints::Constraint<SideGraphType> >;
public:

	explicit PropStringCore(const GraphType &g) : PropCore(g) {
		verify(&g);
	}

	PropStringCore(const GraphType &g,
			const std::vector<ConstraintPtr> &leftMatchConstraints,
			const std::vector<ConstraintPtr> &rightMatchConstraints,
			const PropTermCore &term, const StringStore &strings);
};

} // namespace Rules
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RULES_PROP_STRING_H */