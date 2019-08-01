#include "ComposeRuleReal.hpp"

#include <mod/lib/RC/ComposeRuleRealGeneric.hpp>
#include <mod/lib/RC/MatchMaker/Common.hpp>
#include <mod/lib/RC/MatchMaker/Parallel.hpp>
#include <mod/lib/RC/MatchMaker/Sub.hpp>
#include <mod/lib/RC/MatchMaker/Super.hpp>

namespace mod {
namespace lib {
namespace RC {

#define MOD_RC_COMPOSE_BY_MATCH_MAKER(MM)                                       \
	void composeRuleRealByMatchMaker(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, const MM &mm, std::function<bool(std::unique_ptr<lib::Rules::Real>) > rr, LabelSettings labelSettings) { \
		composeRuleRealByMatchMakerGeneric(rFirst, rSecond, mm, rr, labelSettings); \
	}
MOD_RC_COMPOSE_BY_MATCH_MAKER(Common);
MOD_RC_COMPOSE_BY_MATCH_MAKER(Parallel);
MOD_RC_COMPOSE_BY_MATCH_MAKER(Sub);
MOD_RC_COMPOSE_BY_MATCH_MAKER(Super);
#undef MOD_RC_COMPOSE_BY_MATCH_MAKER


} // namespace RC
} // namespace lib
} // namespace mod