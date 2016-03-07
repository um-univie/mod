#ifndef MOD_LIB_RULE_REAL_H
#define MOD_LIB_RULE_REAL_H

#include <mod/BuildConfig.h>
#include <mod/Config.h>
#include <mod/lib/Graph/Base.h>
#include <mod/lib/Rule/Base.h>
#include <mod/lib/Rule/LabelledRule.h>

#include <jla_boost/graph/morphism/Predicates.hpp>

#include <boost/bimap.hpp>

#include <map>

namespace mod {
namespace lib {
namespace Graph {
class Single;
} // namespace Graph

namespace Rule {
struct PropStringCore;
struct PropMoleculeCore;
struct DepictionDataCore;

using DPOProjection = LabelledRule::LabelledLeftType::GraphType;
using ComponentFilter = ConnectedComponentFilter<DPOProjection, std::vector<std::size_t> >;
using ComponentGraph = boost::filtered_graph<DPOProjection, ComponentFilter, ComponentFilter>;

struct RuleSideComponentRange {
	using const_iterator = std::vector<ComponentGraph>::const_iterator;
public:

	RuleSideComponentRange(const std::vector<ComponentGraph> &c) : c(c) { }

	std::size_t size() const {
		return c.size();
	}

	const_iterator begin() const {
		return c.begin();
	}

	const_iterator end() const {
		return c.end();
	}

	const ComponentGraph &operator[](std::size_t i) const {
		return c[i];
	}
private:
	const std::vector<ComponentGraph> &c;
};

struct Real : public Base {
	using ComponentConstraint = GraphMorphism::MatchConstraint::Constraint<GraphType>;
	using CoreCoreMap = boost::bimap<Vertex, Vertex>;
public: // DPO requirements
	using LeftComponents = RuleSideComponentRange;
	using RightComponents = LeftComponents;
public:
	static bool sanityChecks(const GraphType &core, const PropStringCore &labelState, std::ostream &s);
public:
	Real(LabelledRule &&rule);
	LabelledRule clone() const;
	virtual ~Real() override;
	virtual bool isReal() const override;
	const LabelledRule &getDPORule() const;
	const GraphType &getGraph() const;
	const DPOProjection &getLeft() const;
	const DPOProjection &getContext() const;
	const DPOProjection &getRight() const;
	RuleSideComponentRange getLeftComponents() const;
	RuleSideComponentRange getRightComponents() const;
	const std::vector<std::unique_ptr<LabelledRule::MatchConstraint> > &getLeftConstraints() const;
	const std::vector<std::size_t> &getCompMapLeft() const;
	const std::vector<std::size_t> &getCompMapRight() const;
	const PropStringCore &getStringState() const;
	const PropMoleculeCore &getMoleculeState() const;
	DepictionDataCore &getDepictionData();
	const DepictionDataCore &getDepictionData() const;
	bool isChemical() const;
	bool isOnlySide(Membership membership) const;
	bool isOnlyRightSide() const; // shortcut of above
	std::size_t getNumLeftComponents() const;
	std::size_t getNumRightComponents() const;
public:
	static std::size_t isomorphism(const Rule::Real &rDom, const Rule::Real &rCodom, std::size_t maxNumMatches);
	static std::size_t monomorphism(const Rule::Real &rDom, const Rule::Real &rCodom, std::size_t maxNumMatches);
	static std::shared_ptr<mod::Rule> createSide(const lib::Graph::GraphType &g, const lib::Graph::PropStringType &pString, Membership membership, const std::string &name);
	static void invert(GraphType &g, PropStringCore &pString);
private:
	LabelledRule dpoRule;
	DPOProjection left, context, right;
	mutable std::unique_ptr<PropMoleculeCore> moleculeState;
	mutable std::unique_ptr<DepictionDataCore> depictionData;
private:
	struct ComponentStorage;
	mutable std::unique_ptr<ComponentStorage> componentStore;
	ComponentStorage &getComponentStore();
	const ComponentStorage &getComponentStore() const;
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
	auto operator()(const OuterGraph &gDomain, const OuterGraph &gCodomain, Pred pred) const
	-> decltype(jla_boost::GraphMorphism::makePropertyPredicate(
			makeMembershipPropertyMap(get_graph(gDomain)),
			makeMembershipPropertyMap(get_graph(gCodomain)), pred)) {
		return jla_boost::GraphMorphism::makePropertyPredicate(
				makeMembershipPropertyMap(get_graph(gDomain)),
				makeMembershipPropertyMap(get_graph(gCodomain)), pred);
	}
};

} // namespace Rule
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RULE_REAL_H */
