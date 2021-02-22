#include <mod/lib/Rules/Application/Enumerate.hpp>
#include <iostream>


namespace mod::lib::Rules::Application::detail {

std::vector<std::unique_ptr<Rules::Real>>
computeDerivations(const Rules::Real& realRule,
                   const std::vector<const Graph::Single*>& subset,
                   const std::vector<const Graph::Single*>& universe,
                   const std::map<std::pair<const Graph::Single*, size_t>, std::vector<ComponentMatch>>& matchDB) {

	const LabelledRule& rule = realRule.getDPORule();
	std::vector<std::unique_ptr<Rules::Real>> derivations;

	std::vector< const Graph::Single*> graphs(subset.begin(), subset.end());
	graphs.insert(graphs.end(), universe.begin(), universe.end());
	std::vector<std::vector<ComponentMatch>> matches(rule.numLeftComponents);

	int numMatches = 0;
	for (const auto& it : matchDB) {
		for (const auto& cm : it.second) {
			matches[cm.componentIndex].push_back(cm);
			numMatches++;
		}
	}
	std::cout << "NUM MATCHES: " << numMatches << std::endl;

	return derivations;
}

}
