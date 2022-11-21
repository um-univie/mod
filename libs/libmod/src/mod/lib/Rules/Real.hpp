#ifndef MOD_LIB_RULES_REAL_HPP
#define MOD_LIB_RULES_REAL_HPP

#include <mod/BuildConfig.hpp>
#include <mod/Config.hpp>
#include <mod/rule/ForwardDecl.hpp>
#include <mod/lib/Graph/GraphDecl.hpp>
#include <mod/lib/Rules/LabelledRule.hpp>

#include <jla_boost/graph/morphism/Predicates.hpp>

#include <optional>

namespace mod::lib::Graph {
struct PropStereo;
struct PropString;
struct Single;
} // namespace mod::lib::Graph
namespace mod::lib::Rules {
struct PropString;
struct PropMolecule;
namespace Write {
struct DepictionData;
} // namespace Write

struct Real {
	static bool sanityChecks(const lib::DPO::CombinedRule::CombinedGraphType &gCombined,
	                         const PropString &pString, std::ostream &s);
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
public: // shorthands, deprecated
	const GraphType &getGraph() const;
	Write::DepictionData &getDepictionData(); // TODO: should not be available as non-const
public:
	const Write::DepictionData &getDepictionData() const;
	bool isChemical() const;
	bool isOnlySide(Membership membership) const;
	bool isOnlyRightSide() const; // shortcut of above
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
	mutable std::unique_ptr<Write::DepictionData> depictionData;
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

	bool operator()(const std::unique_ptr<Real> &rDom, const std::unique_ptr<Real> &rCodom) const {
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
	template<typename Pred>
	auto operator()(const LabelledRule &gLabDomain, const LabelledRule &gLabCodomain, Pred pred) const {
		return jla_boost::GraphMorphism::makePropertyPredicateEq(
				gLabDomain.getRule().makeMembershipPropertyMap(),
				gLabCodomain.getRule().makeMembershipPropertyMap(), pred);
	}
};

} // namespace mod::lib::Rules

#endif // MOD_LIB_RULES_REAL_HPP
