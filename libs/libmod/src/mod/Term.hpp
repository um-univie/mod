#ifndef MOD_UNIFICATION_H
#define	MOD_UNIFICATION_H

#include <mod/BuildConfig.hpp>

#include <string>

namespace mod {
namespace Term {

MOD_DECL void mgu(const std::string &left, const std::string &right);

} // namespace Term
} // namespace mod

#endif	/* MOD_UNIFICATION_H */