#ifndef MOD_LIB_RULES_REAL_H
#define MOD_LIB_RULES_REAL_H

#include <mod/BuildConfig.h>
#include <mod/Config.h>
#include <mod/lib/Graph/GraphDecl.h>
#include <mod/lib/Rules/LabelledRule.h>

#include <jla_boost/graph/morphism/Predicates.hpp>

namespace mod {
struct Rule;
namespace lib {
namespace Graph {
struct PropString;
struct Single;
} // namespace Graph

namespace Rules {
struct PropStringCore;
struct PropMoleculeCore;
struct DepictionDataCore;

using DPOProjection = LabelledRule::LabelledLeftType::GraphType;

struct Real {
	static bool sanityChecks(const GraphType &core, const PropStringCore &labelState, std::ostream &s);
public:
	Real(LabelledRule &&rule);
	~Real();
	std::size_t getId() const;
	std::shared_ptr<mod::Rule> getAPIReference() const;
	void setAPIReference(std::shared_ptr<mod::Rule> r);
	const std::string &getName() const;
	void setName(std::string name);
	const LabelledRule &getDPORule() const;
public: // shorthands
	const GraphType &getGraph() const;
	const PropStringCore &getStringState() const;
	const PropMoleculeCore &getMoleculeState() const;
public:
	DepictionDataCore &getDepictionData();
	const DepictionDataCore &getDepictionData() const;
	bool isChemical() const;
	bool isOnlySide(Membership membership) const;
	bool isOnlyRightSide() const; // shortcut of above
public:
	static std::size_t isomorphism(const Real &rDom, const Real &rCodom, std::size_t maxNumMatches);
	static std::size_t monomorphism(const Real &rDom, const Real &rCodom, std::size_t maxNumMatches);
	static std::shared_ptr<mod::Rule> createSide(const lib::Graph::GraphType &g, const lib::Graph::PropString &pString, Membership membership, const std::string &name);
private:
	const std::size_t id;
	std::weak_ptr<mod::Rule> apiReference;
	std::string name;
private:
	LabelledRule dpoRule;
	mutable std::unique_ptr<PropMoleculeCore> moleculeState;
	mutable std::unique_ptr<DepictionDataCore> depictionData;
};

struct LessById {

	bool operator()(const Real *r1, const Real *r2) const {
		return r1->getId() < r2->getId();
	}
};

namespace detail {

struct IsomorphismPredicate {

	bool operator()(const Real *rDom, const Real *rCodom) const {
		return 1 == Real::isomorphism(*rDom, *rCodom, 1);
	}
};

} // namespace detail

inline detail::IsomorphismPredicate makeIsomorphismPredicate() {
	return detail::IsomorphismPredicate();
}

struct MembershipPredWrapper {

	template<typename OuterGraph, typename Pred>
	auto operator()(const OuterGraph &gDomain, const OuterGraph &gCodomain, Pred pred) const {
		return jla_boost::GraphMorphism::makePropertyPredicateEq(
				makeMembershipPropertyMap(get_graph(gDomain)),
				makeMembershipPropertyMap(get_graph(gCodomain)), pred);
	}
};

} // namespace Rules
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RULES_REAL_H */
