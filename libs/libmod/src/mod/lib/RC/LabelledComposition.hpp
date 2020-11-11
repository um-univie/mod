#ifndef MOD_LIB_RC_LABELLEDCOMPOSITION_H
#define MOD_LIB_RC_LABELLEDCOMPOSITION_H

// This file is specific to the label settings we use.

#include <mod/Config.hpp>
#include <mod/Error.hpp>
#include <mod/lib/RC/Compose.hpp>
#include <mod/lib/RC/Result.hpp>
#include <mod/lib/RC/Visitor/MatchConstraints.hpp>
#include <mod/lib/RC/Visitor/Stereo.hpp>
#include <mod/lib/RC/Visitor/String.hpp>
#include <mod/lib/RC/Visitor/Term.hpp>

namespace mod {
namespace lib {
namespace RC {
namespace detail {

template<bool Verbose, typename Result, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename VisitorT>
boost::optional<Result> composeLabelledFinallyDoIt(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, VisitorT visitor,
                                                   const std::vector<size_t>* copyVertices) {
	return compose<Verbose, Result>(rFirst, rSecond, match, std::move(visitor), copyVertices);
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
boost::optional<Result> composeLabelled(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, VisitorT visitor = Visitor::Null(),
                                        const std::vector<size_t>* copyVertices = nullptr) {
	return detail::composeLabelledFinallyDoIt<Verbose, Result>(rFirst, rSecond, match, Visitor::makeVisitor(
			std::move(visitor),
			typename detail::LabelTypeToVisitor<labelType>::type(),
	        typename detail::WithStereoVisitor<withStereo>::type()
	        ), copyVertices);
}

} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_LABELLEDCOMPOSITION_H */
