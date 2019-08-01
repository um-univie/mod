#include "WAM.hpp"

#include <mod/lib/StringStore.hpp>

namespace mod {
namespace lib {
namespace Term {

const StringStore &getStrings() {
	static StringStore strings;
	return strings;
}

} // namespace Term
} // namespace lib
} // namespace mod