#ifndef MOD_LIB_RC_MATCH_MAKER_COMPONENTWISE_UTIL_H
#define MOD_LIB_RC_MATCH_MAKER_COMPONENTWISE_UTIL_H

#include <mod/Error.h>
#include <mod/lib/GraphMorphism/LabelledMorphism.h>
#include <mod/lib/GraphMorphism/VF2Finder.hpp>
#include <mod/lib/Rules/Real.h>

#include <jla_boost/graph/FilteredWrapper.hpp>
#include <jla_boost/graph/morphism/Predicates.hpp>
#include <jla_boost/graph/morphism/UnwrapperCallback.hpp>
#include <jla_boost/graph/morphism/VectorVertexMap.hpp>

namespace mod {
namespace lib {
namespace RC {

namespace GM = jla_boost::GraphMorphism;
namespace GM_MOD = lib::GraphMorphism;

template<typename RuleDomain, typename RuleCodomain>
struct FilteredWrapperReinterpretMRWrapper {

	template<typename OuterDomain, typename OuterCodomain, typename MR>
	auto operator()(const OuterDomain &gDomain, const OuterCodomain &gCodomain, MR mr) const {
		return GM::makeUnwrapperLeft < jla_boost::FilteredWrapper<typename RuleDomain::ComponentGraph> >(
				GM::makeUnwrapperRight < jla_boost::FilteredWrapper<typename RuleCodomain::ComponentGraph> >(mr));
	}
};

template<typename Rule>
struct WrappedComponentGraph {
	using ComponentGraph = typename Rule::ComponentGraph;
	using GraphType = jla_boost::FilteredWrapper<ComponentGraph>;
	using PropStringType = typename Rule::PropStringType;
public:

	WrappedComponentGraph(const ComponentGraph &g, std::size_t i, const Rule &r)
	: g(jla_boost::makeFilteredWrapper(g)), i(i), r(r) { }

	friend const GraphType &get_graph(const WrappedComponentGraph<Rule> &g) {
		return g.g;
	}

	friend PropStringType get_string(const WrappedComponentGraph<Rule> &g) {
		return get_string(g.r);
	}

	friend const std::vector<typename boost::graph_traits<GraphType>::vertex_descriptor>&
	get_vertex_order(const WrappedComponentGraph<Rule> &g) {
		return get_vertex_order_component(g.i, g.r);
	}
private:
	GraphType g;
	std::size_t i;
	const Rule &r;
};

template<typename Rule>
WrappedComponentGraph<Rule> makeWrappedComponentGraph(const typename Rule::ComponentGraph &g, std::size_t i, const Rule &r) {
	return WrappedComponentGraph<Rule>(g, i, r);
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
		auto mrStore = GM::makeStore(std::back_inserter(morphisms));
		const auto &pattern = get_component_graph(idPattern, rDomain);
		const auto &host = get_component_graph(idHost, rCodomain);
		auto patternWrapped = makeWrappedComponentGraph(pattern, idPattern, rDomain);
		auto hostWrapped = makeWrappedComponentGraph(host, idHost, rCodomain);

		auto makeCheckConstraints = [&](auto &&mrNext) {
			const auto &constraints = get_match_constraints(rDomain);
			auto labelHost = get_string(rCodomain);
			auto constraintsIterEnd = enforceConstraints ? constraints.end() : constraints.begin();
			return GraphMorphism::makeCheckConstraints(
					asRange(std::make_pair(constraints.begin(), constraintsIterEnd)),
					labelHost, mrNext);
		};
		// First reinterpret the vertex descriptors from the reindexed graphs to their parent graphs.
		auto mrWrapper = FilteredWrapperReinterpretMRWrapper<RuleDomain, RuleCodomain>();
		// Then do whatever checked is needed by the labelled morphisms (morphismSelectByLabelSettings injects those).
		// And now process the final morphisms:
		auto mr =
				// The next two unwrappings will increase the domain and codomain,
				// so we first need to capture the mapping:
				// Store them in a vector (which runs through the domain graph):
				GM::makeToVectorVertexMap(
				// Unwrap the filtering by connected component, so we get side graphs:
				GM::makeUnwrapperLeft<typename RuleDomain::ComponentGraph > (
				GM::makeUnwrapperRight<typename RuleCodomain::ComponentGraph > (
				// Check constraints using the side graphs:
				makeCheckConstraints(
				// Unwrap the filtering by rule side, so we get to core graphs:
				GM::makeUnwrapperLeft<typename RuleDomain::GraphType > (
				GM::makeUnwrapperRight<typename RuleCodomain::GraphType > (
				// And finally push it into our storage:
				mrStore
				))))))
				;
		auto predWrapper = lib::GraphMorphism::IdentityWrapper();

		//				auto mrPrinter = GraphMorphism::Callback::makePrint(IO::log(), patternWrapped, targetWrapped, mrCheckConstraints);
		lib::GraphMorphism::morphismSelectByLabelSettings(patternWrapped, hostWrapped, GM_MOD::VF2Monomorphism(), mr, predWrapper, mrWrapper);
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
