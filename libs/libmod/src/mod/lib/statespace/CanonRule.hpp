#ifndef MOD_LIB_STATESPACE_CANON_RULE_HPP
#define MOD_LIB_STATESPACE_CANON_RULE_HPP

#include <mod/lib/Rules/Real.hpp>
#include <perm_group/allocator/raw_ptr.hpp>
#include <perm_group/group/generated.hpp>
#include <perm_group/permutation/built_in.hpp>

namespace mod::lib::Rules {

using AutGroup = perm_group::generated_group<perm_group::raw_ptr_allocator<std::vector<int> > >;

std::unique_ptr<AutGroup> getRuleAutomorphisms(const Real &r);

}
#endif
