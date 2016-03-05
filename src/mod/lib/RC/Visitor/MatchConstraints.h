#ifndef MOD_LIB_RC_VISITOR_MATCH_CONSTRAINTS_H
#define	MOD_LIB_RC_VISITOR_MATCH_CONSTRAINTS_H

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

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result>
	bool finalize(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, Result &result) {
		if(!getConfig().rc.composeConstraints.get()) return true;
		using GraphResult = typename jla_boost::GraphDPO::PushoutRuleTraits<typename Result::RuleResult>::GraphType;
		using GraphFirst = typename jla_boost::GraphDPO::PushoutRuleTraits<RuleFirst>::GraphType;
		using GraphSecond = typename jla_boost::GraphDPO::PushoutRuleTraits<RuleSecond>::GraphType;

		struct ConvertFirst : public ConstraintVisitor<GraphFirst> {

			ConvertFirst(const RuleFirst &rFirst, Result &result) : rFirst(rFirst), result(result) { }

			void operator()(const ConstraintAdj<GraphFirst> &c) const {
				auto cResult = make_unique<ConstraintAdj<GraphResult> >(c);
				auto vFirst = cResult->vConstrained;
				auto vResult = get(result.mFirstToResult, get_graph(rFirst), get_graph(result.rResult), vFirst);
				assert(vResult != boost::graph_traits<GraphResult>::null_vertex());
				cResult->vConstrained = vResult;
				// IO::log() << "WARNING: check transfered constraint for " << rFirst.getName() << " -> " << rSecond.getName() << std::endl;
				this->cResult = std::move(cResult);
			}
		public:
			const RuleFirst &rFirst;
			Result &result;
			mutable std::unique_ptr<Constraint<GraphResult> > cResult;
		};

		struct ConvertSecond : public ConstraintVisitor<GraphSecond> {

			ConvertSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, Result &result)
			: rFirst(rFirst), rSecond(rSecond), match(match), result(result) { }

			void operator()(const ConstraintAdj<GraphSecond> &c) const {
				auto vSecond = c.vConstrained;
				auto vResult = get(result.mSecondToResult, get_graph(rSecond), get_graph(result.rResult), vSecond);
				// vResult may be null_vertex
				if(vResult == boost::graph_traits<GraphSecond>::null_vertex()) {
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

				auto cResult = make_unique<ConstraintAdj<GraphResult> >(c);
				cResult->vConstrained = vResult;
				this->cResult = std::move(cResult);
				// IO::log() << "WARNING: check converted constraint on vertex " << vNew << " for " << rFirst.getName() << " -> " << rSecond.getName() << std::endl;
			}
		public:
			const RuleFirst &rFirst;
			const RuleSecond &rSecond;
			InvertibleVertexMap &match;
			Result &result;
			mutable std::unique_ptr<Constraint<GraphResult> > cResult;
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


#endif	/* MOD_LIB_RC_VISITOR_MATCH_CONSTRAINTS_H */