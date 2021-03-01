#ifndef MOD_LIB_RULES_REAL_HPP
#define MOD_LIB_RULES_REAL_HPP

#include <mod/BuildConfig.hpp>
#include <mod/Config.hpp>
#include <mod/rule/ForwardDecl.hpp>
#include <mod/lib/Graph/GraphDecl.hpp>
#include <mod/lib/Rules/LabelledRule.hpp>
#include <mod/lib/Rules/Application/CanonRule.hpp>

#include <jla_boost/graph/morphism/Predicates.hpp>

#include <optional>

namespace mod::lib::Graph {
struct PropStereo;
struct PropString;
struct Single;
} // namespace mod::lib::Graph
namespace mod::lib::Rules {
struct PropStringCore;
struct PropMoleculeCore;
struct DepictionDataCore;

using DPOProjection = LabelledRule::LabelledLeftType::GraphType;

struct Real {
	static bool sanityChecks(const GraphType &core, const PropStringCore &labelState, std::ostream &s);
public:
	Real(LabelledRule &&rule, std::optional<LabelType> labelType);
	~Real();
	std::size_t getId() const;
	std::shared_ptr<rule::Rule> getAPIReference() const;
	void setAPIReference(std::shared_ptr<rule::Rule> r);
	const std::string &getName() const;
	void setName(std::string name);
	std::optional<LabelType> getLabelType() const;
	const LabelledRule &getDPORule() const;
public: // shorthands
	const GraphType &getGraph() const;
	DepictionDataCore &getDepictionData(); // TODO: should not be available as non-const
public:
	const DepictionDataCore &getDepictionData() const;
	bool isChemical() const;
	bool isOnlySide(Membership membership) const;
	bool isOnlyRightSide() const; // shortcut of above
public: // deprecated
	const PropStringCore &getStringState() const;
	const PropTermCore &getTermState() const;
	const PropMoleculeCore &getMoleculeState() const;
public:
	static std::size_t isomorphism(const Real &rDom,
	                               const Real &rCodom,
	                               std::size_t maxNumMatches,
	                               LabelSettings labelSettings);
	static std::size_t monomorphism(const Real &rDom,
	                                const Real &rCodom,
	                                std::size_t maxNumMatches,
	                                LabelSettings labelSettings);
	static bool isomorphicLeftRight(const Real &rDom,
	                                const Real &rCodom,
	                                LabelSettings labelSettings);
private:
	const std::size_t id;
	std::weak_ptr<rule::Rule> apiReference;
	std::string name;
	const std::optional<LabelType> labelType;
private:
	LabelledRule dpoRule;
	mutable std::unique_ptr<DepictionDataCore> depictionData;

public: // experimental rule autgroup
	const Rules::AutGroup &getAutGroup() const;
	mutable std::unique_ptr<const Rules::AutGroup> aut_group;
};

struct LessById {
	bool operator()(const Real *r1, const Real *r2) const {
		return r1->getId() < r2->getId();
	}
};

namespace detail {

struct IsomorphismPredicate {
	IsomorphismPredicate(LabelType labelType, bool withStereo)
			: settings(labelType, LabelRelation::Isomorphism, withStereo, LabelRelation::Isomorphism) {}

	bool operator()(const Real *rDom, const Real *rCodom) const {
		return 1 == Real::isomorphism(*rDom, *rCodom, 1, settings);
	}

private:
	LabelSettings settings;
};

} // namespace detail

inline detail::IsomorphismPredicate makeIsomorphismPredicate(LabelType labelType, bool withStereo) {
	return detail::IsomorphismPredicate(labelType, withStereo);
}

struct MembershipPredWrapper {
	template<typename OuterGraph, typename Pred>
	auto operator()(const OuterGraph &gDomain, const OuterGraph &gCodomain, Pred pred) const {
		return jla_boost::GraphMorphism::makePropertyPredicateEq(
				makeMembershipPropertyMap(get_graph(gDomain)),
				makeMembershipPropertyMap(get_graph(gCodomain)), pred);
	}
};

} // namespace mod::lib::Rules

#endif // MOD_LIB_RULES_REAL_HPP
