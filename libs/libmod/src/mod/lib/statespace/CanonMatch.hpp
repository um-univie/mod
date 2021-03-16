#ifndef MOD_LIB_STATESPACE_CANON_MATCH_HPP
#define MOD_LIB_STATESPACE_CANON_MATCH_HPP

#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/statespace/ComponentMap.hpp>

namespace mod::lib::Rules {
struct Real;
struct GraphAsRuleCache;
} // mod::lib::Rules
namespace mod::lib::statespace {

class CanonMatch {
public:
	CanonMatch(const std::vector<const Graph::Single *>& hosts, const Rules::Real& rule,
	           LabelSettings ls);
	bool push(const ComponentMatch& cm, lib::Rules::GraphAsRuleCache &graphAsRule);
	void pop();
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


};

}

#endif
