#ifndef MOD_LIB_RC_COMPOSE_RULE_REAL_GENERIC_HPP
#define MOD_LIB_RC_COMPOSE_RULE_REAL_GENERIC_HPP

#include <mod/Config.hpp>
#include <mod/Misc.hpp>
#include <mod/lib/GraphMorphism/LabelledMorphism.hpp>
#include <mod/lib/RC/LabelledComposition.hpp>
#include <mod/lib/RC/Result.hpp>
#include <mod/lib/RC/IO/Write.hpp>
#include <mod/lib/RC/Visitor/MatchConstraints.hpp>
#include <mod/lib/RC/Visitor/String.hpp>
#include <mod/lib/RC/Visitor/Term.hpp>

#include <optional>

namespace mod::lib::RC {
struct Common;
struct Parallel;
struct Sub;
struct Super;

template<bool verbose, LabelType labelType, bool withStereo, typename InvertibleVertexMap>
std::optional<lib::Rules::LabelledRule> composeRuleRealByMatch(const lib::Rules::Real &rFirst,
                                                               const lib::Rules::Real &rSecond,
                                                               InvertibleVertexMap &match) {
	auto visitor = Visitor::MatchConstraints<labelType>(rFirst.getDPORule(), rSecond.getDPORule());
	LabelledResult result(rFirst.getDPORule().getRule(), rSecond.getDPORule().getRule());
	const bool success = composeLabelled<verbose, labelType, withStereo>(
			result, rFirst.getDPORule(), rSecond.getDPORule(), match, visitor);
	if(success) {
		assert(result.pString || result.pTerm);
		auto rule = result.pString
		            ? lib::Rules::LabelledRule(std::move(result.rDPO), std::move(result.pString),
		                                       std::move(result.pStereo))
		            : lib::Rules::LabelledRule(std::move(result.rDPO), std::move(result.pTerm),
		                                       std::move(result.pStereo));
		rule.leftData.matchConstraints = std::move(result.matchConstraints);
		return rule;
	} else return {};
}

template<LabelType labelType, bool withStereo, typename InvertibleVertexMap>
std::unique_ptr<lib::Rules::Real> composeRuleRealByMatch(const lib::Rules::Real &rFirst,
                                                         const lib::Rules::Real &rSecond,
                                                         InvertibleVertexMap &match,
                                                         const bool verbose, IO::Logger logger) {
	if(verbose) {
		logger.indent() << "Composing " << rFirst.getName() << " and " << rSecond.getName() << "\n";
		++logger.indentLevel;
	}
	auto resultOpt = verbose
	                 ? composeRuleRealByMatch<true, labelType, withStereo>(rFirst, rSecond, match)
	                 : composeRuleRealByMatch<false, labelType, withStereo>(rFirst, rSecond, match);
	if(!resultOpt) {
		if(verbose) logger.indent() << "Composition failed" << std::endl;
		return nullptr;
	}
	auto rResult = std::make_unique<lib::Rules::Real>(std::move(*resultOpt), labelType);
	if(verbose)
		logger.indent() << "Composition done, rNew is '" << rResult->getName() << "'" << std::endl;
	return rResult;
}

namespace detail {

struct MatchMakerCallback {
	MatchMakerCallback(std::function<bool(std::unique_ptr<lib::Rules::Real>)> rr) : rr(rr) {}

	template<typename InvertibleVertexMap>
	bool operator()(const lib::Rules::Real &rFirst,
	                const lib::Rules::Real &rSecond,
	                InvertibleVertexMap &&m,
	                bool verbose,
	                IO::Logger logger) const {
		using HasTerm = GraphMorphism::HasTermData<InvertibleVertexMap>;
		using HasStereo = GraphMorphism::HasStereoData<InvertibleVertexMap>;
		constexpr LabelType labelType = HasTerm::value ? LabelType::Term : LabelType::String;
		auto rResult = composeRuleRealByMatch<labelType, HasStereo::value>(rFirst, rSecond, m, verbose, logger);
		if(rResult) {
			if(verbose)
				logger.indent() << "RuleComp\t" << rResult->getName()
				                << "\t= " << rFirst.getName()
				                << "\t. " << rSecond.getName() << std::endl;
			if(getConfig().rc.printMatches.get()) {
				RC::Write::test(rFirst, rSecond, m, *rResult);
			}
			const bool cont = rr(std::move(rResult));
			if(!cont) return false;
		}
		return true;
	}
private:
	std::function<bool(std::unique_ptr<lib::Rules::Real>)> rr;
};

} // namespace detail

template<typename MatchMaker>
void composeRuleRealByMatchMakerGeneric(const lib::Rules::Real &rFirst,
                                        const lib::Rules::Real &rSecond,
                                        MatchMaker mm,
                                        std::function<bool(std::unique_ptr<lib::Rules::Real>)> rr,
                                        LabelSettings labelSettings) {
	if(getConfig().rc.printMatches.get())
		IO::post() << "summarySection \"RC Matches\"\n";
	mm.makeMatches(rFirst, rSecond, detail::MatchMakerCallback(rr), labelSettings);
}

} // namespace mod::lib::RC

#endif // MOD_LIB_RC_COMPOSE_RULE_REAL_GENERIC_HPP