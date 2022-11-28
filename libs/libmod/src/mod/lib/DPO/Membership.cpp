#include "Membership.hpp"

#include <ostream>

namespace mod::lib::DPO {

std::ostream &operator<<(std::ostream &s, Membership m) {
	switch(m) {
	case Membership::L:
		return s << "Left";
	case Membership::R:
		return s << "Right";
	case Membership::K:
		return s << "Context";
	}
	__builtin_unreachable();
}

} // namespace mod::lib::DPO