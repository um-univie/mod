#ifndef MOD_LIB_DG_RULEAPPLICATION_ENUMERATE_HPP
#define MOD_LIB_DG_RULEAPPLICATION_ENUMERATE_HPP

#include <mod/lib/Rules/Real.hpp>
#include <iostream>
#include <mod/lib/Rules/Application/ComponentMatch.hpp>
#include <functional>

namespace mod::lib::Rules::Application {

namespace detail {

std::vector<std::unique_ptr<Rules::Real>>
computeDerivations(const Rules::Real& rule,
                   const std::vector<ComponentMatch>& matches,
                   std::function<bool(std::unique_ptr<Rules::Real>)>& onMatch,
                   std::function<bool(const Graph::LabelledGraph*, int)>& onNewGraphInstance);
}

template<typename ComponentMatchDB>
std::vector<std::unique_ptr<Rules::Real>> computeDerivations(const Rules::Real& rule,
                                                             const std::vector<const Graph::Single*>& subset,
                                                             const std::vector<const Graph::Single*>& universe,
                                                             ComponentMatchDB& matchDB,
                   std::function<bool(std::unique_ptr<Rules::Real>)>& onMatch,
                   std::function<bool(const Graph::LabelledGraph*, int)>& onNewGraphInstance
                                                             ) {
	std::vector<std::unique_ptr<Rules::Real>> derivations;
	std::cout << "Now I'm here" << std::endl;

	if (!matchDB.isValid(rule, subset, universe)) {
		return derivations;
	}

	std::cout << "All connected components has a match..." << std::endl;
	std::cout << "Finding matches" << std::endl;
	auto matches = matchDB.getMatches(rule, subset, universe);

	derivations = detail::computeDerivations(rule, matches, onMatch, onNewGraphInstance);
	return derivations;
}

}

#endif
