#ifndef MOD_LIB_RC_MATCH_MAKER_COMPONENTWISE_UTIL_H
#define MOD_LIB_RC_MATCH_MAKER_COMPONENTWISE_UTIL_H

#include <mod/Error.h>
#include <mod/lib/GraphMorphism/LabelledMorphism.h>
#include <mod/lib/Rule/Real.h>

#include <jla_boost/graph/FilteredWrapper.hpp>
#include <jla_boost/graph/morphism/Predicates.hpp>
#include <jla_boost/graph/morphism/VectorVertexMap.hpp>
#include <jla_boost/graph/morphism/VF2Finder.hpp>

namespace mod {
namespace lib {
namespace RC {

namespace GM = jla_boost::GraphMorphism;

template<typename RuleDomain, typename RuleCodomain>
struct FilteredWrapperReinterpretMRWrapper {
	template<typename OuterDomain, typename OuterCodomain, typename MR>
	auto operator()(const OuterDomain &gDomain, const OuterCodomain &gCodomain, MR mr) const
	-> decltype(
			GM::makeReinterpreter<jla_boost::FilteredWrapper<typename RuleDomain::ComponentGraph> >(
			GM::makeReinterpreter<jla_boost::FilteredWrapper<typename RuleCodomain::ComponentGraph> >(mr))
			) {
		return GM::makeReinterpreter < jla_boost::FilteredWrapper<typename RuleDomain::ComponentGraph> >(
				GM::makeReinterpreter < jla_boost::FilteredWrapper<typename RuleCodomain::ComponentGraph> >(mr));
	}
};

template<typename Rule>
struct WrappedComponentGraph {
	using ComponentGraph = typename Rule::ComponentGraph;
	using GraphType = jla_boost::FilteredWrapper<ComponentGraph>;
	using PropStringType = typename Rule::PropStringType;
public:

	WrappedComponentGraph(const ComponentGraph &g, const Rule &r)
	: g(jla_boost::makeFilteredWrapper(g)), r(r) { }

	friend const GraphType &get_graph(const WrappedComponentGraph<Rule> &g) {
		return g.g;
	}

	friend PropStringType get_string(const WrappedComponentGraph<Rule> &g) {
		return get_string(g.r);
	}
private:
	GraphType g;
	const Rule &r;
};

template<typename Rule>
WrappedComponentGraph<Rule> makeWrappedComponentGraph(const typename Rule::ComponentGraph &g, const Rule &r) {
	return WrappedComponentGraph<Rule>(g, r);
}

template<typename RuleDomain, typename RuleCodomain>
struct RuleRuleComponentMonomorphism {
	using Morphism = GM::VectorVertexMap<
			typename RuleDomain::LabelledRule::GraphType,
			typename RuleCodomain::LabelledRule::GraphType>;
public:

	RuleRuleComponentMonomorphism(const RuleDomain &rDomain, const RuleCodomain &rCodomain, bool enforceConstraints)
	: rDomain(rDomain), rCodomain(rCodomain), enforceConstraints(enforceConstraints) { }

	std::vector<Morphism> operator()(std::size_t idPattern, std::size_t idHost) const {
		std::vector<Morphism> morphisms;
		auto mrStore = GM::makeStoreVertexMap(std::back_inserter(morphisms));
		const auto &pattern = get_component_graph(idPattern, rDomain);
		const auto &host = get_component_graph(idHost, rCodomain);
		auto patternWrapped = makeWrappedComponentGraph(pattern, rDomain);
		auto hostWrapped = makeWrappedComponentGraph(host, rCodomain);

		auto mrCheckConstraints = [&]() {
			const auto &constraints = get_match_constraints(rDomain);
			auto labelHost = get_string(rCodomain);
			auto constraintsIterEnd = enforceConstraints ? constraints.end() : constraints.begin();
			return GraphMorphism::makeCheckConstraints(
					asRange(std::make_pair(constraints.begin(), constraintsIterEnd)),
					labelHost, mrStore);
		}();
		auto mrWrapper = FilteredWrapperReinterpretMRWrapper<RuleDomain, RuleCodomain>();
		auto mr = GM::makeToVectorVertexMap(
				GM::makeReinterpreter<typename RuleDomain::ComponentGraph > (
				GM::makeReinterpreter<typename RuleCodomain::ComponentGraph > (
				GM::makeReinterpreter<typename RuleDomain::GraphType > (
				GM::makeReinterpreter<typename RuleCodomain::GraphType > (
				mrCheckConstraints
				)))))
				;
		auto predWrapper = lib::GraphMorphism::IdentityWrapper();

		//				auto mrPrinter = GraphMorphism::Callback::makePrint(IO::log(), patternWrapped, targetWrapped, mrCheckConstraints);
		lib::GraphMorphism::morphismSelectByLabelSettings(patternWrapped, hostWrapped, GM::VF2Monomorphism(), mr, predWrapper, mrWrapper);
		return morphisms;
	}
private:
	const RuleDomain &rDomain;
	const RuleCodomain &rCodomain;
	const bool enforceConstraints;
};

template<typename RuleDomain, typename RuleCodomain>
inline RuleRuleComponentMonomorphism<RuleDomain, RuleCodomain>
makeRuleRuleComponentMonomorphism(const RuleDomain &rDomain, const RuleCodomain &rCodomain, bool enforceConstraints) {
	return RuleRuleComponentMonomorphism<RuleDomain, RuleCodomain>(rDomain, rCodomain, enforceConstraints);
}

} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_MATCH_MAKER_COMPONENTWISE_UTIL_H */
