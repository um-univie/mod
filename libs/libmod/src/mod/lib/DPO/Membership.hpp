#ifndef MOD_LIB_DPO_MEMBERSHIP_HPP
#define MOD_LIB_DPO_MEMBERSHIP_HPP

#include <iosfwd>

namespace mod::lib::DPO {

enum class Membership {
	L, K, R
};

inline Membership invert(Membership m) {
	switch(m) {
	case Membership::L:
		return Membership::R;
	case Membership::K:
		return Membership::K;
	case Membership::R:
		return Membership::L;
	}
	__builtin_unreachable();
}

std::ostream &operator<<(std::ostream &s, Membership m);

} // namespace mod::lib::DPO

#endif // MOD_LIB_DPO_MEMBERSHIP_HPP