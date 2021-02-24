#ifndef MOD_LIB_DG_RULEAPPLICATION_ENUMERATE_HPP
#define MOD_LIB_DG_RULEAPPLICATION_ENUMERATE_HPP

#include <mod/lib/Rules/Real.hpp>
#include <iostream>
#include <mod/lib/Rules/Application/ComponentMatch.hpp>
#include <functional>

namespace mod::lib::Rules::Application {

namespace detail {

void
computeDerivations(const Rules::Real& rule,
                   const std::vector<ComponentMatch>& matches,
                   std::function<bool(std::vector<const Graph::Single*>, std::unique_ptr<Rules::Real>)>& onMatch,
                   std::function<bool(const Graph::Single*, int)>& onNewGraphInstance
                   );
}

template<typename ComponentMatchDB>
void computeDerivations(const Rules::Real& rule,
                                                             size_t numSubsetGraphs,
                                                             const std::vector<const Graph::Single*>& universe,
                                                             ComponentMatchDB& matchDB,
                   std::function<bool(std::vector<const Graph::Single*>, std::unique_ptr<Rules::Real>)>& onMatch,
                   std::function<bool(const Graph::Single*, int)>& onNewGraphInstance
                                                             ) {
	std::vector<std::unique_ptr<Rules::Real>> derivations;
	std::cout << "Now I'm here" << std::endl;

	if (!matchDB.isValid(rule, numSubsetGraphs, universe)) {
		return;
	}

	std::cout << "All connected components has a match..." << std::endl;
	std::cout << "Finding matches" << std::endl;
	auto matches = matchDB.getMatches(rule, numSubsetGraphs, universe);

	detail::computeDerivations(rule, matches, onMatch, onNewGraphInstance);
}

}

#endif
