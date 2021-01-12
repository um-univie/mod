#ifndef MOD_UNIFICATION_HPP
#define MOD_UNIFICATION_HPP

#include <mod/BuildConfig.hpp>

#include <string>

namespace mod::Term {

MOD_DECL void mgu(const std::string &left, const std::string &right);

} // namespace mod::Term

#endif // MOD_UNIFICATION_HPP