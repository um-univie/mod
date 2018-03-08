#ifndef MOD_LIB_RC_VISITOR_MATCH_CONSTRAINTS_H
#define MOD_LIB_RC_VISITOR_MATCH_CONSTRAINTS_H

#include <mod/lib/GraphMorphism/Constraints/AllVisitor.h>
#include <mod/lib/RC/Visitor/Compound.h>

namespace mod {
namespace lib {
namespace RC {
namespace Visitor {

template<LabelType labelType>
struct MatchConstraints : Null {
	template<typename G>
	using ConstraintVisitor = lib::GraphMorphism::Constraints::AllVisitor<G>;
	template<typename G>
	using Constraint = lib::GraphMorphism::Constraints::Constraint<G>;
	template<typename G>
	using ConstraintAdj = lib::GraphMorphism::Constraints::VertexAdjacency<G>;
	template<typename G>
	using ConstraintSP = lib::GraphMorphism::Constraints::ShortestPath<G>;

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

			virtual void operator()(const ConstraintAdj<GraphFirstLeft> &c) override {
				auto cResult = std::make_unique<ConstraintAdj<GraphResultLeft> >(c);
				switch(labelType) {
				case LabelType::String:
					cResult->vertexTerms.clear();
					cResult->edgeTerms.clear();
					break;
				case LabelType::Term:
					cResult->vertexLabels.clear();
					cResult->edgeLabels.clear();
					break;
				}
				auto vFirst = cResult->vConstrained;
				auto vResult = get(result.mFirstToResult, get_graph(rFirst), get_graph(result.rResult), vFirst);
				assert(vResult != boost::graph_traits<GraphResult>::null_vertex());
				cResult->vConstrained = vResult;
				// IO::log() << "WARNING: check transfered constraint for " << rFirst.getName() << " -> " << rSecond.getName() << std::endl;
				this->cResult = std::move(cResult);
			}

			virtual void operator()(const ConstraintSP<GraphFirstLeft> &c) override {
				auto cResult = std::make_unique<ConstraintSP<GraphResultLeft> >(c);
				MOD_ABORT;
			}
		public:
			const RuleFirst &rFirst;
			Result &result;
			std::unique_ptr<Constraint<GraphResultLeft> > cResult;
		};

		struct ConvertSecond : public ConstraintVisitor<GraphSecondLeft> {

			ConvertSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, Result &result)
			: rFirst(rFirst), rSecond(rSecond), match(match), result(result) { }

			virtual void operator()(const ConstraintAdj<GraphSecondLeft> &c) override {
				const auto vSecond = c.vConstrained;
				const auto vResult = get(result.mSecondToResult, get_graph(rSecond), get_graph(result.rResult), vSecond);
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

				if(get(match, get_graph(get_labelled_left(rSecond)), get_graph(get_labelled_right(rFirst)), vSecond) != boost::graph_traits<GraphFirst>::null_vertex()) {
					// IO::log() << "WARNING: maybe missing constraint on " << vNew << " for " << rFirst.getName() << " -> " << rSecond.getName() << std::endl;
					return;
				}

				auto cResult = std::make_unique<ConstraintAdj<GraphResultLeft> >(c);
				cResult->vConstrained = vResult;
				switch(labelType) {
				case LabelType::String:
					cResult->vertexTerms.clear();
					cResult->edgeTerms.clear();
					break;
				case LabelType::Term:
				{
					cResult->vertexLabels.clear();
					cResult->edgeLabels.clear();
					// we need to make sure the terms referred to are actually in the result machine,
					// and they need to be correct
					auto vTerms = std::move(cResult->vertexTerms);
					auto eTerms = std::move(cResult->edgeTerms);
					cResult->vertexTerms.clear();
					cResult->edgeTerms.clear();
					auto &m = getMachine(*result.rResult.pTerm);
					const auto handleTerm = [&m](const auto tSecond) {
						m.verify();
						//			lib::IO::Term::Write::wam(m, lib::Term::getStrings(), lib::IO::log() << "Copy " << addr << "\n");
						auto res = m.copyFromTemp(tSecond);
						//			lib::IO::Term::Write::wam(m, lib::Term::getStrings(), lib::IO::log() << "After copy " << addr << "\n");
						m.verify();
						return res.addr;
					};
					for(const auto tSecond : vTerms) cResult->vertexTerms.insert(handleTerm(tSecond));
					for(const auto tSecond : eTerms) cResult->edgeTerms.insert(handleTerm(tSecond));
					break;
				}
				}
				this->cResult = std::move(cResult);
				// IO::log() << "WARNING: check converted constraint on vertex " << vNew << " for " << rFirst.getName() << " -> " << rSecond.getName() << std::endl;
			}

			virtual void operator()(const ConstraintSP<GraphSecondLeft> &c) override {
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
		for(const auto &cFirst : rFirst.leftMatchConstraints) {
			ConvertFirst visitor(rFirst, result);
			cFirst->accept(visitor);
			assert(visitor.cResult);
			result.rResult.leftMatchConstraints.push_back(std::move(visitor.cResult));
		}
		for(const auto &cSecond : rSecond.leftMatchConstraints) {
			ConvertSecond visitor(rFirst, rSecond, match, result);
			cSecond->accept(visitor);
			if(visitor.cResult)
				result.rResult.leftMatchConstraints.push_back(std::move(visitor.cResult));
		}
		return true;
	}
};

} // namespace Visitor
} // namespace RC
} // namespace lib
} // namespace mod


#endif /* MOD_LIB_RC_VISITOR_MATCH_CONSTRAINTS_H */