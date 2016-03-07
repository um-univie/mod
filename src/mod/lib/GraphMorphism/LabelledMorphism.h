#ifndef MOD_LIB_GRAPHMORPHISM_LABELLEDMORPHISM_H
#define MOD_LIB_GRAPHMORPHISM_LABELLEDMORPHISM_H

#include <mod/Config.h>

#include <mod/lib/LabelledGraph.h>
#include <jla_boost/graph/morphism/Finder.hpp>
#include <jla_boost/graph/morphism/Predicates.hpp>

namespace mod {
namespace lib {
namespace GraphMorphism {
namespace detail {

template<typename OuterGraph>
auto get_vertex_order_impl(const OuterGraph &gOuter, int) -> decltype(get_vertex_order(gOuter)) {
	return get_vertex_order(gOuter);
}

template<typename OuterGraph>
std::vector<typename boost::graph_traits<typename LabelledGraphTraits<OuterGraph>::GraphType>::vertex_descriptor>
get_vertex_order_impl(const OuterGraph &gOuter, long /* worse than int */) {
	return get_vertex_order(jla_boost::GraphMorphism::DefaultFinderArgsProvider(), get_graph(gOuter));
}

template<typename OuterGraph>
struct ArgsProvider {
	using GraphType = typename LabelledGraphTraits<OuterGraph>::GraphType;

	ArgsProvider(const OuterGraph &gOuter) : gOuter(gOuter) { }

	friend std::vector<typename boost::graph_traits<GraphType>::vertex_descriptor>
	get_vertex_order(const ArgsProvider &arp, const GraphType &g) {
		assert(&g == &get_graph(arp.gOuter));
		return get_vertex_order_impl(arp.gOuter, int());
	}
private:
	const OuterGraph &gOuter;
};

template<typename OuterGraph>
ArgsProvider<OuterGraph> makeArgsProvider(const OuterGraph &gOuter) {
	return ArgsProvider<OuterGraph>(gOuter);
}

namespace GM = jla_boost::GraphMorphism;

template<typename OuterDomain, typename OuterCodomain, typename MR, typename Finder, typename PredWrapper, typename MRWrapper>
bool morphismFinallyDoIt(const OuterDomain &gDomain, const OuterCodomain &gCodomain, Finder finder, MR mr, PredWrapper predWrapper, MRWrapper mrWrapper) {
	auto pred = predWrapper(gDomain, gCodomain, jla_boost::AlwaysTrue());
	auto mrFinal = mrWrapper(gDomain, gCodomain, mr);
	return finder(get_graph(gDomain), get_graph(gCodomain), mrFinal, pred, pred,
			makeArgsProvider(gDomain), makeArgsProvider(gCodomain));
}

//------------------------------------------------------------------------------

template<typename PredWrapper>
struct StringLabelPredWrapper {
	PredWrapper predWrapper;
public:

	StringLabelPredWrapper(PredWrapper predWrapper) : predWrapper(predWrapper) { }

	template<typename OuterDomain, typename OuterCodomain, typename Pred>
	auto operator()(const OuterDomain &gDomain, const OuterCodomain &gCodomain, Pred pred) const
	-> decltype(this->predWrapper(gDomain, gCodomain, GM::makePropertyPredicate(get_string(gDomain), get_string(gCodomain), pred))) {
		return predWrapper(gDomain, gCodomain, GM::makePropertyPredicate(get_string(gDomain), get_string(gCodomain), pred));
	}
};

} // namespace detail

struct IdentityWrapper {

	template<typename OuterDomain, typename OuterCodomain, typename F>
	F operator()(const OuterDomain &gDomain, const OuterCodomain &gCodomain, F f) const {
		return f;
	}
};

template<typename OuterDomain, typename OuterCodomain, typename MR, typename Finder, typename PredWrapper = IdentityWrapper, typename MRWrapper = IdentityWrapper>
bool morphismSelectByLabelSettings(const OuterDomain &gDomain, const OuterCodomain &gCodomain,
		Finder finder, MR mr, PredWrapper predWrapper = IdentityWrapper(), MRWrapper mrWrapper = IdentityWrapper()) {
	return detail::morphismFinallyDoIt(gDomain, gCodomain, finder, mr, detail::StringLabelPredWrapper<PredWrapper>(predWrapper), mrWrapper);
}

} // namespace GraphMorphism
} // namespace lib
} // namespace mod


#endif /* MOD_LIB_GRAPHMORPHISM_LABELLEDMORPHISM_H */
