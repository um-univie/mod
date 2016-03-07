#ifndef MOD_LIB_RC_CORE_H
#define MOD_LIB_RC_CORE_H

#include <mod/Config.h>
#include <mod/Misc.h>
#include <mod/lib/IO/RC.h>
#include <mod/lib/IO/Rule.h>
#include <mod/lib/RC/LabelledComposition.h>
#include <mod/lib/RC/MatchMaker/MatchMaker.h>
#include <mod/lib/RC/Visitor/MatchConstraints.h>
#include <mod/lib/RC/Visitor/String.h>

#include <jla_boost/Memory.hpp>

#include <boost/optional/optional.hpp>

namespace mod {
namespace lib {
namespace Term {
struct Wam;
} // namespace Term
namespace RC {

template<typename InvertibleVertexMap>
std::unique_ptr<lib::Rule::Real> composeRules(const Rule::Real &rFirst, const Rule::Real &rSecond, InvertibleVertexMap &match) {
	if(getConfig().rc.verbose.get()) IO::log() << "Composing " << rFirst.getName() << " and " << rSecond.getName() << "\n" << std::string('=', 80) << std::endl;
	using Result = BaseResult<lib::Rule::LabelledRule, lib::Rule::LabelledRule, lib::Rule::LabelledRule>;
	auto visitor = Visitor::MatchConstraints();
	auto resultOpt = getConfig().rc.verbose.get()
			? composeLabelled<true, Result>(rFirst.getDPORule(), rSecond.getDPORule(), match, visitor)
			: composeLabelled<false, Result>(rFirst.getDPORule(), rSecond.getDPORule(), match, visitor);
	if(!resultOpt) {
		if(getConfig().rc.verbose.get()) IO::log() << "Composition failed" << std::endl;
		return nullptr;
	}
	auto rResult = make_unique<lib::Rule::Real>(std::move(resultOpt->rResult));
	if(getConfig().rc.verbose.get()) IO::log() << "Composition done, rNew is '" << rResult->getName() << "'" << std::endl;
	return rResult;
}

namespace detail {

struct MatchMakerCallback {

	MatchMakerCallback(std::function<void(std::unique_ptr<lib::Rule::Real>) > rr) : rr(rr) { }

	template<typename InvertibleVertexMap>
	bool operator()(const lib::Rule::Real &rFirst, const lib::Rule::Real &rSecond, InvertibleVertexMap &&m) const {
		auto rResult = composeRules(rFirst, rSecond, m);
		if(rResult) {
			if(getConfig().rc.verbose.get())
				IO::log() << "RuleComp\t" << rResult->getName()
				<< "\t= " << rFirst.getName()
				<< "\t. " << rSecond.getName() << std::endl;
			if(getConfig().rc.printMatches.get()) {
				IO::RC::Write::test(rFirst, rSecond, m, *rResult);
			}
			rr(std::move(rResult));
		}
		return true;
	}
private:
	std::function<void(std::unique_ptr<lib::Rule::Real>) > rr;
};

} // namespace detail

template<typename MatchMaker>
void composeRules(const Rule::Real &rFirst, const Rule::Real &rSecond, MatchMaker mm, std::function<void(std::unique_ptr<lib::Rule::Real>) > rr) {
	if(getConfig().rc.printMatches.get()) {
		IO::post() << "summarySection \"RC Matches\"\n";
	}
	mm.makeMatches(rFirst, rSecond, detail::MatchMakerCallback(rr));
}

} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_CORE_H */