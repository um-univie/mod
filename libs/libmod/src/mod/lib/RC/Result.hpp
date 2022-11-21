#ifndef MOD_LIB_RC_RESULT_HPP
#define MOD_LIB_RC_RESULT_HPP

#include <mod/lib/DPO/CombinedRule.hpp>

#include <jla_boost/graph/morphism/models/InvertibleVector.hpp>

namespace mod::lib::RC {

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

struct Result {
	using Rule = lib::DPO::CombinedRule;
	using CombinedGraph = Rule::CombinedGraphType;

	struct RuleResult { // TODO: temporary hax
		using GraphType = CombinedGraph;
		using SideGraphType = Rule::SideGraphType;
	};

	Result(const Rule &rFirst, const Rule &rSecond) :
			rDPO(new Rule()),
			mFirstToResult(rFirst.getCombinedGraph(), rDPO->getCombinedGraph()),
			mSecondToResult(rSecond.getCombinedGraph(), rDPO->getCombinedGraph()) {}
public:
	std::unique_ptr<Rule> rDPO;
	jla_boost::GraphMorphism::InvertibleVectorVertexMap<CombinedGraph, CombinedGraph> mFirstToResult;
	jla_boost::GraphMorphism::InvertibleVectorVertexMap<CombinedGraph, CombinedGraph> mSecondToResult;
};


struct LabelledResult : Result {
	using MatchConstraint = GraphMorphism::Constraints::Constraint<RuleResult::SideGraphType>;
	using PropStringType = lib::Rules::PropString;
	using PropTermType = lib::Rules::PropTerm;
public:
	using Result::Result;
public:
	std::unique_ptr<lib::Rules::PropString> pString;
	std::unique_ptr<lib::Rules::PropTerm> pTerm;
	std::unique_ptr<lib::Rules::PropStereo> pStereo;
	std::vector<std::unique_ptr<MatchConstraint>> matchConstraints;
};

} // namespace mod::lib::RC


#endif // MOD_LIB_RC_RESULT_HPP