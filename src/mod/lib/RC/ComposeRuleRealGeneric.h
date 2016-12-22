#ifndef MOD_LIB_RC_COMPOSE_RULE_REAL_GENERIC_H
#define MOD_LIB_RC_COMPOSE_RULE_REAL_GENERIC_H

#include <mod/Config.h>
#include <mod/Misc.h>
#include <mod/lib/IO/RC.h>
#include <mod/lib/IO/Rule.h>
#include <mod/lib/RC/LabelledComposition.h>
#include <mod/lib/RC/Visitor/MatchConstraints.h>
#include <mod/lib/RC/Visitor/String.h>

#include <boost/optional.hpp>

namespace mod {
namespace lib {
namespace RC {
struct Common;
struct Parallel;
struct Sub;
struct Super;

template<typename InvertibleVertexMap>
std::unique_ptr<lib::Rules::Real> composeRuleRealByMatch(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, InvertibleVertexMap &match) {
	if(getConfig().rc.verbose.get()) IO::log() << "Composing " << rFirst.getName() << " and " << rSecond.getName() << "\n" << std::string(80, '=') << std::endl;
	using Result = BaseResult<lib::Rules::LabelledRule, lib::Rules::LabelledRule, lib::Rules::LabelledRule>;
	auto visitor = Visitor::MatchConstraints();
	auto resultOpt = getConfig().rc.verbose.get()
			? composeLabelled<true, Result>(rFirst.getDPORule(), rSecond.getDPORule(), match, visitor)
			: composeLabelled<false, Result>(rFirst.getDPORule(), rSecond.getDPORule(), match, visitor);
	if(!resultOpt) {
		if(getConfig().rc.verbose.get()) IO::log() << "Composition failed" << std::endl;
		return nullptr;
	}
	resultOpt->rResult.initComponents(); // TODO: move to the visitor finalizer
	auto rResult = std::make_unique<lib::Rules::Real>(std::move(resultOpt->rResult));
	if(getConfig().rc.verbose.get()) IO::log() << "Composition done, rNew is '" << rResult->getName() << "'" << std::endl;
	return rResult;
}

namespace detail {

struct MatchMakerCallback {

	MatchMakerCallback(std::function<void(std::unique_ptr<lib::Rules::Real>) > rr) : rr(rr) { }

	template<typename InvertibleVertexMap>
	bool operator()(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, InvertibleVertexMap &&m) const {
		auto rResult = composeRuleRealByMatch(rFirst, rSecond, m);
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
	std::function<void(std::unique_ptr<lib::Rules::Real>) > rr;
};

} // namespace detail

template<typename MatchMaker>
void composeRuleRealByMatchMakerGeneric(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, MatchMaker mm, std::function<void(std::unique_ptr<lib::Rules::Real>) > rr) {
	if(getConfig().rc.printMatches.get()) {
		IO::post() << "summarySection \"RC Matches\"\n";
	}
	mm.makeMatches(rFirst, rSecond, detail::MatchMakerCallback(rr));
}

} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_COMPOSE_RULE_REAL_GENERIC_H */