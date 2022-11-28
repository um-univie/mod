#ifndef MOD_LIB_RC_COMPOSE_RULE_REAL_HPP
#define MOD_LIB_RC_COMPOSE_RULE_REAL_HPP

#include <mod/Config.hpp>

#include <functional>
#include <memory>

namespace mod::lib::IO {
struct Logger;
} // namespace mod::lib::IO
namespace mod::lib::Rules {
struct Real;
} // namespace mod::lib::Rules
namespace mod::lib::RC {
struct Common;
struct Parallel;
struct Sub;
struct Super;

// For explicitly instantiating the generic function separately.
#define MOD_RC_COMPOSE_BY_MATCH_MAKER(MM)                                                                              \
   void composeRuleRealByMatchMaker(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond,                   \
         const MM &mm, std::function<bool(std::unique_ptr<lib::Rules::Real>) > rr, LabelSettings labelSettings);
MOD_RC_COMPOSE_BY_MATCH_MAKER(Common)
MOD_RC_COMPOSE_BY_MATCH_MAKER(Parallel)
MOD_RC_COMPOSE_BY_MATCH_MAKER(Sub)
MOD_RC_COMPOSE_BY_MATCH_MAKER(Super)
#undef MOD_RC_COMPOSE_BY_MATCH_MAKER

#define MOD_RC_COMPOSE_BY_MATCH_MAKER_IMPL(MM)                                                                         \
	void composeRuleRealByMatchMaker(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond,                   \
			const MM &mm, std::function<bool(std::unique_ptr<lib::Rules::Real>) > rr, LabelSettings labelSettings) {      \
		composeRuleRealByMatchMakerGeneric(rFirst, rSecond, mm, rr, labelSettings);                                      \
	}

} // namespace mod::lib::RC

#endif // MOD_LIB_RC_COMPOSE_RULE_REAL_HPP