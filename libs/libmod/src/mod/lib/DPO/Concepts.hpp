#ifndef MOD_LIB_DPO_CONCEPTS_HPP
#define MOD_LIB_DPO_CONCEPTS_HPP

#include <mod/lib/DPO/Traits.hpp>

#include <jla_boost/graph/morphism/Concepts.hpp>

#include <boost/graph/graph_concepts.hpp>

namespace mod::lib::DPO {

template<typename Rule>
struct RuleConcept {
	using Traits = RuleTraits<Rule>;
	using SideGraphType = typename Traits::SideGraphType;
	using KGraphType = typename Traits::KGraphType;
	using MorphismType = typename Traits::MorphismType;

	BOOST_CONCEPT_ASSERT((boost::GraphConcept<SideGraphType>));
	BOOST_CONCEPT_ASSERT((boost::GraphConcept<KGraphType>));
	BOOST_CONCEPT_ASSERT((jla_boost::GraphMorphism::InvertibleGraphMapConcept<MorphismType>));

	BOOST_CONCEPT_USAGE(RuleConcept) {
		const Rule &rConst = r;
		[[maybe_unused]] const SideGraphType &gL = getL(rConst);
		[[maybe_unused]] const KGraphType &gK = getK(rConst);
		[[maybe_unused]] const SideGraphType &gR = getR(rConst);
		[[maybe_unused]] const MorphismType &mL = getMorL(rConst);
		[[maybe_unused]] const MorphismType &mR = getMorR(rConst);
	}
private:
	Rule r;
};

template<typename Rule>
struct WritableRuleConcept : RuleConcept<Rule> {
	BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<Rule>));

	using Traits = RuleTraits<Rule>;
	using SideGraphType = typename Traits::SideGraphType;
	using SideVertex = typename boost::graph_traits<SideGraphType>::vertex_descriptor;
	using SideEdge = typename boost::graph_traits<SideGraphType>::edge_descriptor;
	using KGraphType = typename Traits::KGraphType;
	using KVertex = typename boost::graph_traits<KGraphType>::vertex_descriptor;
	using KEdge = typename boost::graph_traits<KGraphType>::edge_descriptor;

	BOOST_CONCEPT_USAGE(WritableRuleConcept) {
		invert(r); // swap L and R
		[[maybe_unused]] const SideVertex vL = addVertexL(r);
		[[maybe_unused]] const KVertex vK = addVertexK(r);
		[[maybe_unused]] const SideVertex vR = addVertexR(r);
		[[maybe_unused]] const SideVertex vRfL = promoteVertexL(r, cvSide);
		//[[maybe_unused]] const SideVertex vLfR = promoteVertexR(r, cvSide);
		[[maybe_unused]] const SideEdge eL = addEdgeL(r, cvSide, cvSide);
		[[maybe_unused]] const KEdge eK = addEdgeK(r, cvK, cvK);
		[[maybe_unused]] const SideEdge eR = addEdgeR(r, cvSide, cvSide);
	}
private:
	Rule r;
	static const SideVertex cvSide;
	static const KVertex cvK;
};

} // namespace mod::lib::DPO

#endif // MOD_LIB_DPO_CONCEPTS_HPP