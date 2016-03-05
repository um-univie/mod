#include <jla_boost/graph/dpo/IO.hpp>

namespace jla_boost {
namespace GraphDPO {

std::ostream &operator<<(std::ostream &s, Membership m) {
	switch(m) {
	case Membership::Left: return s << "Left";
	case Membership::Right: return s << "Right";
	case Membership::Context: return s << "Context";
	}
	assert(false);
	return s;
}

} // namespace GraphDPO
} // namespace jla_boost