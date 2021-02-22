#include <mod/lib/Rules/Application/ComponentMatchDB/Basic.hpp>
#include <mod/lib/Rules/Application/ComponentMatchDB/Util.hpp>

namespace mod::lib::Rules::Application::ComponentMatchDB {

Basic::Basic(const LabelSettings& labelSettings): labelSettings(labelSettings) {};

bool Basic::isValid(const Real& rule,
                    const std::vector<const Graph::Single*>& subset,
                    const std::vector<const Graph::Single*>& universe) {
	bool hasSubsetMatch = false;

	for (size_t cid = 0; cid < rule.getDPORule().numLeftComponents; ++cid) {
		bool hasMatch = false;
		for (const Graph::Single* host : subset) {
			hasMatch = isMonomorphic(rule.getDPORule(), cid, host->getLabelledGraph(),
			                         labelSettings);
			if (hasMatch) {
				hasSubsetMatch = true;
				break;
			}
		}

		if (hasMatch) {
			continue;
		}

		for (const Graph::Single* host : universe) {
			hasMatch = isMonomorphic(rule.getDPORule(), cid, host->getLabelledGraph(),
			                         labelSettings);
			if (hasMatch) {
				break;
			}
		}

		if (!hasMatch) {
			return false;
		}
	}

	return hasSubsetMatch;
}

std::map<std::pair<const Graph::Single*, size_t>, std::vector<ComponentMatch>>
Basic::getMatches(const Real& rule,
                  const std::vector<const Graph::Single*>& subset,
                  const std::vector<const Graph::Single*>& universe)  {
	std::map<std::pair<const Graph::Single*, size_t>, std::vector<ComponentMatch>> out;
	std::vector<const Graph::Single*> graphs(subset.begin(), subset.end());
	graphs.insert(graphs.end(), universe.begin(), universe.end());
	for (size_t cid = 0; cid < rule.getDPORule().numLeftComponents; ++cid) {
		for (size_t graphIndex = 0; graphIndex < subset.size(); ++graphIndex) {
			const auto& host = graphs[graphIndex]->getLabelledGraph();
			auto key = std::make_pair(graphs[graphIndex], cid);
			matches[key] = enumerateMonomorphisms(rule.getDPORule(), cid, host, labelSettings);
			out[key] = std::vector<ComponentMatch>();
			for (const auto& m : matches[key]) {
				out[key].push_back(ComponentMatch(cid, graphIndex, m));
			}
		}
	}
	return out;
}



}
