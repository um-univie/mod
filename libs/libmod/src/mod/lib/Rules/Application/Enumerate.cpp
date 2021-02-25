#include <mod/lib/Rules/Application/Enumerate.hpp>
#include <mod/lib/Rules/Application/PartialMatch.hpp>
#include <iostream>


namespace mod::lib::Rules::Application::detail {

std::tuple<std::vector<size_t>, std::vector<std::vector<ComponentMatch>>, std::vector<const ComponentMatch*>>
buildPatternMatchVector(const LabelledRule& rule,
                        const std::vector<ComponentMatch>& matches) {
	std::vector<size_t> universeOffsets(rule.numLeftComponents, 0);
	std::vector<std::vector<ComponentMatch>> patternMatches(rule.numLeftComponents);
	std::vector<const ComponentMatch*> subsetMatches;
	bool doneUniverse = false;
	for (const auto& m : matches) {
		assert(m.graphInstance == 0);
		// ensure matches are sorted such that unverse graphs comes before subset graphs
		assert(!doneUniverse || (m.isSubsetHost && doneUniverse));
		doneUniverse = m.isSubsetHost;
		if (!doneUniverse) {
			universeOffsets[m.componentIndex] += 1;
		} else {
			subsetMatches.push_back(&m);

		}
		patternMatches[m.componentIndex].push_back(m);
	}
	std::sort(subsetMatches.begin(), subsetMatches.end(),
	          [] (const ComponentMatch* m1, const ComponentMatch* m2) {
		return m1->componentIndex < m2->componentIndex;
	});
	return std::make_tuple(universeOffsets, patternMatches, subsetMatches);
}

std::unordered_map<const Graph::Single *, std::vector<const ComponentMatch*>>
buildMatchInstanceMap(const std::vector<ComponentMatch>& matches) {
	std::unordered_map<const Graph::Single *, std::vector<const ComponentMatch*>> out;
	for (const auto& m : matches) {
		assert(m.graphInstance == 0);
		out[m.host].push_back(&m);
	}
	return out;
}

void
computeDerivations(const Rules::Real& realRule,
                   const std::vector<ComponentMatch>& matchDB,
                   std::function<bool(std::vector<const Graph::Single*>, std::unique_ptr<Rules::Real>)>& onMatch,
                   std::function<bool(const Graph::Single*, int)>& onNewGraphInstance,
                   int verbosity) {
	if (matchDB.size() == 0) {
		return;
	}


	const LabelledRule& rule = realRule.getDPORule();
	std::vector<std::unique_ptr<Rules::Real>> derivations;

	IO::Logger logger(std::cout);
	if (verbosity > 1) {
		logger.indent() << "computeDerivations(rule: " << realRule.getName() << " num_matches: " << matchDB.size() << ")" << std::endl;
		logger.indentLevel++;
		logger.indent() << "building data structures..." << std::endl;
	}


	std::vector<size_t> stack;
	auto matchInstanceMap = buildMatchInstanceMap(matchDB);

	std::vector<size_t> universeOffsets;
	std::vector<std::vector<ComponentMatch>> matches;
	std::vector<const ComponentMatch*> subsetMatches;

	std::tie(universeOffsets, matches, subsetMatches) = buildPatternMatchVector(rule, matchDB);

	std::vector<std::vector<size_t>> instanceOffsets(rule.numLeftComponents, std::vector<size_t>(rule.numLeftComponents));
	for (std::vector<size_t>& offsets : instanceOffsets) {
		for (size_t cid = 0; cid < rule.numLeftComponents; ++cid) {
			offsets[cid] = matches[cid].size();
		}
	}

	if (subsetMatches.size() == 0) {
		return;
	}

	size_t curCid = subsetMatches[0]->componentIndex;

	auto addInstance = [&] (const ComponentMatch& cm, bool addAll) {
		if(!onNewGraphInstance(cm.host, cm.graphInstance + 1)) {
			return;
		}
		for (size_t cid = 0; cid < rule.numLeftComponents; ++cid) {
			instanceOffsets[cm.componentIndex][cid] = matches[cid].size();
		}
		assert(matchInstanceMap.find(cm.host) != matchInstanceMap.end());
		for (const ComponentMatch *m : matchInstanceMap[cm.host]) {
			if (m->componentIndex == cm.componentIndex || (!addAll && m->componentIndex <= cm.componentIndex)) {
				continue;
			}

			// We have already fixed the subset match (and enumerated everything),
			// hence no reason to add the match again, again
			if (m->isSubsetHost && m->componentIndex < curCid) {
				continue;
			}

			matches[m->componentIndex].push_back(ComponentMatch(*m));
			matches[m->componentIndex].back().graphInstance = cm.graphInstance + 1;
		}
	};

	auto removeInstance = [&] (const ComponentMatch& cm) {
		for (size_t cid = 0; cid < rule.numLeftComponents; ++cid) {
			matches[cid].resize(instanceOffsets[cm.componentIndex][cid]);
		}
	};

	if (verbosity > 1) {
		logger.indent() << "subsetMatches: " << subsetMatches.size();
		logger.s << " matchVector: (";
		for (const auto& ms : matches) {
			logger.s << ms.size() << ",";
		}
		logger.s << ")" << std::endl;
	}

	assert(subsetMatches.size() > 0);
	PartialMatch partialMatch(realRule);
	for (const ComponentMatch* subsetMatch : subsetMatches) {
		bool success, addedGraph;
		// We have tried to fix all subset matches for curCid.
		// In doing so, we have enumerated all combinations that include these matches.
		// When fixing later components, we can therefore forget these matches from
		// The match vector.
		if (subsetMatch->componentIndex > curCid) {
			matches[curCid].resize(universeOffsets[curCid]); //dummy default
			if (matches[curCid].empty()) {
				break;
			}
			curCid = subsetMatch->componentIndex;
		}

		std::tie(success, addedGraph) = partialMatch.tryPush(*subsetMatch);
		if (!success) {
			continue;
		}

		assert(addedGraph);

		// A start graph is always added
		addInstance(*subsetMatch, true);

		if (partialMatch.isFull()) {
			std::unique_ptr<Rules::Real> res = partialMatch.apply();
			assert(res != nullptr);
			bool shouldContinue = onMatch(partialMatch.getLhs(), std::move(res));
			if (!shouldContinue) {
				return;
			}
			//derivations.push_back(std::move(res));
			partialMatch.pop();
			continue;
		}

		if (verbosity > 1) {
			logger.indent() << "fixing component match:" << subsetMatch->componentIndex << " " << partialMatch << std::endl;
			logger.indentLevel++;
		}

		stack.push_back(0);
		while (stack.size() > 0) {
			size_t cid = stack.size() - 1;
			if (cid >= curCid) {
				cid += 1;
			}
			size_t mid = stack.back();
			if (verbosity > 1) {
				logger.indent() << "cid: " << cid << " " << "mid: " << mid << std::endl;
			}

			if (mid == matches[cid].size()) {

				//logger.indent() << "no more matches for cid. popping from stack." << std::endl;

				stack.pop_back();
				if (partialMatch.lastPushIsNewInstance()) {
					removeInstance(partialMatch.getCompMatches().back());
				}
				partialMatch.pop();
				if (stack.size() > 0) {
					stack.back() += 1;
				}
				continue;
			}

			assert(cid < matches.size() && mid < matches[cid].size());
			const ComponentMatch& match = matches[cid][mid];

			if (verbosity > 1) {
				logger.indent() << "trying to push match" << std::endl;
			}
			std::tie(success, addedGraph) = partialMatch.tryPush(match);
			if (!success) {
				stack.back() += 1;
				continue;
			}

			if (verbosity > 1) {
				logger.indent() << "could compose" << partialMatch << std::endl;
			}

			if (partialMatch.isFull()) {
				std::unique_ptr<Rules::Real> res = partialMatch.apply();
				assert(res != nullptr);
				bool shouldContinue = onMatch(partialMatch.getLhs(), std::move(res));
				if (!shouldContinue) {
					return;
				}
				partialMatch.pop();
				stack.back() += 1;
			} else {
				if (addedGraph) {
					addInstance(match, false);
				}
				stack.push_back(0);
			}

		}
		if(verbosity > 1) {
			logger.indentLevel--;
		}
	}



	return;
}

}
