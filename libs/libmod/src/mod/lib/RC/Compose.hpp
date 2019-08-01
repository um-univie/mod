#ifndef MOD_LIB_RC_COMPOSE_H
#define MOD_LIB_RC_COMPOSE_H

// Generic DPO rule composition for rules represented as the pushout of
// L <- K -> R, i.e., the "core" graph.
// The graphs may not have parallel edges.

#include <mod/Error.hpp>
#include <mod/lib/RC/Visitor/Compound.hpp>
#include <mod/lib/RC/detail/CompositionHelper.hpp>

#include <jla_boost/graph/dpo/IO.hpp>
#include <jla_boost/graph/morphism/VertexMap.hpp>

namespace mod {
namespace lib {
namespace RC {

template<typename R>
using DPOTraits = jla_boost::GraphDPO::PushoutRuleTraits<R>;

template<bool Verbose, typename Result, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename VisitorT>
boost::optional<Result> compose(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, VisitorT visitor) {
	// match must be rSecond -> rFirst
	using RuleResult = typename Result::RuleResult;
	using WrappedVisitor = Visitor::Compound<VisitorT>;
	BOOST_CONCEPT_ASSERT((jla_boost::GraphDPO::WritablePushoutRuleConcept<RuleResult>));
	BOOST_CONCEPT_ASSERT((jla_boost::GraphDPO::PushoutRuleConcept<RuleFirst>));
	BOOST_CONCEPT_ASSERT((jla_boost::GraphDPO::PushoutRuleConcept<RuleSecond>));
	BOOST_CONCEPT_ASSERT((jla_boost::GraphMorphism::InvertibleVertexMapConcept<InvertibleVertexMap>));
	return detail::CompositionHelper<Verbose, Result, RuleFirst, RuleSecond,
			InvertibleVertexMap, WrappedVisitor>(rFirst, rSecond, match, Visitor::makeVisitor(std::move(visitor)))();
}

} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_COMPOSE_H */