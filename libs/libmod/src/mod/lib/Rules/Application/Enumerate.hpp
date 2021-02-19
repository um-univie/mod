#ifndef MOD_LIB_DG_RULEAPPLICATION_ENUMERATE_HPP
#define MOD_LIB_DG_RULEAPPLICATION_ENUMERATE_HPP

#include <mod/lib/Rules/Real.hpp>

namespace mod::lib::Rules::Application {

std::vector<std::unique_ptr<Rules::Real>> computeDerivations(const Rules::Real& rule,
                                                             const std::vector<const Graph::Single*>& subset,
                                                             const std::vector<const Graph::Single*>& universe);

}

#endif
