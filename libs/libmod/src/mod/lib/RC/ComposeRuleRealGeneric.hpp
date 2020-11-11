#ifndef MOD_LIB_RC_COMPOSE_RULE_REAL_GENERIC_H
#define MOD_LIB_RC_COMPOSE_RULE_REAL_GENERIC_H

#include <mod/Config.hpp>
#include <mod/Misc.hpp>
#include <mod/lib/GraphMorphism/LabelledMorphism.hpp>
#include <mod/lib/IO/RC.hpp>
#include <mod/lib/IO/Rule.hpp>
#include <mod/lib/RC/LabelledComposition.hpp>
#include <mod/lib/RC/Visitor/MatchConstraints.hpp>
#include <mod/lib/RC/Visitor/String.hpp>
#include <mod/lib/RC/Visitor/Term.hpp>

#include <boost/optional.hpp>

namespace mod {
namespace lib {
namespace RC {
struct Common;
struct Parallel;
struct Sub;
struct Super;

template<bool verbose, LabelType labelType, bool withStereo, typename InvertibleVertexMap>
auto composeRuleRealByMatch(const lib::Rules::Real &rFirst,
                            const lib::Rules::Real &rSecond,
                            InvertibleVertexMap &match,
                            const std::vector<size_t> *copyVertices) {
	using Result = BaseResult<lib::Rules::LabelledRule, lib::Rules::LabelledRule, lib::Rules::LabelledRule>;
	auto visitor = Visitor::MatchConstraints<labelType>();
	auto res = composeLabelled<verbose, Result, labelType, withStereo>(
	        rFirst.getDPORule(), rSecond.getDPORule(), match, visitor, copyVertices);
	if(res) res->rResult.initComponents(); // TODO: move to the visitor finalizer
	return res;
}

template<LabelType labelType, bool withStereo, typename InvertibleVertexMap>
std::unique_ptr<lib::Rules::Real> composeRuleRealByMatch(const lib::Rules::Real &rFirst,
                                                         const lib::Rules::Real &rSecond,
                                                         InvertibleVertexMap &match,
                                                         const bool verbose, IO::Logger logger,
                                                         const std::vector<size_t> *copyVertices = nullptr) {
	if(verbose) {
		logger.indent() << "Composing " << rFirst.getName() << " and " << rSecond.getName() << "\n";
		++logger.indentLevel;
	}
	auto resultOpt = verbose
	                 ? composeRuleRealByMatch<true, labelType, withStereo>(rFirst, rSecond, match, copyVertices)
	                 : composeRuleRealByMatch<false, labelType, withStereo>(rFirst, rSecond, match, copyVertices);
	if(!resultOpt) {
		if(verbose) logger.indent() << "Composition failed" << std::endl;
		return nullptr;
	}
	auto rResult = std::make_unique<lib::Rules::Real>(std::move(resultOpt->rResult), labelType);
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
				IO::RC::Write::test(rFirst, rSecond, m, *rResult);
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

} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_COMPOSE_RULE_REAL_GENERIC_H */
