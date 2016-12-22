#ifndef MOD_LIB_RULES_PROP_STRING_H
#define	MOD_LIB_RULES_PROP_STRING_H

#include <mod/lib/Rules/GraphDecl.h>
#include <mod/lib/Rules/Properties/Property.h>

namespace mod {
namespace lib {
namespace Rules {

struct PropStringCore : PropCore<PropStringCore, GraphType, std::string, std::string> {

	explicit PropStringCore(const GraphType &g) : PropCore(g) {
		verify(&g);
	}
};

} // namespace Rules
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_RULES_PROP_STRING_H */