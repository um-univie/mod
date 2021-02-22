#ifndef MOD_LIB_DG_RULEAPPLICATION_ENUMERATE_HPP
#define MOD_LIB_DG_RULEAPPLICATION_ENUMERATE_HPP

#include <mod/lib/Rules/Real.hpp>
#include <iostream>
#include <mod/lib/Rules/Application/ComponentMatch.hpp>

namespace mod::lib::Rules::Application {

namespace detail {
std::vector<std::unique_ptr<Rules::Real>>
computeDerivations(const Rules::Real& rule,
                   const std::vector<const Graph::Single*>& subset,
                   const std::vector<const Graph::Single*>& universe,
                   const std::map<std::pair<const Graph::Single*, size_t>, std::vector<ComponentMatch>>& matches);
}

template<typename ComponentMatchDB>
std::vector<std::unique_ptr<Rules::Real>> computeDerivations(const Rules::Real& rule,
                                                             const std::vector<const Graph::Single*>& subset,
                                                             const std::vector<const Graph::Single*>& universe,
                                                             ComponentMatchDB& matchDB) {
	std::vector<std::unique_ptr<Rules::Real>> derivations;
	std::cout << "Now I'm here" << std::endl;

	if (!matchDB.isValid(rule, subset, universe)) {
		return derivations;
	}

	std::cout << "All connected components has a match..." << std::endl;
	std::cout << "Finding matches" << std::endl;
	auto matches = matchDB.getMatches(rule, subset, universe);

	derivations = detail::computeDerivations(rule, subset, universe, matches);
	return derivations;
}

}

#endif
