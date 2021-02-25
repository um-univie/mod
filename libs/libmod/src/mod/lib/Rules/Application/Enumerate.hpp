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
                   std::function<bool(const Graph::Single*, int)>& onNewGraphInstance,
                   int verbosity
                   );
}

template<typename ComponentMatchDB>
void computeDerivations(const Rules::Real& rule,
                                                             size_t numSubsetGraphs,
                                                             const std::vector<const Graph::Single*>& universe,
                                                             ComponentMatchDB& matchDB,
                   std::function<bool(std::vector<const Graph::Single*>, std::unique_ptr<Rules::Real>)>& onMatch,
                   std::function<bool(const Graph::Single*, int)>& onNewGraphInstance,
                        int verbosity

                                                             ) {
	std::vector<std::unique_ptr<Rules::Real>> derivations;

	if (!matchDB.isValid(rule, numSubsetGraphs, universe)) {
		return;
	}

	auto matches = matchDB.getMatches(rule, numSubsetGraphs, universe);

	detail::computeDerivations(rule, matches, onMatch, onNewGraphInstance, verbosity);
}

}

#endif
