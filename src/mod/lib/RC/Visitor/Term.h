#ifndef MOD_LIB_RC_VISITOR_TERM_H
#define MOD_LIB_RC_VISITOR_TERM_H

#include <mod/lib/GraphMorphism/TermVertexMap.h>
#include <mod/lib/IO/Term.h>
#include <mod/lib/RC/Visitor/Compound.h>
#include <mod/lib/Rules/Properties/Term.h>
#include <mod/lib/Term/WAM.h>

#include <mod/lib/IO/Term.h>

namespace mod {
namespace lib {
namespace RC {
namespace Visitor {
static constexpr std::size_t TERM_MAX = std::numeric_limits<std::size_t>::max();

struct Term {
	using Membership = jla_boost::GraphDPO::Membership;
	using AddressType = lib::Term::AddressType;
	using Cell = lib::Term::Cell;
	using CellTag = lib::Term::CellTag;
public:

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result>
	bool init(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, Result &result) {
		auto &data = get_prop(lib::GraphMorphism::TermDataT(), match);
		auto &machine = data.machine;
		machine.verify();
		result.rResult.pTerm = std::make_unique<typename Result::RuleResult::PropTermType > (get_graph(result.rResult), std::move(machine));
		if(Verbose) {
			lib::IO::log() << "New machine:\n";
			lib::IO::Term::Write::wam(getMachine(*result.rResult.pTerm), lib::Term::getStrings(), lib::IO::log());
		}
		return true;
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result>
	bool finalize(const RuleFirst &rFirst, const RuleSecond &rSecond, InvertibleVertexMap &match, Result &result) {
		result.rResult.pTerm ->verify(&get_graph(result.rResult));
		return true;
	}
public:

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexFirst, typename VertexResult>
	void copyVertexFirst(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const VertexFirst &vFirst, const VertexResult &vResult) {
		assert(result.rResult.pTerm);
		const auto &pFirst = *rFirst.pTerm;
		auto &pResult = *result.rResult.pTerm;
		auto m = membership(rFirst, vFirst);
		assert(m == membership(result.rResult, vResult));
		switch(m) {
		case Membership::Left:
			pResult.add(vResult, pFirst.getLeft()[vFirst], TERM_MAX);
			break;
		case Membership::Right:
			pResult.add(vResult, TERM_MAX, pFirst.getRight()[vFirst]);
			break;
		case Membership::Context:
			pResult.add(vResult, pFirst.getLeft()[vFirst], pFirst.getRight()[vFirst]);
			break;
		}
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexSecond, typename VertexResult>
	void copyVertexSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const VertexSecond &vSecond, const VertexResult &vResult) {
		assert(result.rResult.pTerm);
		const auto &pSecond = *rSecond.pTerm;
		auto &pResult = *result.rResult.pTerm;
		auto m = membership(rSecond, vSecond);
		assert(m == membership(result.rResult, vResult));
		if(m != Membership::Left) fixSecondTerm<Verbose>(pSecond.getRight()[vSecond], result);
		if(m != Membership::Right) fixSecondTerm<Verbose>(pSecond.getLeft()[vSecond], result);
		switch(m) {
		case Membership::Left:
			pResult.add(vResult, deref(pSecond.getLeft()[vSecond], result), TERM_MAX);
			break;
		case Membership::Right:
			pResult.add(vResult, TERM_MAX, deref(pSecond.getRight()[vSecond], result));
			break;
		case Membership::Context:
			pResult.add(vResult, deref(pSecond.getLeft()[vSecond], result), deref(pSecond.getRight()[vSecond], result));
			break;
		}
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename EdgeFirst, typename EdgeResult>
	void copyEdgeFirst(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const EdgeFirst &eFirst, const EdgeResult &eResult) {
		// the membership of e may be different from eResult
		assert(result.rResult.pTerm);
		const auto &pFirst = *rFirst.pTerm;
		auto &pResult = *result.rResult.pTerm;
		auto m = membership(result.rResult, eResult);
		switch(m) {
		case Membership::Left:
			pResult.add(eResult, pFirst.getLeft()[eFirst], TERM_MAX);
			break;
		case Membership::Right:
			pResult.add(eResult, TERM_MAX, pFirst.getRight()[eFirst]);
			break;
		case Membership::Context:
			pResult.add(eResult, pFirst.getLeft()[eFirst], pFirst.getRight()[eFirst]);
			break;
		}
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename EdgeSecond, typename EdgeResult>
	void copyEdgeSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			const EdgeSecond &eSecond, const EdgeResult &eResult) {
		// the membership of e may be different from eResult
		assert(result.rResult.pTerm);
		const auto &pSecond = *rSecond.pTerm;
		auto &pResult = *result.rResult.pTerm;
		auto m = membership(result.rResult, eResult);
		if(m != Membership::Left) fixSecondTerm<Verbose>(pSecond.getRight()[eSecond], result);
		if(m != Membership::Right) fixSecondTerm<Verbose>(pSecond.getLeft()[eSecond], result);
		switch(m) {
		case Membership::Left:
			pResult.add(eResult, deref(pSecond.getLeft()[eSecond], result), TERM_MAX);
			break;
		case Membership::Right:
			pResult.add(eResult, TERM_MAX, deref(pSecond.getRight()[eSecond], result));
			break;
		case Membership::Context:
			pResult.add(eResult, deref(pSecond.getLeft()[eSecond], result), deref(pSecond.getRight()[eSecond], result));
			break;
		}
	}
public:

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename VertexFirst>
	void printVertexFirst(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const VertexFirst &vFirst) {
		rFirst.pTerm->print(s, vFirst);
	}

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename VertexSecond>
	void printVertexSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const VertexSecond &vSecond) {
		rSecond.pTerm->print(s, vSecond);
	}

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename VertexResult>
	void printVertexResult(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const VertexResult &vResult) {
		result.rResult.pTerm->print(s, vResult);
	}

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename EdgeFirst>
	void printEdgeFirst(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const EdgeFirst &eFirst) {
		rFirst.pTerm->print(s, eFirst);
	}

	template<typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result, typename EdgeSecond>
	void printEdgeSecond(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			std::ostream &s, const EdgeSecond &eSecond) {
		rSecond.pTerm->print(s, eSecond);
	}
public:

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexResult, typename VertexSecond>
	void composeVertexRvsLR(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			VertexResult vResult, VertexSecond vSecond) {
		//   -> a | a -> b, maybe a == b
		auto addr = rSecond.pTerm->getRight()[vSecond];
		fixSecondTerm<Verbose>(addr, result);
		result.rResult.pTerm->setRight(vResult, deref(addr, result));
		//   -> b
	}

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename VertexResult, typename VertexSecond>
	void composeVertexLRvsL(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			VertexResult vResult, VertexSecond vSecond) {
		// vFirst is CONTEXT, so do nothing
		// a -> a | a ->
		// b -> a | a ->
		// to
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
		auto addr = rSecond.pTerm->getRight()[vSecond];
		fixSecondTerm<Verbose>(addr, result);
		result.rResult.pTerm->setRight(vResult, deref(addr, result));
	}
public:

	template<bool Verbose, typename RuleFirst, typename RuleSecond, typename InvertibleVertexMap, typename Result,
	typename EdgeResult, typename EdgeSecond>
	void setEdgeResultRightFromSecondRight(const RuleFirst &rFirst, const RuleSecond &rSecond, const InvertibleVertexMap &match, const Result &result,
			EdgeResult eResult, EdgeSecond eSecond) {
		auto addr = rSecond.pTerm->getRight()[eSecond];
		fixSecondTerm<Verbose>(addr, result);
		result.rResult.pTerm->setRight(eResult, deref(addr, result));
	}
private:

	template<bool Verbose, typename Result>
	void fixSecondTerm(std::size_t addr, Result &result) {
		auto &m = getMachine(*result.rResult.pTerm);
		m.verify();
		//			lib::IO::Term::Write::wam(m, lib::Term::getStrings(), lib::IO::log() << "Copy " << addr << "\n");
		m.copyFromTemp(addr);
		//			lib::IO::Term::Write::wam(m, lib::Term::getStrings(), lib::IO::log() << "After copy " << addr << "\n");
		m.verify();
	}

	template<typename Result>
	std::size_t deref(std::size_t addrTemp, Result &result) {
		getMachine(*result.rResult.pTerm).verify();
		auto addr = getMachine(*result.rResult.pTerm).deref({AddressType::Temp, addrTemp});
		assert(addr.type == AddressType::Heap);
		return addr.addr;
	}
};

} // namespace Composer
} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_VISITOR_TERM_H */
