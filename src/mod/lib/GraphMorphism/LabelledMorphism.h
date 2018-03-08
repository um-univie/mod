#ifndef MOD_LIB_GRAPHMORPHISM_LABELLEDMORPHISM_H
#define MOD_LIB_GRAPHMORPHISM_LABELLEDMORPHISM_H

#include <mod/Config.h>
#include <mod/Error.h>

#include <mod/lib/GraphMorphism/Finder.hpp>
#include <mod/lib/GraphMorphism/StereoVertexMap.h>
#include <mod/lib/GraphMorphism/TermVertexMap.h>
#include <mod/lib/LabelledGraph.h>

#include <jla_boost/graph/morphism/Predicates.hpp>
#include <jla_boost/graph/morphism/callbacks/Filter.hpp>

// A labelled morphism is here any of the labelled morphisms arising from
// combinations of the LabelSettings members.

namespace mod {
namespace lib {
namespace GraphMorphism {
namespace detail {

template<typename...>
using Void = void;

template<typename VertexMap, typename = void>
struct HasTermData : std::false_type {
};

template<typename VertexMap>
struct HasTermData<VertexMap, Void<decltype(get_prop(TermDataT(), std::declval<VertexMap>()))> > : std::true_type {
};

template<typename VertexMap, typename = void>
struct HasStereoData : std::false_type {
};

template<typename VertexMap>
struct HasStereoData<VertexMap, Void<decltype(get_prop(StereoDataT(), std::declval<VertexMap>()))> > : std::true_type {
};

} // namespace detail

template<typename VertexMap>
using HasTermData = detail::HasTermData<VertexMap>;

template<typename VertexMap>
using HasStereoData = detail::HasStereoData<VertexMap>;

namespace detail {

template<typename OuterGraph>
auto get_vertex_order_impl(const OuterGraph &gOuter, int) -> decltype(get_vertex_order(gOuter)) {
	return get_vertex_order(gOuter);
}

template<typename OuterGraph>
decltype(auto) get_vertex_order_impl(const OuterGraph &gOuter, ... /* worse than everything */) {
	return get_vertex_order(DefaultFinderArgsProvider(), get_graph(gOuter));
}

template<typename OuterGraph>
struct ArgsProvider {
	using GraphType = typename LabelledGraphTraits<OuterGraph>::GraphType;

	ArgsProvider(const OuterGraph &gOuter) : gOuter(gOuter) { }

	friend decltype(auto) get_vertex_order(const ArgsProvider &arp, const GraphType &g) {
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

template<typename LabGraphDom, typename LabGraphCodom, typename MR, typename Finder, typename PredWrapper, typename MRWrapper>
bool morphismFinallyDoIt(const LabGraphDom &gDom, const LabGraphCodom &gCodom, Finder finder, MR mr, PredWrapper predWrapper, MRWrapper mrWrapper) {
	auto predOuter = predWrapper(gDom, gCodom, jla_boost::AlwaysTrue());
	auto pred = [&predOuter, &gDom, &gCodom](const auto &l, const auto &r) {
		return predOuter(l, r, gDom, gCodom);
	};
	auto mrWrapped = mrWrapper(gDom, gCodom, mr);
	return finder(get_graph(gDom), get_graph(gCodom), mrWrapped, pred, pred,
			makeArgsProvider(gDom), makeArgsProvider(gCodom));
}

//------------------------------------------------------------------------------

template<typename LabGraphDom, typename LabGraphCodom, typename MR, typename Finder, typename PredWrapper, typename MRWrapper, typename TermFilter>
bool morphismCreateTermRelation(const LabGraphDom &gDomain, const LabGraphCodom &gCodomain, Finder finder,
		MR mr, PredWrapper predWrapper, MRWrapper mrWrapper, TermFilter termFilter) {
	auto mrFinal = makeToTermVertexMap(gDomain, gCodomain, GM::makeFilter(termFilter, mr));
	return morphismFinallyDoIt(gDomain, gCodomain, finder, mrFinal, predWrapper, mrWrapper);
}

//------------------------------------------------------------------------------

template<typename LabGraphDom, typename LabGraphCodom, typename MR, typename Finder, typename PredWrapper, typename MRWrapper>
bool morphismSelectTermRelation(const LabGraphDom &gDomain, const LabGraphCodom &gCodomain, LabelRelation labelRelation, Finder finder,
		MR mr, PredWrapper predWrapper, MRWrapper mrWrapper) {
	switch(labelRelation) {
	case LabelRelation::Isomorphism:
		return morphismCreateTermRelation(gDomain, gCodomain, finder, mr, predWrapper, mrWrapper, TermFilterRenaming());
	case LabelRelation::Specialisation:
		return morphismCreateTermRelation(gDomain, gCodomain, finder, mr, predWrapper, mrWrapper, TermFilterSpecialisation());
	case LabelRelation::Unification:
		return morphismCreateTermRelation(gDomain, gCodomain, finder, mr, predWrapper, mrWrapper, jla_boost::AlwaysTrue());
	}
	MOD_ABORT;
}

//------------------------------------------------------------------------------

template<typename PredWrapper>
struct StringLabelPredWrapper {
	PredWrapper predWrapper;
public:

	StringLabelPredWrapper(PredWrapper predWrapper) : predWrapper(predWrapper) { }

	template<typename LabGraphDom, typename LabGraphCodom, typename Pred>
	auto operator()(const LabGraphDom &gDomain, const LabGraphCodom &gCodomain, Pred pred) const {
		return predWrapper(gDomain, gCodomain, GM::makePropertyPredicateEq(get_string(gDomain), get_string(gCodomain), pred));
	}
};

template<typename PredWrapper>
struct TermLabelPredWrapper {
	PredWrapper predWrapper;
public:

	TermLabelPredWrapper(PredWrapper predWrapper) : predWrapper(predWrapper) { }

	template<typename LabGraphDom, typename LabGraphCodom, typename Pred>
	auto operator()(const LabGraphDom &gDomain, const LabGraphCodom &gCodomain, Pred pred) const {
		// TODO: these two should only differ in efficiency, but the maximum common subgraph enumerator
		// will do maximum on the unfiltered morphisms.
		// TODO: test with the AlwaysTrue predicate when the maximum common subgraph enumerator has been fixed.
		//		return predWrapper(gDomain, gCodomain, jla_boost::AlwaysTrue());
		return predWrapper(gDomain, gCodomain, lib::GraphMorphism::makeTermPredConstants(pred));
	}
};

template<typename LabGraphDom, typename LabGraphCodom, typename MR, typename Finder, typename PredWrapper, typename MRWrapper>
bool morphismSelectStringOrTerm(const LabGraphDom &gDomain, const LabGraphCodom &gCodomain, LabelType labelType, LabelRelation labelRelation,
		Finder finder, MR mr, PredWrapper predWrapper, MRWrapper mrWrapper) {
	switch(labelType) {
	case LabelType::String:
		switch(labelRelation) {
		case LabelRelation::Isomorphism:
		case LabelRelation::Specialisation:
		case LabelRelation::Unification:
			return detail::morphismFinallyDoIt(gDomain, gCodomain, finder, mr, StringLabelPredWrapper<PredWrapper>(predWrapper), mrWrapper);
		}
		MOD_ABORT;
	case LabelType::Term:
		return detail::morphismSelectTermRelation(gDomain, gCodomain, labelRelation, finder, mr, TermLabelPredWrapper<PredWrapper>(predWrapper), mrWrapper);
	}
	MOD_ABORT;
}

//------------------------------------------------------------------------------

template<typename PredWrapper>
struct StereoPredWrapperIso {
	PredWrapper predWrapper;
public:

	StereoPredWrapperIso(PredWrapper predWrapper) : predWrapper(predWrapper) { }

	template<typename LabGraphDom, typename LabGraphCodom, typename Pred>
	auto operator()(const LabGraphDom &gDomain, const LabGraphCodom &gCodomain, Pred pred) const {
		return predWrapper(gDomain, gCodomain, lib::GraphMorphism::Stereo::makePredIso(pred));
	}
};

template<typename PredWrapper>
struct StereoPredWrapperSpec {
	PredWrapper predWrapper;
public:

	StereoPredWrapperSpec(PredWrapper predWrapper) : predWrapper(predWrapper) { }

	template<typename LabGraphDom, typename LabGraphCodom, typename Pred>
	auto operator()(const LabGraphDom &gDom, const LabGraphCodom &gCodom, Pred pred) const {
		return predWrapper(gDom, gCodom, lib::GraphMorphism::Stereo::makePredSpec(pred));
	}
};

template<typename LabGraphDom, typename LabGraphCodom, typename MR, typename Finder, typename PredWrapper, typename MRWrapper>
bool morphismSelectStereo(const LabGraphDom &gDomain, const LabGraphCodom &gCodomain, LabelType labelType, LabelRelation labelRelation, LabelRelation stereoRelation,
		Finder finder, MR mr, PredWrapper predWrapper, MRWrapper mrWrapper) {
	switch(stereoRelation) {
	case LabelRelation::Isomorphism:
		return detail::morphismSelectStringOrTerm(gDomain, gCodomain, labelType, labelRelation, finder,
				Stereo::makeToVertexMapIso(gDomain, gCodomain, mr),
				StereoPredWrapperIso<PredWrapper>(predWrapper),
				mrWrapper
				);
	case LabelRelation::Specialisation:
		return detail::morphismSelectStringOrTerm(gDomain, gCodomain, labelType, labelRelation, finder,
				Stereo::makeToVertexMapSpec(gDomain, gCodomain, mr),
				StereoPredWrapperSpec<PredWrapper>(predWrapper),
				mrWrapper
				);
	case LabelRelation::Unification:
		MOD_ABORT; // not yet implemented
	}
	MOD_ABORT;
}


//------------------------------------------------------------------------------

} // namespace detail

struct IdentityWrapper {

	template<typename LabGraphDom, typename LabGraphCodom, typename F>
	F operator()(const LabGraphDom &gDomain, const LabGraphCodom &gCodomain, F f) const {
		return f;
	}
};

template<typename LabGraphDom, typename LabGraphCodom, typename MR, typename Finder, typename PredWrapper = IdentityWrapper, typename MRWrapper = IdentityWrapper>
bool morphismSelectByLabelSettings(const LabGraphDom &gDomain, const LabGraphCodom &gCodomain, LabelSettings labelSettings,
		Finder finder, MR mr, PredWrapper predWrapper = IdentityWrapper(), MRWrapper mrWrapper = IdentityWrapper()) {
	if(labelSettings.withStereo) {
		return detail::morphismSelectStereo(gDomain, gCodomain, labelSettings.type, labelSettings.relation, labelSettings.stereoRelation, finder, mr, predWrapper, mrWrapper);
	} else {
		return detail::morphismSelectStringOrTerm(gDomain, gCodomain, labelSettings.type, labelSettings.relation, finder, mr, predWrapper, mrWrapper);
	}
}

} // namespace GraphMorphism
} // namespace lib
} // namespace mod


#endif /* MOD_LIB_GRAPHMORPHISM_LABELLEDMORPHISM_H */
