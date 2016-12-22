#ifndef MOD_LIB_RC_VISITOR_COMPOUND_H
#define	MOD_LIB_RC_VISITOR_COMPOUND_H

#include <mod/lib/RC/Result.h>
#include <mod/lib/Rules/Real.h>

#include <cassert>

namespace mod {
namespace lib {
namespace RC {
namespace Visitor {

template<typename ...Composers>
struct Compound {
};

template<>
struct Compound<> {

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result>
	bool init(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, Result &result) {
		return true;
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result>
	bool finalize(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, Result &result) {
		return true;
	}
public:

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexFirst, typename VertexResult>
	void copyVertexFirst(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const VertexFirst &vFirst, const VertexResult &vResult) { }

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexSecond, typename VertexResult>
	void copyVertexSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const VertexSecond &vSecond, const VertexResult &vResult) { }

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename EdgeFirst, typename EdgeResult>
	void copyEdgeFirst(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const EdgeFirst &eFirst, const EdgeResult &eResult) { }

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename EdgeSecond, typename EdgeResult>
	void copyEdgeSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const EdgeSecond &eSecond, const EdgeResult &eResult) { }
public:

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename VertexSecond>
	void printVertexSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const VertexSecond &vSecond) { }

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename VertexResult>
	void printVertexResult(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const VertexResult &vResult) { }

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename EdgeFirst>
	void printEdgeFirst(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const EdgeFirst &eFirst) { }

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename EdgeSecond>
	void printEdgeSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const EdgeSecond &eSecond) { }
public:

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexResult, typename VertexSecond>
	void composeVertexRvsLR(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			VertexResult vResult, VertexSecond vSecond) { }

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexResult, typename VertexSecond>
	void composeVertexLRvsL(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			VertexResult vResult, VertexSecond vSecond) { }

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexResult, typename VertexSecond>
	void composeVertexLRvsLR(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			VertexResult vResult, VertexSecond vSecond) { }
public:

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename EdgeResult, typename EdgeSecond>
	void setEdgeResultRightFromSecondRight(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			EdgeResult eResult, EdgeSecond eSecond) { }
};

template<typename Visitor, typename ...Visitors>
struct Compound<Visitor, Visitors...> : Compound<Visitors...> {
	using Base = Compound<Visitors...>;

	Compound(Visitor visitor, Visitors ...visitors)
	: Base(visitors...), visitor(visitor) { }

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result>
	bool init(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, Result &result) {
		bool res = visitor.template init<Verbose>(rFirst, rSecond, match, result);
		return res && Base::template init<Verbose>(rFirst, rSecond, match, result);
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result>
	bool finalize(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, Result &result) {
		bool res = visitor.template finalize<Verbose>(rFirst, rSecond, match, result);
		return res && Base::template finalize<Verbose>(rFirst, rSecond, match, result);
	}
public:

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexFirst, typename VertexResult>
	void copyVertexFirst(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const VertexFirst &vFirst, const VertexResult &vResult) {
		visitor.template copyVertexFirst<Verbose>(rFirst, rSecond, match, result, vFirst, vResult);
		Base::template copyVertexFirst<Verbose>(rFirst, rSecond, match, result, vFirst, vResult);
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexSecond, typename VertexResult>
	void copyVertexSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const VertexSecond &vSecond, const VertexResult &vResult) {
		visitor.template copyVertexSecond<Verbose>(rFirst, rSecond, match, result, vSecond, vResult);
		Base::template copyVertexSecond<Verbose>(rFirst, rSecond, match, result, vSecond, vResult);
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename EdgeFirst, typename EdgeResult>
	void copyEdgeFirst(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const EdgeFirst &eFirst, const EdgeResult &eResult) {
		visitor.template copyEdgeFirst<Verbose>(rFirst, rSecond, match, result, eFirst, eResult);
		Base::template copyEdgeFirst<Verbose>(rFirst, rSecond, match, result, eFirst, eResult);
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename EdgeSecond, typename EdgeResult>
	void copyEdgeSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const EdgeSecond &eSecond, const EdgeResult &eResult) {
		visitor.template copyEdgeSecond<Verbose>(rFirst, rSecond, match, result, eSecond, eResult);
		Base::template copyEdgeSecond<Verbose>(rFirst, rSecond, match, result, eSecond, eResult);
	}
public:

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename VertexSecond>
	void printVertexSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const VertexSecond &vSecond) {
		visitor.printVertexSecond(rFirst, rSecond, match, result, s, vSecond);
		Base::template printVertexSecond(rFirst, rSecond, match, result, s, vSecond);
	}

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename VertexResult>
	void printVertexResult(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const VertexResult &vResult) {
		visitor.printVertexResult(rFirst, rSecond, match, result, s, vResult);
		Base::template printVertexResult(rFirst, rSecond, match, result, s, vResult);
	}

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename EdgeFirst>
	void printEdgeFirst(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const EdgeFirst &eFirst) {
		visitor.printEdgeFirst(rFirst, rSecond, match, result, s, eFirst);
		Base::template printEdgeFirst(rFirst, rSecond, match, result, s, eFirst);
	}

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename EdgeSecond>
	void printEdgeSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const EdgeSecond &eSecond) {
		visitor.printEdgeSecond(rFirst, rSecond, match, result, s, eSecond);
		Base::template printEdgeSecond(rFirst, rSecond, match, result, s, eSecond);
	}
public:

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexResult, typename VertexSecond>
	void composeVertexRvsLR(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			VertexResult vResult, VertexSecond vSecond) {
		visitor.template composeVertexRvsLR<Verbose>(rFirst, rSecond, match, result, vResult, vSecond);
		Base::template composeVertexRvsLR<Verbose>(rFirst, rSecond, match, result, vResult, vSecond);
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexResult, typename VertexSecond>
	void composeVertexLRvsL(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			VertexResult vResult, VertexSecond vSecond) {
		visitor.template composeVertexLRvsL<Verbose>(rFirst, rSecond, match, result, vResult, vSecond);
		Base::template composeVertexLRvsL<Verbose>(rFirst, rSecond, match, result, vResult, vSecond);
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexResult, typename VertexSecond>
	void composeVertexLRvsLR(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			VertexResult vResult, VertexSecond vSecond) {
		visitor.template composeVertexLRvsLR<Verbose>(rFirst, rSecond, match, result, vResult, vSecond);
		Base::template composeVertexLRvsLR<Verbose>(rFirst, rSecond, match, result, vResult, vSecond);
	}
public:

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename EdgeResult, typename EdgeSecond>
	void setEdgeResultRightFromSecondRight(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			EdgeResult eResult, EdgeSecond eSecond) {
		visitor.template setEdgeResultRightFromSecondRight<Verbose>(rFirst, rSecond, match, result, eResult, eSecond);
		Base::template setEdgeResultRightFromSecondRight<Verbose>(rFirst, rSecond, match, result, eResult, eSecond);
	}
private:
	Visitor visitor;
};


using Null = Compound<>;

template<typename ...Visitors>
Compound<Visitors...> makeVisitor(Visitors ...visitors) {
	return Compound < Visitors...>(visitors...);
}

} // namespace Visitor
} // namespace RC
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_RC_VISITOR_COMPOUND_H */