#ifndef MOD_LIB_RULE_SHALLOW_H
#define	MOD_LIB_RULE_SHALLOW_H

#include <mod/lib/Rule/Base.h>

#include <string>

namespace mod {
namespace lib {
namespace Rule {

struct Shallow : public Base {

	virtual bool isReal() const override {
		return false;
	}
};

} // namespace Rule
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_RULE_SHALLOW_H */