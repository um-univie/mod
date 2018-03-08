#ifndef MOD_LIB_RC_VISITOR_STRING_H
#define MOD_LIB_RC_VISITOR_STRING_H

#include <mod/lib/RC/Visitor/Compound.h>

#include <mod/lib/Rules/Properties/String.h>

namespace mod {
namespace lib {
namespace RC {
namespace Visitor {

struct String {
	using Membership = jla_boost::GraphDPO::Membership;
public:

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result>
	bool init(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, Result &result) {
		result.rResult.pString = std::make_unique<typename Result::RuleResult::PropStringType > (get_graph(result.rResult));
		return true;
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result>
	bool finalize(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, Result &result) {
		result.rResult.pString->verify(&get_graph(result.rResult));
		return true;
	}
public:

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexFirst, typename VertexResult>
	void copyVertexFirst(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const VertexFirst &vFirst, const VertexResult &vResult) {
		copyVertex(rFirst, result.rResult, vFirst, vResult, *rFirst.pString);
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexSecond, typename VertexResult>
	void copyVertexSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const VertexSecond &vSecond, const VertexResult &vResult) {
		copyVertex(rSecond, result.rResult, vSecond, vResult, *rSecond.pString);
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename EdgeFirst, typename EdgeResult>
	void copyEdgeFirst(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const EdgeFirst &eFirst, const EdgeResult &eResult) {
		copyEdge(rFirst, result.rResult, eFirst, eResult, *rFirst.pString);
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename EdgeSecond, typename EdgeResult>
	void copyEdgeSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const EdgeSecond &eSecond, const EdgeResult &eResult) {
		copyEdge(rSecond, result.rResult, eSecond, eResult, *rSecond.pString);
	}
public:

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename VertexFirst>
	void printVertexFirst(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const VertexFirst &vFirst) {
		rFirst.pString->print(s, vFirst);
	}

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename VertexSecond>
	void printVertexSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const VertexSecond &vSecond) {
		rSecond.pString->print(s, vSecond);
	}

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename VertexResult>
	void printVertexResult(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const VertexResult &vResult) {
		result.rResult.pString->print(s, vResult);
	}

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename EdgeFirst>
	void printEdgeFirst(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const EdgeFirst &eFirst) {
		rFirst.pString->print(s, eFirst);
	}

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename EdgeSecond>
	void printEdgeSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const EdgeSecond &eSecond) {
		rSecond.pString->print(s, eSecond);
	}
public:

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexResult, typename VertexSecond>
	void composeVertexRvsLR(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			VertexResult vResult, VertexSecond vSecond) {
		//   -> a | a -> b, maybe a == b
		result.rResult.pString->setRight(vResult, rSecond.pString->getRight()[vSecond]);
		//   -> b
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexResult, typename VertexSecond>
	void composeVertexLRvsL(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			VertexResult vResult, VertexSecond vSecond) {
		// vFirst is CONTEXT, so do nothing
		// a -> a | a ->
		// b -> a | a ->
		//		stateNew->setLeft(vNew, stateNew->getLeft()[vNew]);
		// a ->
		// b ->
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexResult, typename VertexSecond>
	void composeVertexLRvsLR(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			VertexResult vResult, VertexSecond vSecond) {
		// the left label of vResult is ok, but the right label might have to change
		// a != b, a != c, b =? c
		// a -> a | a -> a
		// a -> a | a -> c
		// b -> a | a -> a
		// b -> a | a -> c
		result.rResult.pString->setRight(vResult, rSecond.pString->getRight()[vSecond]);
	}
public:

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename EdgeResult, typename EdgeSecond>
	void setEdgeResultRightFromSecondRight(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			EdgeResult eResult, EdgeSecond eSecond) {
		result.rResult.pString->setRight(eResult, rSecond.pString->getRight()[eSecond]);
	}
private:

	template<typename RuleFrom, typename RuleResult, typename VertexFrom, typename VertexResult, typename Prop>
	void copyVertex(const RuleFrom &rFrom, const RuleResult &rResult, VertexFrom vFrom, VertexResult vResult, const Prop &pFrom) {
		assert(rResult.pString);
		auto &pString = *rResult.pString;
		auto m = membership(rFrom, vFrom);
		assert(m == membership(rResult, vResult));
		switch(m) {
		case Membership::Left:
			pString.add(vResult, pFrom.getLeft()[vFrom], "");
			break;
		case Membership::Right:
			pString.add(vResult, "", pFrom.getRight()[vFrom]);
			break;
		case Membership::Context:
			pString.add(vResult, pFrom.getLeft()[vFrom], pFrom.getRight()[vFrom]);
			break;
		}
	}

	template<typename RuleFrom, typename RuleResult, typename EdgeFrom, typename EdgeResult, typename Prop>
	void copyEdge(const RuleFrom &rFrom, const RuleResult &rResult, EdgeFrom eFrom, EdgeResult eResult, const Prop &pFrom) {
		// the membership of eFrom may be different from eResult
		assert(rResult.pString);
		auto &pString = *rResult.pString;
		auto m = membership(rResult, eResult);
		switch(m) {
		case Membership::Left:
			pString.add(eResult, pFrom.getLeft()[eFrom], "");
			break;
		case Membership::Right:
			pString.add(eResult, "", pFrom.getRight()[eFrom]);
			break;
		case Membership::Context:
			pString.add(eResult, pFrom.getLeft()[eFrom], pFrom.getRight()[eFrom]);
			break;
		}
	}
};

} // namespace Composer
} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_VISITOR_STRING_H */
