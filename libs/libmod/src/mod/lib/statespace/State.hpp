#ifndef MOD_LIB_STATESPACE_STATE_HPP
#define MOD_LIB_STATESPACE_STATE_HPP

#include <mod/lib/statespace/RuleApplication.hpp>
#include <mod/lib/statespace/ComponentMap.hpp>

namespace mod::lib::statespace {

class State {
public:
private:
	std::vector<std::vector<RuleApplication>> applications;
};

}

#endif
