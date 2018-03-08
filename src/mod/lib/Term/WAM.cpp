#include "WAM.h"

#include <mod/lib/StringStore.h>

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