#ifndef MOD_LIB_RULE_PROP_STRING_H
#define	MOD_LIB_RULE_PROP_STRING_H

#include <mod/lib/Rule/GraphDecl.h>
#include <mod/lib/Rule/Properties/Property.h>

namespace mod {
namespace lib {
struct StringStore;
namespace Rule {
struct PropTermCore;

struct PropStringCore : PropCore<PropStringCore, GraphType, std::string, std::string> {

	explicit PropStringCore(const GraphType &g) : PropCore(g) {
		verify(&g);
	}
};

} // namespace Rule
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_RULE_PROP_STRING_H */