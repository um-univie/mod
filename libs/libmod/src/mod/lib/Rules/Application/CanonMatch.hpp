#ifndef MOD_LIB_RULES_APPLICATION_CANON_MATCH_HPP
#define MOD_LIB_RULES_APPLICATION_CANON_MATCH_HPP

#include <mod/lib/Rules/Application/ComponentMatch.hpp>
#include <mod/lib/Rules/Real.hpp>
#include <mod/lib/Graph/Single.hpp>

namespace mod::lib::Rules::Application {

bool isCanonComponentMatch(const Real& rule,
                           size_t compIndex,
                           const Graph::LabelledGraph& host,
                           const ComponentMatch::Morphism& match);

bool isCanonComponentMatchFast(const Real& rule,
                           size_t compIndex,
                           const Graph::LabelledGraph& host,
                           const ComponentMatch::Morphism& match);

class CanonMatch {
public:
	CanonMatch(const std::vector<const Graph::Single *>& hosts, const Rules::Real& rule,
	           LabelSettings ls);
	bool push(const ComponentMatch& cm, size_t hostIndex);
	bool pushFast(const ComponentMatch& cm, size_t hostIndex);

	void pop();
	void popFast();
private:
	const std::vector<const Graph::Single *>& hosts;
	const Rules::Real& rule;
	struct PushedFixes {
		size_t hid;
		size_t oldSize;
	};
	std::vector<std::vector<int>> hostFixes;
	std::vector<PushedFixes> pushedStack;
	LabelSettings labelSettings;

	std::vector<std::vector<Graph::Single::AutGroup::const_pointer>> auts;
	std::vector<size_t> autSize;

};

} //mod::lib::Rules::Application




#endif
