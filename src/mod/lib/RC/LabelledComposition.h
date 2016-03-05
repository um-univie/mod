#ifndef MOD_LIB_RC_LABELLEDCOMPOSITION_H
#define MOD_LIB_RC_LABELLEDCOMPOSITION_H

// This file is specific to the label settings we use.

#include <mod/Config.h>
#include <mod/Error.h>
#include <mod/lib/RC/Compose.h>
#include <mod/lib/RC/Result.h>
#include <mod/lib/RC/Visitor/MatchConstraints.h>
#include <mod/lib/RC/Visitor/String.h>

namespace mod {
namespace lib {
namespace RC {
namespace detail {

template<bool Verbose, typename Result, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename VisitorT>
boost::optional<Result> composeLabelledFinallyDoIt(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, VisitorT visitor) {
	return compose<Verbose, Result>(rFirst, rSecond, match, visitor);
}

} // namespace detail

template<bool Verbose, typename Result, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename VisitorT = Visitor::Null>
boost::optional<Result> composeLabelled(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, VisitorT visitor = Visitor::Null()) {
	return detail::composeLabelledFinallyDoIt<Verbose, Result>(rFirst, rSecond, match, Visitor::makeVisitor(
															visitor,
															Visitor::String()
															));
}

} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_LABELLEDCOMPOSITION_H */