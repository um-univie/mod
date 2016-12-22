#ifndef MOD_LIB_RC_VISITOR_MATCH_CONSTRAINTS_H
#define MOD_LIB_RC_VISITOR_MATCH_CONSTRAINTS_H

#include <mod/lib/RC/Visitor/Compound.h>

namespace mod {
namespace lib {
namespace RC {
namespace Visitor {

struct MatchConstraints : Null {
	template<typename G>
	using ConstraintVisitor = lib::GraphMorphism::MatchConstraint::Visitor<G, void>;
	template<typename G>
	using Constraint = lib::GraphMorphism::MatchConstraint::Constraint<G>;
	template<typename G>
	using ConstraintAdj = lib::GraphMorphism::MatchConstraint::VertexAdjacency<G>;
	template<typename G>
	using ConstraintSP = lib::GraphMorphism::MatchConstraint::ShortestPath<G>;

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result>
	bool finalize(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, Result &result) {
		if(!getConfig().rc.composeConstraints.get()) return true;
		using GraphResult = typename jla_boost::GraphDPO::PushoutRuleTraits<typename Result::RuleResult>::GraphType;
		using GraphFirst = typename jla_boost::GraphDPO::PushoutRuleTraits<RuleFirst>::GraphType;
		using GraphSecond = typename jla_boost::GraphDPO::PushoutRuleTraits<RuleSecond>::GraphType;
		
		using GraphResultLeft = typename jla_boost::GraphDPO::PushoutRuleTraits<typename Result::RuleResult>::LeftGraphType;
		using GraphFirstLeft = typename jla_boost::GraphDPO::PushoutRuleTraits<RuleFirst>::LeftGraphType;
		using GraphSecondLeft = typename jla_boost::GraphDPO::PushoutRuleTraits<RuleSecond>::LeftGraphType;

		struct ConvertFirst : public ConstraintVisitor<GraphFirstLeft> {

			ConvertFirst(const RuleFirst &rFirst, Result &result) : rFirst(rFirst), result(result) { }

			void operator()(const ConstraintAdj<GraphFirstLeft> &c) const {
				auto cResult = std::make_unique<ConstraintAdj<GraphResultLeft> >(c);
				auto vFirst = cResult->vConstrained;
				auto vResult = get(result.mFirstToResult, get_graph(rFirst), get_graph(result.rResult), vFirst);
				assert(vResult != boost::graph_traits<GraphResult>::null_vertex());
				cResult->vConstrained = vResult;
				// IO::log() << "WARNING: check transfered constraint for " << rFirst.getName() << " -> " << rSecond.getName() << std::endl;
				this->cResult = std::move(cResult);
			}

			void operator()(const ConstraintSP<GraphFirstLeft> &c) const {
				auto cResult = std::make_unique<ConstraintSP<GraphResultLeft> >(c);
				MOD_ABORT;
			}
		public:
			const RuleFirst &rFirst;
			Result &result;
			mutable std::unique_ptr<Constraint<GraphResultLeft> > cResult;
		};

		struct ConvertSecond : public ConstraintVisitor<GraphSecondLeft> {

			ConvertSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, Result &result)
			: rFirst(rFirst), rSecond(rSecond), match(match), result(result) { }

			void operator()(const ConstraintAdj<GraphSecondLeft> &c) const {
				auto vSecond = c.vConstrained;
				auto vResult = get(result.mSecondToResult, get_graph(rSecond), get_graph(result.rResult), vSecond);
				// vResult may be null_vertex
				if(vResult == boost::graph_traits<GraphResult>::null_vertex()) {
					// IO::log() << "WARNING: constrained vertex " << vSecondId << " deleted in " << rFirst.getName() << " -> " << rSecond.getName() << std::endl;
					return;
				}

				auto mResult = membership(result.rResult, vResult);
				if(mResult == jla_boost::GraphDPO::Membership::Right) {
					// IO::log() << "WARNING: constrained vertex " << vSecondId << " changed to right side in " << rFirst.getName() << " -> " << rSecond.getName() << std::endl;
					return;
				}

				if(get(match, get_graph(rSecond), get_graph(rFirst), vSecond) != boost::graph_traits<GraphFirst>::null_vertex()) {
					// IO::log() << "WARNING: maybe missing constraint on " << vNew << " for " << rFirst.getName() << " -> " << rSecond.getName() << std::endl;
					return;
				}

				auto cResult = std::make_unique<ConstraintAdj<GraphResultLeft> >(c);
				cResult->vConstrained = vResult;
				this->cResult = std::move(cResult);
				// IO::log() << "WARNING: check converted constraint on vertex " << vNew << " for " << rFirst.getName() << " -> " << rSecond.getName() << std::endl;
			}

			void operator()(const ConstraintSP<GraphSecondLeft> &c) const {
				auto vResultSrc = get(result.mSecondToResult, get_graph(rSecond), get_graph(result.rResult), c.vSrc);
				auto vResultTar = get(result.mSecondToResult, get_graph(rSecond), get_graph(result.rResult), c.vTar);
				auto isSrcDeleted = vResultSrc == boost::graph_traits<GraphResult>::null_vertex();
				auto isTarDeleted = vResultTar == boost::graph_traits<GraphResult>::null_vertex();
				if(isSrcDeleted && isTarDeleted) {
					MOD_ABORT;
				}
				if(isSrcDeleted || isTarDeleted) {
					MOD_ABORT;
				}

				auto isInRightSrc = membership(result.rResult, vResultSrc) == jla_boost::GraphDPO::Membership::Right;
				auto isInRightTar = membership(result.rResult, vResultTar) == jla_boost::GraphDPO::Membership::Right;
				if(isInRightSrc || isInRightTar) return; // must have been checked

				auto cResult = std::make_unique<ConstraintSP<GraphResultLeft> >(c);
				cResult->vSrc = vResultSrc;
				cResult->vTar = vResultTar;
				this->cResult = std::move(cResult);
			}
		public:
			const RuleFirst &rFirst;
			const RuleSecond &rSecond;
			InvertibleVertexMap &match;
			Result &result;
			mutable std::unique_ptr<Constraint<GraphResultLeft> > cResult;
		};
		for(const auto &cFirst : rFirst.leftComponentMatchConstraints) {
			ConvertFirst visitor(rFirst, result);
			cFirst->visit(visitor);
			assert(visitor.cResult);
			result.rResult.leftComponentMatchConstraints.push_back(std::move(visitor.cResult));
		}
		for(const auto &cSecond : rSecond.leftComponentMatchConstraints) {
			ConvertSecond visitor(rFirst, rSecond, match, result);
			cSecond->visit(visitor);
			if(visitor.cResult)
				result.rResult.leftComponentMatchConstraints.push_back(std::move(visitor.cResult));
		}
		return true;
	}
};

} // namespace Visitor
} // namespace RC
} // namespace lib
} // namespace mod


#endif /* MOD_LIB_RC_VISITOR_MATCH_CONSTRAINTS_H */