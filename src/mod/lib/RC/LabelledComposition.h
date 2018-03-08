#ifndef MOD_LIB_RC_LABELLEDCOMPOSITION_H
#define MOD_LIB_RC_LABELLEDCOMPOSITION_H

// This file is specific to the label settings we use.

#include <mod/Config.h>
#include <mod/Error.h>
#include <mod/lib/RC/Compose.h>
#include <mod/lib/RC/Result.h>
#include <mod/lib/RC/Visitor/MatchConstraints.h>
#include <mod/lib/RC/Visitor/Stereo.h>
#include <mod/lib/RC/Visitor/String.h>
#include <mod/lib/RC/Visitor/Term.h>

namespace mod {
namespace lib {
namespace RC {
namespace detail {

template<bool Verbose, typename Result, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename VisitorT>
boost::optional<Result> composeLabelledFinallyDoIt(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, VisitorT visitor) {
	return compose<Verbose, Result>(rFirst, rSecond, match, std::move(visitor));
}

template<LabelType T>
struct LabelTypeToVisitor;

template<>
struct LabelTypeToVisitor<LabelType::String> {
	using type = Visitor::String;
};

template<>
struct LabelTypeToVisitor<LabelType::Term> {
	using type = Visitor::Term;
};

template<bool withStereo>
struct WithStereoVisitor;

template<>
struct WithStereoVisitor<true> {
	using type = Visitor::Stereo;
};

template<>
struct WithStereoVisitor<false> {
	using type = Visitor::Null;
};

} // namespace detail

template<bool Verbose, typename Result, LabelType labelType, bool withStereo, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename VisitorT = Visitor::Null>
boost::optional<Result> composeLabelled(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, VisitorT visitor = Visitor::Null()) {
	return detail::composeLabelledFinallyDoIt<Verbose, Result>(rFirst, rSecond, match, Visitor::makeVisitor(
			std::move(visitor),
			typename detail::LabelTypeToVisitor<labelType>::type(),
			typename detail::WithStereoVisitor<withStereo>::type()
			));
}

} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_LABELLEDCOMPOSITION_H */