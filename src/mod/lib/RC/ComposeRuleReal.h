#ifndef MOD_LIB_RC_COMPOSE_RULE_REAL_H
#define MOD_LIB_RC_COMPOSE_RULE_REAL_H

#include <mod/Config.h>

#include <functional>
#include <memory>

namespace mod {
namespace lib {
namespace Rules {
struct Real;
} // namespace Rules
namespace RC {
struct Common;
struct Parallel;
struct Sub;
struct Super;

// For explicitly instantiating the generic function separately.
#define MOD_RC_COMPOSE_BY_MATCH_MAKER(MM)                                       \
	void composeRuleRealByMatchMaker(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, const MM &mm, std::function<void(std::unique_ptr<lib::Rules::Real>) > rr, LabelSettings labelSettings);
MOD_RC_COMPOSE_BY_MATCH_MAKER(Common);
MOD_RC_COMPOSE_BY_MATCH_MAKER(Parallel);
MOD_RC_COMPOSE_BY_MATCH_MAKER(Sub);
MOD_RC_COMPOSE_BY_MATCH_MAKER(Super);
#undef MOD_RC_COMPOSE_BY_MATCH_MAKER

} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_COMPOSE_RULE_REAL_H */