#include <mod/lib/statespace/CanonMatch.hpp>
#include <perm_group/group/schreier_stabilizer.hpp>
#include <perm_group/transversal/explicit.hpp>
#include <mod/lib/statespace/GeneratingSystem.hpp>
#include <mod/lib/Rules/Real.hpp>
#include <mod/rule/Rule.hpp>
#include <mod/lib/Rules/GraphAsRuleCache.hpp>

#include <perm_group/orbit.hpp>

namespace mod::lib::statespace {

CanonMatch::CanonMatch(const std::vector<const Graph::Single *>& hosts,
                       const Rules::Real& rule, LabelSettings ls): hosts(hosts), rule(rule),
                        hostFixes(hosts.size()), pushedStack(), labelSettings(ls) {}

bool CanonMatch::push(const ComponentMatch& cm, lib::Rules::GraphAsRuleCache &graphAsRule) {
	assert(cm.getHostId() < hostFixes.size());

	auto& fixVec = hostFixes[cm.getHostId()];
	size_t alreadyFixed = fixVec.size();
	const Graph::Single *host = hosts[cm.getHostId()];

	const auto& dpoRule = rule.getDPORule();
	const auto& dpoHost = graphAsRule.getBindRule(host)->getRule().getDPORule();
	const auto& lgRule = get_labelled_left(dpoRule);;
	const auto& lgHost = get_labelled_right(dpoHost);
	const auto& gRule = get_graph(lgRule);
	const auto& gHost = get_graph(lgHost);
	for (auto vp : asRange(vertices(get_component_graph(cm.getPatternId(), lgRule)))) {
		auto vh = get(cm.match, gRule, gHost, vp);
		fixVec.push_back(vh);
	}
	pushedStack.push_back(PushedFixes{cm.getHostId(), alreadyFixed});
//	std::cout << "PUSHING HIST: " << cm.getHostId() << ", " << alreadyFixed << std::endl;

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
	for (auto vp : asRange(vertices(get_component_graph(cm.getPatternId(), lgRule)))) {
//		std::cout << "-------" << std::endl;
		auto vh = get(cm.match, gRule, gHost, vp);
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
}

}
