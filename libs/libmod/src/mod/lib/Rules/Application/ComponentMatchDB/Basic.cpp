#include <mod/lib/Rules/Application/ComponentMatchDB/Basic.hpp>
#include <mod/lib/Rules/Application/ComponentMatchDB/Util.hpp>

namespace mod::lib::Rules::Application::ComponentMatchDB {

Basic::Basic(const LabelSettings& labelSettings): labelSettings(labelSettings) {};

bool Basic::isValid(const Real& rule,
                    size_t numSubsetGraphs,
                    const std::vector<const Graph::Single*>& universe) {
	bool hasSubsetMatch = false;

	for (size_t cid = 0; cid < rule.getDPORule().numLeftComponents; ++cid) {
		bool hasMatch = false;
		for (size_t gid = 0; gid < universe.size(); ++gid) {
			const Graph::Single* host = universe[gid];
			hasMatch = isMonomorphic(rule.getDPORule(), cid, host->getLabelledGraph(),
			                         labelSettings);
			if (hasMatch) {
				if (gid < numSubsetGraphs) {
					hasSubsetMatch = true;
				}
				break;
			}

//			if (rule.getDPORule().numLeftComponents == 1 && gid+1 == numSubsetGraphs ) {
//				break;
//			}
		}

		if (!hasMatch) {
			return false;
		}
	}

	return hasSubsetMatch;
}

//std::map<std::pair<const Graph::Single*, size_t>, std::vector<ComponentMatch>>
std::vector<ComponentMatch>
Basic::getMatches(const Real& rule,
                  size_t numSubsetGraphs,
                  const std::vector<const Graph::Single*>& universe)  {
	// std::map<std::pair<const Graph::Single*, size_t>, std::vector<ComponentMatch>> out;
	std::vector<ComponentMatch> out;
	size_t universeSize = universe.size() - 1;
	/*
	if (rule.getDPORule().numLeftComponents == 1) {
		universeSize = numSubsetGraphs - 1;
	}*/
	for (size_t gid = universeSize; gid != static_cast<size_t>(-1); --gid) {
		for (size_t cid = 0; cid < rule.getDPORule().numLeftComponents; ++cid) {
			const auto& host = universe[gid]->getLabelledGraph();

			bool isSubset = (gid < numSubsetGraphs);
			auto key = std::make_pair(universe[gid], cid);
			matches[key] = enumerateMonomorphisms(rule.getDPORule(), cid, host, labelSettings);
			for (const auto& m : matches[key]) {
				out.push_back(ComponentMatch(cid, isSubset, universe[gid], m));
			}
		}
	}
	return out;
}



}
