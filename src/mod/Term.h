#ifndef MOD_UNIFICATION_H
#define	MOD_UNIFICATION_H

#include <string>

namespace mod {
namespace Term {

void mgu(const std::string &left, const std::string &right);

} // namespace Term
} // namespace mod

#endif	/* MOD_UNIFICATION_H */