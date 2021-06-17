#include <mod/lib/Rules/Application/CanonMatch.hpp>
#include <mod/lib/Rules/Application/GeneratingSystem.hpp>
#include <mod/lib/Graph/LabelledGraph.hpp>

#include <perm_group/group/schreier_stabilizer.hpp>
#include <perm_group/transversal/explicit.hpp>
#include <perm_group/orbit.hpp>
#include <perm_group/allocator/raw_ptr.hpp>
#include <perm_group/group/generated.hpp>
#include <perm_group/permutation/built_in.hpp>

#include <iostream>




namespace mod::lib::Rules::Application {

class PatternSchreierGroup {
public:
	using Perm = std::vector<int>;
	using Alloc = perm_group::raw_ptr_allocator<Perm>;
	using Transversal = perm_group::transversal_explicit<Alloc>;
	struct fixed_vertices {
		fixed_vertices(const Real& rule, size_t compIndex) {
			const auto& dpoRule = rule.getDPORule();
			const auto& lgRule = get_labelled_left(dpoRule);;
			const auto& compPatternGraph = get_graph(get_component_graph_v2(compIndex, lgRule));
			for (auto vp : asRange(vertices(compPatternGraph))) {
				const auto vpCore = get_component_core_vertex(compIndex, vp, lgRule);
				fixes.push_back(vpCore);
			}
		}

		int operator()(const Perm &p) {
			if (curFix == fixes.size()) {
				return bpp(p);
			} else {
				curFix += 1;
				return fixes[curFix - 1];
			}
		}

		size_t curFix = 0;
		std::vector<size_t> fixes;
		perm_group::base_point_first_moved bpp;
	};

	PatternSchreierGroup(const Real& rule, size_t compIndex): gens(rule.getAutGroup()),
	    fv(rule, compIndex), stab(gens.get_allocator(), fv), chain(nullptr)  {

		for (const auto& gen : gens.generators()) {
			stab.add_generator(gen);
		}

	}

	void next() {
		if (chain == nullptr) {
			chain = stab.getChain();
		} else if (chain->get_next() != nullptr) {
			chain = chain->get_next();
		}
//		std::cout << "fixed element " << chain->fixed_element() << std::endl;
	}

	void reset() {
		chain = nullptr;
	}

	decltype(auto) generator_ptrs() {
		return (chain) ? chain->generator_ptrs() : gens.generator_ptrs();
	}

	const Rules::AutGroup& gens;
	fixed_vertices fv;
	perm_group::GeneratingSystem<Transversal, fixed_vertices> stab;
	const perm_group::GeneratingSystem<Transversal, fixed_vertices>::Chain *chain;
};

class HostSchreierGroup {
public:
	using Perm = std::vector<int>;
	using Alloc = perm_group::raw_ptr_allocator<Perm>;
	using Transversal = perm_group::transversal_explicit<Alloc>;
	struct fixed_vertices {
		fixed_vertices(const std::vector<int>& fixes): fixes(fixes) {}
		int operator()(const Perm &p) {
			if (curFix == fixes.size()) {
				return bpp(p);
			} else {
				curFix += 1;
				return fixes[curFix - 1];
			}
		}

		size_t curFix = 0;
		const std::vector<int> &fixes;
		perm_group::base_point_first_moved bpp;
	};

	HostSchreierGroup(const Graph::Single& host, const std::vector<int>& fixes,
	                  const LabelSettings& labelSettings):
	    gens(host.getAutGroup(labelSettings.type, labelSettings.withStereo)), fv(fixes),
	    stab(gens.get_allocator(), fv), chain(nullptr)  {

		for (const auto& gen : gens.generators()) {
			stab.add_generator(gen);
		}

	}

	bool next() {
		if (chain && chain->get_next()) {
			chain = chain->get_next();
			return true;
		}  else if (!chain) {
			chain = stab.getChain();
			return true;
		}
		return false;
	}

	void reset() {
		chain = nullptr;
	}

	decltype(auto) generator_ptrs() {
		return (chain) ? chain->generator_ptrs() : gens.generator_ptrs();
	}

	const Graph::Single::AutGroup& gens;
	fixed_vertices fv;
	perm_group::GeneratingSystem<Transversal, fixed_vertices> stab;
	const perm_group::GeneratingSystem<Transversal, fixed_vertices>::Chain *chain;
};

bool isCanonComponentMatchFast(const Real& rule,
                               size_t compIndex,
                               const Graph::LabelledGraph& host,
                               const ComponentMatch::Morphism& match) {
	const auto& constAutPtrs = rule.getAutGroup().generator_ptrs();
	if (constAutPtrs.size() == 0)  {
		return true;
	}

	std::vector<Rules::AutGroup::const_pointer> gens(constAutPtrs.begin(), constAutPtrs.end());
	size_t genSize = gens.size();

	const auto& dpoRule = rule.getDPORule();
	//const auto& dpoHost = g->getBindRule()->getRule().getDPORule();
	const auto& lgRule = get_labelled_left(dpoRule);;
	//const auto& lgHost = get_labelled_right(dpoHost);
//	const auto& gRule = get_graph(lgRule);
	const auto& compPatternGraph = get_graph(get_component_graph_v2(compIndex, lgRule));
	const auto& gHost = get_graph(host);

	for (auto vp : asRange(vertices(compPatternGraph))) {
		const auto vpCore = get_component_core_vertex(compIndex, vp, lgRule);
		auto vh = get(match, compPatternGraph, gHost, vp);
//		std::cout << vp << ": " << vh << std::endl;
		size_t curAutIndex = 0;
		while (curAutIndex < genSize) {
//			const auto& aut = *gens[curAutIndex];
//			for (size_t i = 0; i < aut.size(); ++i) { std::cout << aut[i] << " "; }
//			std::cout << std::endl;
//			std::cout << validAuts[curAutIndex]->size() << " " << num_vertices(gHost) << std::endl;
			size_t img =  gens[curAutIndex]->at(vpCore);
			const auto vpImg = dpoRule.coreVertexToLeftComponentVertex[img];
			//assert(vpImg.first == compIndex);

			if (vpImg.first == compIndex) {
				auto vhImg = get(match, compPatternGraph, gHost, vpImg.second);
	//			std::cout << "\t" << vhImg << std::endl;
				if (vh > vhImg) {
//					std::cout << "NOT CANON COMP MATCH" << std::endl;
					 //std::cout << "\t" << vh << " is not canon" << std::endl;
					return false;
				}
			}
			if (img != vpCore) {
				assert(static_cast<int>(genSize) >= 0);
				std::swap(gens[curAutIndex], gens[genSize - 1]);
				genSize -= 1;
			} else {
				curAutIndex += 1;
			}
		}
		if (genSize == 0)  {
			// std::cout << "\tNO MORE AUTS" << std::endl;
			return true;
		}
		// std::cout << "\tFixed " << vh << " remaining auts: " << validAutSize << std::endl;
	}
	return true;
}

bool isCanonComponentMatch(const Real& rule,
                           size_t compIndex,
                           const Graph::LabelledGraph& host,
                           const ComponentMatch::Morphism& match) {
	const auto& gens = rule.getAutGroup();
	using Perm = std::vector<int>;
	using Alloc = perm_group::raw_ptr_allocator<Perm>;
	using Transversal = perm_group::transversal_explicit<Alloc>;

	struct fixed_vertices {
		int operator()(const Perm &p) {
			if (curFix == fixes.size()) {
				return bpp(p);
			} else {
				curFix += 1;
				return fixes[curFix - 1];
			}
		}

		size_t curFix = 0;
		std::vector<size_t> fixes;
		perm_group::base_point_first_moved bpp;
	};
	fixed_vertices fv;
	const auto& dpoRule = rule.getDPORule();
	//const auto& dpoHost = g->getBindRule()->getRule().getDPORule();
	const auto& lgRule = get_labelled_left(dpoRule);;
	//const auto& lgHost = get_labelled_right(dpoHost);
//	const auto& gRule = get_graph(lgRule);
	const auto& compPatternGraph = get_graph(get_component_graph_v2(compIndex, lgRule));
	const auto& gHost = get_graph(host);
//	std::cout << "fix order: ";
	for (auto vp : asRange(vertices(compPatternGraph))) {
//		std::cout << vp << ", ";
		const auto vpCore = get_component_core_vertex(compIndex, vp, lgRule);
		fv.fixes.push_back(vpCore);
	}
//	std::cout << std::endl;
	perm_group::GeneratingSystem<Transversal, fixed_vertices> stab(gens.get_allocator(), fv);
	for (const auto& gen : gens.generators()) {
//		std::cout << "gen: ";
//		for (auto p : gen) {
//			std::cout << p << ", ";
//		}
//		std::cout << std::endl;
		stab.add_generator(gen);
	}
	const perm_group::GeneratingSystem<Transversal, fixed_vertices>::Chain *chain = nullptr;
	for (auto vp : asRange(vertices(compPatternGraph))) {
//		std::cout << "-------" << std::endl;
		const auto vpCore = get_component_core_vertex(compIndex, vp, lgRule);
		auto vh = get(match, compPatternGraph, gHost, vp);
		bool isCanon = true;
		if (chain == nullptr) {
			const auto genPtrs = gens.generator_ptrs();
			perm_group::orbit(vpCore, genPtrs.begin(), genPtrs.end(),
			                  [&] (int w, int img,
			                        decltype(genPtrs.begin())) {
				//auto vhImg = get(match, gRule, gHost, img);
				const auto vpImg = dpoRule.coreVertexToLeftComponentVertex[img];
				//assert(vpImg.first == compIndex);
				if (vpImg.first != compIndex) {
					return;
				}
				auto vhImg = get(match, compPatternGraph, gHost, vpImg.second);
//				std::cout << "orbit: " << w << ", " << img << ", vh: " << vhImg << std::endl;
//				std::cout << "orbit: (" << compIndex << ", " << vp << ") (" << vpImg.first << ", " << vpImg.second << ") | " << vh << " "<< vhImg << std::endl;
				if (vh > vhImg) {
					isCanon = false;
				}
			});
			chain = stab.getChain();
		} else {
			const auto genPtrs = chain->generator_ptrs();
			perm_group::orbit(vpCore, genPtrs.begin(), genPtrs.end(),
			                  [&] (int w, int img,
			                        decltype(genPtrs.begin())) {
				//auto vhImg = get(match, gRule, gHost, img);
				const auto vpImg = dpoRule.coreVertexToLeftComponentVertex[img];
				//assert(vpImg.first == compIndex);
				if (vpImg.first != compIndex) {
					return;
				}
				auto vhImg = get(match, compPatternGraph, gHost, vpImg.second);
//				std::cout << "orbit: " << w << ", " << img << ", vh: " << vhImg << std::endl;
//				std::cout << "orbit: (" << compIndex << ", " << vp << ") (" << vpImg.first << ", " << vpImg.second << ") | " << vh << " "<< vhImg << std::endl;
				if (vh > vhImg) {
					isCanon = false;
				}
			});
			if (chain->get_next()) {
//				std::cout << "FIXING " << vp << std::endl;
				chain = chain->get_next();
			}
		}
		if (!isCanon) {
			return false;
		}
	}
	return true;
}

CanonMatch::CanonMatch(const std::vector<const Graph::Single *>& hosts,
                       const Rules::Real& rule, LabelSettings ls): hosts(hosts), rule(rule),
                        hostFixes(hosts.size()), pushedStack(), labelSettings(ls),
                        prevIsoComp(rule.getDPORule().leftComponentGraphs.size(), -1),
                        prevIsoVertex(rule.getDPORule().leftComponentGraphs.size()) {
	const auto& dpoRule = rule.getDPORule();
	const auto& lgRule = get_labelled_left(dpoRule);
	const auto gens = rule.getAutGroup().generator_ptrs();
	for (size_t i = 0; i < rule.getDPORule().leftComponentGraphs.size(); ++i) {
		const auto& gRule = get_graph(get_component_graph_v2(i, lgRule));
		const auto vpCore = get_component_core_vertex(i, 0, lgRule);
		perm_group::orbit(vpCore, gens.begin(), gens.end(),
		                  [&] (int w, int img, decltype(gens.begin())) {
			const auto vpImg = dpoRule.coreVertexToLeftComponentVertex[img];
			if (vpImg.first > i && static_cast<int>(i) > prevIsoComp[vpImg.first]) {
				prevIsoComp[vpImg.first] = static_cast<int>(i);
				prevIsoVertex[vpImg.first].clear();
			}
			if (prevIsoComp[vpImg.first] == i) {
				prevIsoVertex[vpImg.first].push_back(vpImg.second);
			}
		});
//		std::cout << prevIsoComp[i] << "; ";
	}
//	for (int i : prevIsoComp) {
//		std::cout << i << "; ";
//	}
//	std::cout << std::endl;
//	for (const auto& verts : prevIsoVertex) {
//		std::cout << verts.size() << "; ";

//	}
//	std::cout << std::endl;
}

bool CanonMatch::isIsoCompsCorrectlyOrdered(const ComponentMatch& cm, size_t hostIndex, bool isAdded) {
	if (prevIsoComp[cm.componentIndex] == -1) {
		return true;
	}

	const ComponentMatch& prevCm = *pushedStack[prevIsoComp[cm.componentIndex]].cm;
	const size_t prevHostIndex = pushedStack[prevIsoComp[cm.componentIndex]].hid;
	if (cm.host->getId() < prevCm.host->getId()) {
		return (prevHostIndex == 0 && !cm.isSubsetHost);  //ignore the ordering if its not part of the subset
	}


	if (cm.host->getId() == prevCm.host->getId() && prevHostIndex > hostIndex) {
		return false;
	} else if (prevHostIndex == hostIndex) {
		const auto& dpoRule = rule.getDPORule();
		const auto& lgRule = get_labelled_left(dpoRule);;
		const auto& gCurRule = get_graph(get_component_graph_v2(cm.componentIndex, lgRule));
		const auto& gPrevRule = get_graph(get_component_graph_v2(prevCm.componentIndex, lgRule));
		const auto& gHost = get_graph(cm.host->getLabelledGraph());

		auto vhPrev = get(*prevCm.morphism, gPrevRule, gHost, 0);
		for (const size_t vpCand : prevIsoVertex[cm.componentIndex]) {
			auto vhCand = get(*cm.morphism, gCurRule, gHost, vpCand);
			if (vhCand  < vhPrev) {
				return false;
			}
		}
		//assert(vhCur != vhPrev);
//		std::cout << "isos are mapped to the same graph: " << vhCur << ", " << vhPrev << std::endl;

	}

	return true;

}

bool CanonMatch::pushFastCombined(const ComponentMatch& cm, size_t hostIndex) {
	const auto& host = *hosts[hostIndex];
//	std::cout << "pushFast on " << hostIndex << "/" << hosts.size() << std::endl;
	if (hosts.size() > auts.size()) {
//		std::cout << "pushing new gens" << std::endl;
		const auto& gens = host.getAutGroup(labelSettings.type, labelSettings.withStereo);
		const auto genPtrs = gens.generator_ptrs();
		// the first perm is the identity
		auts.emplace_back(genPtrs.begin()+1, genPtrs.end());
		autSize.push_back(auts[hostIndex].size());
		assert(hosts.size() == auts.size());
		assert(hosts.size() == autSize.size());
		assert(hostIndex == hosts.size() - 1);
	}

	auto& validAuts = auts[hostIndex];
	size_t& validAutSize = autSize[hostIndex];

	pushedStack.push_back(PushedFixes{hostIndex, validAutSize, &cm});
	if (validAutSize == 0) {
		return true;
	}
	const auto& constRuleAutPtrs = rule.getAutGroup().generator_ptrs();
	std::vector<Rules::AutGroup::const_pointer> gensRule(constRuleAutPtrs.begin(), constRuleAutPtrs.end());
	std::size_t gensSizeRule = gensRule.size();

	const auto& dpoRule = rule.getDPORule();
	const auto& lgRule = get_labelled_left(dpoRule);;
	const auto& gRule = get_graph(get_component_graph_v2(cm.componentIndex, lgRule));
	const auto& gHost = get_graph(cm.host->getLabelledGraph());
//	std::cout << "CHECKING CANONICITY with " << validAutSize << " auts" << std::endl;

	for (auto vp : asRange(vertices(gRule))) {
		auto vh = get(*cm.morphism, gRule, gHost, vp);
		const auto vpCore = get_component_core_vertex(cm.componentIndex, vp, lgRule);
//		std::cout << vp << ": " << vh << std::endl;
		for (size_t idxAutRule = 0; idxAutRule < gensSizeRule; idxAutRule++) {
			const auto& autRule = gensRule[idxAutRule];
			size_t vpCoreImg =  autRule->at(vpCore);
			const auto vpImg = dpoRule.coreVertexToLeftComponentVertex[vpCoreImg];
			if (vpImg.first != cm.componentIndex) { continue; }

			auto vhCandImg = get(*cm.morphism, gRule, gHost, vpImg.second);

			for (size_t idxAutHost = 0; idxAutHost < validAutSize; idxAutHost++) {
				const auto& autHost = validAuts[idxAutHost];
				size_t vhImg =  autHost->at(vhCandImg);
				if (vh > vhImg) {
					return false;
				}

			}
		}
		size_t curAutIndex = 0;
		while (curAutIndex < validAutSize) {
			assert(validAuts[curAutIndex]->size() == num_vertices(gHost));
			assert(validAuts[curAutIndex]->size()  > vh);
			size_t vhImg =  perm_group::get(*validAuts[curAutIndex], vh);
			if (vh > vhImg) {
				return false;
			}
			if (vh != vhImg) {
				assert(static_cast<int>(validAutSize) >= 0);
				std::swap(validAuts[curAutIndex], validAuts[validAutSize - 1]);
				validAutSize -= 1;
			} else {
				curAutIndex += 1;
			}
		}
		curAutIndex = 0;
		while (curAutIndex < gensSizeRule) {
			size_t img =  gensRule[curAutIndex]->at(vpCore);
			if (img != vpCore) {
				assert(static_cast<int>(gensSizeRule) >= 0);
				std::swap(gensRule[curAutIndex], gensRule[gensSizeRule - 1]);
				gensSizeRule -= 1;
			} else {
				curAutIndex += 1;
			}
		}
		if (validAutSize == 0)  {
			return true;
		}
	}

	return true;
}

bool CanonMatch::pushFast(const ComponentMatch& cm, size_t hostIndex) {
	const auto& host = *hosts[hostIndex];
//	std::cout << "pushFast on " << hostIndex << "/" << hosts.size() << std::endl;
	if (hosts.size() > auts.size()) {
//		std::cout << "pushing new gens" << std::endl;
		const auto& gens = host.getAutGroup(labelSettings.type, labelSettings.withStereo);
		const auto genPtrs = gens.generator_ptrs();
		// the first perm is the identity
		auts.emplace_back(genPtrs.begin()+1, genPtrs.end());
		autSize.push_back(auts[hostIndex].size());
		assert(hosts.size() == auts.size());
		assert(hosts.size() == autSize.size());
		assert(hostIndex == hosts.size() - 1);
	}

	auto& validAuts = auts[hostIndex];
	size_t& validAutSize = autSize[hostIndex];

	pushedStack.push_back(PushedFixes{hostIndex, validAutSize, &cm});
	if (validAutSize == 0) {
		return true;
	}

	const auto& dpoRule = rule.getDPORule();
	const auto& lgRule = get_labelled_left(dpoRule);;
	const auto& gRule = get_graph(get_component_graph_v2(cm.componentIndex, lgRule));
	const auto& gHost = get_graph(cm.host->getLabelledGraph());
//	std::cout << "CHECKING CANONICITY with " << validAutSize << " auts" << std::endl;

	for (auto vp : asRange(vertices(gRule))) {
		auto vh = get(*cm.morphism, gRule, gHost, vp);
//		std::cout << vp << ": " << vh << std::endl;
		size_t curAutIndex = 0;
		while (curAutIndex < validAutSize) {
//			const auto& aut = *validAuts[curAutIndex];
//			for (size_t i = 0; i < aut.size(); ++i) { std::cout << aut[i] << " "; }
//			std::cout << std::endl;
//			std::cout << validAuts[curAutIndex]->size() << " " << num_vertices(gHost) << std::endl;
			assert(validAuts[curAutIndex]->size() == num_vertices(gHost));
			assert(validAuts[curAutIndex]->size()  > vh);
			//size_t vhImg =  validAuts[curAutIndex]->at(vh);
			size_t vhImg =  perm_group::get(*validAuts[curAutIndex], vh);
//			std::cout << "\t" << vhImg << std::endl;
			if (vh > vhImg) {
				 //std::cout << "\t" << vh << " is not canon" << std::endl;
				return false;
			}
			if (vh != vhImg) {
//				std::cout << "does not fix vh, so we discard " << vh << " " << vhImg << std::endl;
				assert(static_cast<int>(validAutSize) >= 0);
				std::swap(validAuts[curAutIndex], validAuts[validAutSize - 1]);
				validAutSize -= 1;
			} else {
				curAutIndex += 1;
			}
		}
		if (validAutSize == 0)  {
			// std::cout << "\tNO MORE AUTS" << std::endl;
			return true;
		}
		// std::cout << "\tFixed " << vh << " remaining auts: " << validAutSize << std::endl;
	}

	return true;
}

bool CanonMatch::pushCombined(const ComponentMatch& cm, size_t hostIndex) {
//	std::cout << "im combining" << std::endl;
	if (hosts.size() > hostFixes.size()) {
		hostFixes.emplace_back();
		assert(hosts.size() == hostFixes.size());
	}
	assert(hostIndex < hostFixes.size());
	assert(cm.host == hosts[hostIndex]);

	auto& fixVec = hostFixes[hostIndex];
	size_t alreadyFixed = fixVec.size();
	const Graph::Single *host = hosts[hostIndex];

	const auto& dpoRule = rule.getDPORule();
//	const auto& dpoHost = host->getBindRule()->getRule().getDPORule();
	const auto& lgRule = get_labelled_left(dpoRule);;
//	const auto& lgHost = get_labelled_right(dpoHost);
	const auto& gRule = get_graph(get_component_graph_v2(cm.componentIndex, lgRule));
//	const auto& gHost = get_graph(lgHost);
	const auto& gHost = get_graph(cm.host->getLabelledGraph());
	for (auto vp : asRange(vertices(gRule))) {
		auto vh = get(*cm.morphism, gRule, gHost, vp);
		fixVec.push_back(vh);
	}
	pushedStack.push_back(PushedFixes{hostIndex, alreadyFixed, &cm});
//	std::cout << "PUSHING HIST: " << hostIndex << ", " << alreadyFixed << std::endl;


	using Perm = std::vector<int>;
	using Alloc = perm_group::raw_ptr_allocator<Perm>;
	using Transversal = perm_group::transversal_explicit<Alloc>;

	HostSchreierGroup hsg(*host, fixVec, labelSettings);
	for (size_t i = 0; i < alreadyFixed; i++) {
		hsg.next();
		/*if (!hsg.next()) {
			break;
		}*/
	}

	PatternSchreierGroup psg(rule, cm.componentIndex);
	for (auto vp : asRange(vertices(gRule))) {
		const auto vpCore = get_component_core_vertex(cm.componentIndex, vp, lgRule);
		const auto vh = get(*cm.morphism, gRule, gHost, vp);
//		std::cout << "trying " << vp << "(" << vpCore << ") -> " << vh << std::endl;
		const auto patternGenPtrs = psg.generator_ptrs();
		bool isCanon = true;
		perm_group::orbit(vpCore, patternGenPtrs.begin(), patternGenPtrs.end(),
		                  [&] (int w, int vpCoreImg,
		                        decltype(patternGenPtrs.begin())) {
			if (!isCanon) {return;}
			const auto vpImg = dpoRule.coreVertexToLeftComponentVertex[vpCoreImg];
			if (vpImg.first != cm.componentIndex) {
				return;
			}
			const auto vhImg = get(*cm.morphism, gRule, gHost, vpImg.second);
			const auto genPtrs = hsg.generator_ptrs();
//			std::cout << "vp  orbit: "<< vpCoreImg << "(" << vpImg.first << "; " << vpImg.second << ")" << std::endl;
			perm_group::orbit(vhImg, genPtrs.begin(), genPtrs.end(),
			                  [&] (int w, int img,
			                        decltype(genPtrs.begin())) {
//				std::cout << "\t vh orbit: " << w << ", " << img << std::endl;
				if (vh > img) {
//					std::cout << "is not canonical since" << vh << ">" << img << std::endl;
					isCanon = false;
				}
			});

		});
		if (!isCanon) {
			return false;
		}
		psg.next();
		hsg.next();
	}
//	std::cout << "is canonical" << std::endl;
	return true;
}

bool CanonMatch::push(const ComponentMatch& cm, size_t hostIndex) {
	if (hosts.size() > hostFixes.size()) {
		hostFixes.emplace_back();
		assert(hosts.size() == hostFixes.size());
	}
	assert(hostIndex < hostFixes.size());
	assert(cm.host == hosts[hostIndex]);

	auto& fixVec = hostFixes[hostIndex];
	size_t alreadyFixed = fixVec.size();
	const Graph::Single *host = hosts[hostIndex];

	const auto& dpoRule = rule.getDPORule();
//	const auto& dpoHost = host->getBindRule()->getRule().getDPORule();
	const auto& lgRule = get_labelled_left(dpoRule);;
//	const auto& lgHost = get_labelled_right(dpoHost);
	const auto& gRule = get_graph(get_component_graph_v2(cm.componentIndex, lgRule));
//	const auto& gHost = get_graph(lgHost);
	const auto& gHost = get_graph(cm.host->getLabelledGraph());
	for (auto vp : asRange(vertices(gRule))) {
		auto vh = get(*cm.morphism, gRule, gHost, vp);
		fixVec.push_back(vh);
	}
	pushedStack.push_back(PushedFixes{hostIndex, alreadyFixed, &cm});
//	std::cout << "PUSHING HIST: " << hostIndex << ", " << alreadyFixed << std::endl;

	using Perm = std::vector<int>;
	using Alloc = perm_group::raw_ptr_allocator<Perm>;
	using Transversal = perm_group::transversal_explicit<Alloc>;

	struct fixed_vertices {
		fixed_vertices(const std::vector<int>& fixes): fixes(fixes) {}
		int operator()(const Perm &p) {
			if (curFix == fixes.size()) {
				return bpp(p);
			} else {
				curFix += 1;
				return fixes[curFix - 1];
			}
		}

		size_t curFix = 0;
		const std::vector<int>& fixes;
		perm_group::base_point_first_moved bpp;
	};
	const auto& gens = host->getAutGroup(labelSettings.type, labelSettings.withStereo);
	perm_group::GeneratingSystem<Transversal, fixed_vertices> stab(gens.get_allocator(),
	                                                               fixed_vertices(fixVec));
	for (const auto& gen : gens.generators()) {
		stab.add_generator(gen);
	}

	const perm_group::GeneratingSystem<Transversal, fixed_vertices>::Chain *chain = nullptr;
	for (size_t i = 0; i < alreadyFixed; i++) {
		if (chain) {
			if (chain->get_next()) {
				chain = chain->get_next();
			} else {
				break;
			}
		} else {
			chain = stab.getChain();
			if (!chain) {
				break;
			}
		}
	}
	for (auto vp : asRange(vertices(gRule))) {
//		std::cout << "-------" << std::endl;
		auto vh = get(*cm.morphism, gRule, gHost, vp);
		bool isCanon = true;
		if (chain == nullptr) {
			const auto genPtrs = gens.generator_ptrs();
			perm_group::orbit(vh, genPtrs.begin(), genPtrs.end(),
			                  [&] (int w, int img,
			                        decltype(genPtrs.begin())) {
//				std::cout << "orbit: " << w << ", " << img << std::endl;
				if (vh > img) {
					isCanon = false;
				}
			});
			chain = stab.getChain();
		} else {
			const auto genPtrs = chain->generator_ptrs();
			perm_group::orbit(vh, genPtrs.begin(), genPtrs.end(),
			                  [&] (int w, int img,
			                        decltype(genPtrs.begin())) {
//				std::cout << "orbit: " << w << ", " << img << std::endl;
				if (vh > img) {
					isCanon = false;
				}
			});
			if (chain->get_next()) {
//				std::cout << "FIXING " << vh << std::endl;
				chain = chain->get_next();
			}
		}
		if (!isCanon) {
			return false;
		}
	}
	return true;
}

void CanonMatch::popFast() {
//	std::cout << "POPPING FAST " << hosts.size() << " " << auts.size() << std::endl;
	assert(pushedStack.size() > 0);
	const PushedFixes& pf = pushedStack.back();
	assert(pf.hid < auts.size() && pf.hid < autSize.size());
	autSize[pf.hid] = pf.oldSize;
	pushedStack.pop_back();

	if (hosts.size() < auts.size()) {
		autSize.pop_back();
		auts.pop_back();
	}
	assert(hosts.size() == auts.size());
	assert(hosts.size() == autSize.size());

}

void CanonMatch::pop() {
	assert(pushedStack.size() > 0);
	const PushedFixes& pf = pushedStack.back();
	assert(pf.hid < hostFixes.size());
	assert(hostFixes[pf.hid].size() > 0);
	hostFixes[pf.hid].resize(pf.oldSize);
	pushedStack.pop_back();

	if (hosts.size() > hostFixes.size()) {
		hostFixes.pop_back();
		assert(hosts.size() == hostFixes.size());
	}
}

}
