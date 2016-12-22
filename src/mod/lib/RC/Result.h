#ifndef MOD_LIB_RC_RESULT_H
#define	MOD_LIB_RC_RESULT_H

#include <jla_boost/graph/dpo/Rule.hpp>
#include <jla_boost/graph/morphism/VectorVertexMap.hpp>

namespace mod {
namespace lib {
namespace RC {

//------------------------------------------------------------------------------
// ResultConcept
//------------------------------------------------------------------------------
//
//template<typename Result>
//struct ResultConcept {
//
//	BOOST_CONCEPT_USAGE(ResultConcept) {
//		const auto &g = *result.graph;
//		using Graph = typename std::remove_cv<typename std::remove_reference < decltype(g)>::type>::type;
//		BOOST_CONCEPT_ASSERT((boost::MutableGraphConcept<Graph>));
//		auto &mLeft = result.mLeftToResult;
//		auto &mRight = result.mRightToResult;
//		using MapLeft = typename std::remove_reference < decltype(mLeft)>::type;
//		using MapRight = typename std::remove_reference < decltype(mRight)>::type;
//		BOOST_CONCEPT_ASSERT((jla_boost::GraphMorphism::InvertibleVertexMapConcept<MapLeft>));
//		BOOST_CONCEPT_ASSERT((jla_boost::GraphMorphism::InvertibleVertexMapConcept<MapRight>));
//		(void) g;
//		(void) mLeft;
//		(void) mRight;
//	}
//private:
//	Result result;
//};

//------------------------------------------------------------------------------
// BaseResult
//------------------------------------------------------------------------------

template<typename RuleResultT, typename RuleFirst, typename RuleSecond>
struct BaseResult {
	using RuleResult = RuleResultT;
	BOOST_CONCEPT_ASSERT((jla_boost::GraphDPO::PushoutRuleConcept<RuleResult>));
	BOOST_CONCEPT_ASSERT((jla_boost::GraphDPO::PushoutRuleConcept<RuleFirst>));
	BOOST_CONCEPT_ASSERT((jla_boost::GraphDPO::PushoutRuleConcept<RuleSecond>));
	using GraphResult = typename jla_boost::GraphDPO::PushoutRuleTraits<RuleResult>::GraphType;
	using GraphFirst = typename jla_boost::GraphDPO::PushoutRuleTraits<RuleFirst>::GraphType;
	using GraphSecond = typename jla_boost::GraphDPO::PushoutRuleTraits<RuleSecond>::GraphType;

	template<typename ...Args>
	BaseResult(const RuleFirst &rFirst, const RuleSecond &rSecond, Args&&... args)
	: rResult(std::forward<Args>(args)...),
	mFirstToResult(get_graph(rFirst), get_graph(rResult)),
	mSecondToResult(get_graph(rSecond), get_graph(rResult)) { }
public:
	RuleResult rResult;
	jla_boost::GraphMorphism::VectorVertexMap<GraphFirst, GraphResult> mFirstToResult;
	jla_boost::GraphMorphism::VectorVertexMap<GraphSecond, GraphResult> mSecondToResult;
};

} // namespace RC
} // namespace lib
} // namespace mod


#endif	/* MOD_LIB_RC_RESULT_H */