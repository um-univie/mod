#ifndef MOD_LIB_RULES_GRAPHASRULECACHE_HPP
#define MOD_LIB_RULES_GRAPHASRULECACHE_HPP

#include <mod/rule/ForwardDecl.hpp>
#include <mod/lib/DPO/Membership.hpp>

#include <map>
#include <memory>

namespace mod::lib::Graph {
struct Single;
} // namespace mod::lib::Graph
namespace mod::lib::Rules {

struct GraphAsRuleCache {
	std::shared_ptr<rule::Rule> getBindRule(const lib::Graph::Single *g);
	std::shared_ptr<rule::Rule> getIdRule(const lib::Graph::Single *g);
	std::shared_ptr<rule::Rule> getUnbindRule(const lib::Graph::Single *g);
private:
	std::shared_ptr<rule::Rule> getRule(const lib::Graph::Single *g, lib::DPO::Membership m);
private:
	std::map<std::pair<const lib::Graph::Single *, lib::DPO::Membership>, std::shared_ptr<rule::Rule>> storage;
};

} // namespace mod::lib::Rules

#endif // MOD_LIB_RULES_GRAPHASRULECACHE_HPP
