#ifndef MOD_LIB_RULES_PROP_TERM_HPP
#define MOD_LIB_RULES_PROP_TERM_HPP

#include <mod/lib/GraphMorphism/Constraints/Constraint.hpp>
#include <mod/lib/Rules/GraphDecl.hpp>
#include <mod/lib/Rules/Properties/Property.hpp>
#include <mod/lib/Term/WAM.hpp>

#include <optional>
#include <string>

namespace mod::lib::Rules {
struct PropStringCore;

struct PropTermCore : PropCore<PropTermCore, GraphType, std::size_t, std::size_t> {
	using Base = PropCore<PropTermCore, GraphType, std::size_t, std::size_t>;
	using ConstraintPtr = std::unique_ptr<GraphMorphism::Constraints::Constraint<SideGraphType> >;
public:
	PropTermCore(const GraphType &core,
			const std::vector<ConstraintPtr> &leftMatchConstraints,
			const std::vector<ConstraintPtr> &rightMatchConstraints,
			const PropStringCore &label, const StringStore &stringStore); // parse-construct
	PropTermCore(const GraphType &core, lib::Term::Wam machine); // import a machine
	friend bool isValid(const PropTermCore &core);
	const std::string &getParsingError() const;
	friend lib::Term::Wam &getMachine(PropTermCore &core);
	friend const lib::Term::Wam &getMachine(const PropTermCore &core);
private:
	std::optional<std::string> parsingError;
	lib::Term::Wam machine;
};

inline const lib::Term::Wam &getMachine(const PropTermCore::LeftType &p) {
	return getMachine(p.state.getDerived());
}

inline bool isValid(const PropTermCore::LeftType &p) {
	return isValid(p.state.getDerived());
}

inline const lib::Term::Wam &getMachine(const PropTermCore::RightType &p) {
	return getMachine(p.state.getDerived());
}

inline bool isValid(const PropTermCore::RightType &p) {
	return isValid(p.state.getDerived());
}

} // namespace mod::lib::Rules

#endif // MOD_LIB_RULES_PROP_TERM_HPP