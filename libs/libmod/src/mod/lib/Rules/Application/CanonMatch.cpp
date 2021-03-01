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
                        hostFixes(hosts.size()), pushedStack(), labelSettings(ls) {}

bool CanonMatch::pushFast(const ComponentMatch& cm, size_t hostIndex) {
	const auto& host = *hosts[hostIndex];
	if (hosts.size() > auts.size()) {
		const auto& gens = host.getAutGroup(labelSettings.type, labelSettings.withStereo);
		const auto genPtrs = gens.generator_ptrs();
		auts.emplace_back(genPtrs.begin(), genPtrs.end());
		autSize.push_back(auts[hostIndex].size());
		assert(hosts.size() == auts.size());
		assert(hosts.size() == autSize.size());
	}

	const auto& validAuts = auts[hostIndex];
	size_t validAutSize = autSize[hostIndex];

	pushedStack.push_back(PushedFixes{hostIndex, validAutSize});
	if (validAutSize == 0) {
		return true;
	}

	const auto& dpoRule = rule.getDPORule();
	const auto& lgRule = get_labelled_left(dpoRule);;
	const auto& gRule = get_graph(get_component_graph_v2(cm.componentIndex, lgRule));
	const auto& gHost = get_graph(cm.host->getLabelledGraph());

	for (auto vp : asRange(vertices(gRule))) {
		auto vh = get(*cm.morphism, gRule, gHost, vp);
	}

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
	pushedStack.push_back(PushedFixes{hostIndex, alreadyFixed});
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
