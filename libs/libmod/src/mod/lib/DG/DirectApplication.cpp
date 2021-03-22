#include <mod/lib/DG/DirectApplication.hpp>

#include <mod/lib/IO/IO.hpp>
#include <mod/lib/LabelledUnionGraph.hpp>
#include <mod/lib/Rules/GraphToRule.hpp>
#include <mod/lib/RC/ComposeRuleRealGeneric.hpp>
#include <mod/rule/Rule.hpp>
#include <mod/lib/DG/RuleApplicationUtils.hpp>

namespace mod::lib::DG {

class ComponentMonomorphism {
	// namespace GM = jla_boost::GraphMorphism;
	using Morphism = jla_boost::GraphMorphism::VectorVertexMap<Rules::LabelledRule::LeftGraphType, Rules::LabelledRule::RightGraphType>;
public:
	ComponentMonomorphism(Morphism &&m, size_t hid, size_t cid) : m(std::move(m)), componentId(cid), hostId(hid) {
	}
	Morphism m;
	size_t componentId;
	size_t hostId;
};

template <typename VertexMap>
bool isValidComponentMap(const Rules::LabelledRule& rHosts,
                         const Rules::LabelledRule& rPatterns,
                         size_t patternId,
                         const VertexMap& map,
                         IO::Logger& logger) {
	    using jla_boost::GraphDPO::Membership;
	    const auto &lgPatterns = get_labelled_left(rPatterns);

		const auto &lgHosts = get_labelled_right(rHosts);
		const auto &gHosts = get_graph(rHosts);

		size_t cid = patternId;
		const auto &gp = get_component_graph(cid, lgPatterns);

		using LeftGraphType = Rules::LabelledRule::LeftGraphType;
		using RightGraphType = Rules::LabelledRule::RightGraphType;
		const auto vNullHosts = boost::graph_traits<RightGraphType>::null_vertex();
//		const auto vNullPatterns = boost::graph_traits<LeftGraphType>::null_vertex();

		const auto &gRightPatterns = get_graph(get_labelled_right(rPatterns));

//		logger.indent() << "-----" << std::endl;
		for (const auto vpSrc : asRange(vertices(gp))) {
			const auto vhSrc = get(map, get_graph(lgPatterns), get_graph(lgHosts), vpSrc);
//			logger.indent() << "vpSrc " << vpSrc << ", vhSrc"  << vhSrc << std::endl;
			assert(vhSrc != vNullHosts);
			for (auto ep :  asRange(out_edges(vpSrc, gRightPatterns))) {
				const auto vpTar = target(ep, gRightPatterns);
				const auto epMember = membership(rPatterns, ep);
				if (epMember == Membership::Context) {
					continue;
				}
				const auto vhTar = get(map, get_graph(lgPatterns), get_graph(lgHosts), vpTar);

				if (vhTar == vNullHosts) {
//					logger.indent() << "Have not mapped " << vpTar << " to host." << std::endl;
					continue;
				}

				if (edge(vhSrc, vhTar, gHosts).second) {
//					logger.indent() << "Invalid map: Results in parallel edges." << std::endl;
					return false;
				}
			}
		}
		return true;
}

class CompositionMap {
	using LeftGraphType = Rules::LabelledRule::LeftGraphType;
	using RightGraphType = Rules::LabelledRule::RightGraphType;
	using InvertibleVertexMap = jla_boost::GraphMorphism::InvertibleVectorVertexMap<LeftGraphType, RightGraphType>;
public:
	CompositionMap(const Rules::Real& rHosts, const Rules::Real& rPatterns, IO::Logger& logger):
	        rHosts(rHosts), rPatterns(rPatterns),
	        m(get_graph(get_labelled_left(rPatterns.getDPORule())),
	          get_graph(get_labelled_right(rHosts.getDPORule()))),
	        logger(logger) {
	}

	bool addComponentMap(const ComponentMonomorphism& partialMap) {
		const auto &lgPatterns = get_labelled_left(rPatterns.getDPORule());
		const auto &gPatterns = get_graph(lgPatterns);
		const auto &lgHosts = get_labelled_right(rHosts.getDPORule());
		const auto &gHosts = get_graph(lgHosts);
		const auto vNullHosts = boost::graph_traits<RightGraphType>::null_vertex();

		const auto &gp = get_component_graph(partialMap.componentId, lgPatterns);
		for (const auto vp : asRange(vertices(gp))) {
			assert(get(m, gPatterns, gHosts, vp) == boost::graph_traits<LeftGraphType>::null_vertex());
			const auto vh = get(partialMap.m, gPatterns, gHosts, vp);
			assert(vh != vNullHosts);
			if (get_inverse(m, gPatterns, gHosts, vh) != vNullHosts) {
				return false;
			}
			put(m, gHosts, gPatterns, vp, vh);
//			std::cout << "mapping " << v << " -> " << vDomHost << std::endl;
		}
		return isValidComponentMap(rHosts.getDPORule(),
		                         rPatterns.getDPORule(),
		                         partialMap.componentId,
		                         m,
		                         logger);
	}

	void removeComponentMap(const ComponentMonomorphism& partialMap) {
		const auto &lgPatterns = get_labelled_left(rPatterns.getDPORule());
		const auto &gPatterns = get_graph(lgPatterns);
		const auto &lgHosts = get_labelled_right(rHosts.getDPORule());
		const auto &gHosts = get_graph(lgHosts);
		const auto vNullHosts = boost::graph_traits<RightGraphType>::null_vertex();

		const auto &gp = get_component_graph(partialMap.componentId, lgPatterns);
		for (const auto vp : asRange(vertices(gp))) {
			put(m, gPatterns, gHosts, vp, vNullHosts);
//			std::cout << "removing " << v << " -> " << vNullDom << std::endl;
		}
	}

	std::unique_ptr<Rules::Real> compose() {
		using HasTerm = GraphMorphism::HasTermData<InvertibleVertexMap>;
		using HasStereo = GraphMorphism::HasStereoData<InvertibleVertexMap>;
		constexpr LabelType labelType = HasTerm::value ? LabelType::Term : LabelType::String;
		return RC::composeRuleRealByMatch<labelType, HasStereo::value>(
		            rHosts, rPatterns, m, false, logger);

	}

	const InvertibleVertexMap& get_map() const {
		return m;
	}

	friend std::ostream& operator<<(std::ostream& os, const CompositionMap& cm) {
		const auto &lgPatterns = get_labelled_left(cm.rPatterns.getDPORule());
		const auto &gPatterns = get_graph(lgPatterns);
		const auto &lgHosts = get_labelled_right(cm.rHosts.getDPORule());
		const auto &gHosts = get_graph(lgHosts);
		os << "CompositionMap(";
		for (const auto vp : asRange(vertices(gPatterns))) {
			auto vh = get(cm.m, gPatterns, gHosts, vp);
			os << vp << " -> " << vh << ", ";
		}
		os << ")";
		return os;
	}

private:
	const Rules::Real& rHosts;
	const Rules::Real& rPatterns;
	InvertibleVertexMap m;
	IO::Logger logger;
};


std::vector<std::vector<ComponentMonomorphism>> computeMorphisms(const Rules::Real& rHosts,
                                                                 const Rules::Real& rPatterns,
                                                                 const LabelSettings& labelSettings,
                                                                 IO::Logger& logger)  {

	size_t nPatterns = rPatterns.getDPORule().numLeftComponents;
	size_t nHosts = rHosts.getDPORule().numRightComponents;
	std::vector<std::vector<ComponentMonomorphism>> maps(nPatterns);


	const auto& patterns = get_labelled_left(rPatterns.getDPORule());
	const auto& hosts = get_labelled_right(rHosts.getDPORule());
	auto mm = RC::makeRuleRuleComponentMonomorphism(patterns, hosts, true, labelSettings, 0, logger);

	for (size_t pid = 0; pid < nPatterns; pid++) {
		for (size_t hid = 0; hid < nHosts; hid++) {
			auto morphisms = mm(pid, hid);
			for (auto& m : morphisms) {
				maps[pid].push_back(ComponentMonomorphism(std::move(m), hid, pid));
				bool isValid = isValidComponentMap(rHosts.getDPORule(),
				                                   rPatterns.getDPORule(),
				                                   maps[pid].back().componentId,
				                                   maps[pid].back().m, logger);
				if (!isValid) {
					maps[pid].pop_back();
				}
			}
		}
	}
	return maps;
}

std::vector<std::unique_ptr<Rules::Real>> composeRules(const Rules::Real& rHosts,
                                                       const Rules::Real& rPatterns,
                                                       const std::vector<std::vector<ComponentMonomorphism>>& maps,
                                                       IO::Logger& logger) {
	std::vector<std::unique_ptr<Rules::Real>> rDerivations;
	CompositionMap compositionMap(rHosts, rPatterns, logger);

	std::vector<size_t> stack;
	std::vector<int> used_graphs(rHosts.getDPORule().numRightComponents, 0);
	stack.reserve(maps.size());
	stack.push_back(0);

	while (stack.size() > 0) {
//		std::cout << compositionMap << std::endl;
		size_t cid = stack.size() - 1;
		size_t mid = stack.back();
		if (mid == maps[cid].size()) {
			stack.pop_back();
//			std::cout << "Popping map: " << cid << ", " << mid << std::endl;
			if (stack.size() > 0) {
				compositionMap.removeComponentMap(maps[cid-1][stack.back()]);
				used_graphs[maps[cid-1][stack.back()].hostId] -= 1;
				assert(used_graphs[maps[cid-1][stack.back()].hostId] >= 0);
				stack.back() += 1;
			}
			continue;
		}
//		std::cout << "Adding map: " << cid << ", " << mid << std::endl;
		bool success = compositionMap.addComponentMap(maps[cid][mid]);
		if (success)  {
			used_graphs[maps[cid][mid].hostId] += 1;
			if (stack.size() == maps.size()){
				bool uses_all_hosts = true;
				for (int b : used_graphs) {
					if (b == 0) {
						uses_all_hosts = false;
						break;
					}
				}
				if (uses_all_hosts) {

//					std::cout << "Found Full Map" << std::endl;
//					std::cout << compositionMap << std::endl;
					auto res = compositionMap.compose();
					if (res) {
						rDerivations.push_back(std::move(res));
					}
				}
				compositionMap.removeComponentMap(maps[cid][mid]);
				used_graphs[maps[cid][mid].hostId] -= 1;
				assert(used_graphs[maps[cid][mid].hostId] >= 0);
				stack.back() += 1;
			} else {
				stack.push_back(0);
			}
		} else {
			compositionMap.removeComponentMap(maps[cid][mid]);
			stack.back() += 1;
		}

	}

	return rDerivations;

}

std::vector<std::unique_ptr<Rules::Real>>
directApply(const std::vector<std::shared_ptr<graph::Graph>> &graphs,
                     std::shared_ptr<rule::Rule> rOrig, LabelSettings labelSettings,
                     int verbosity, IO::Logger& logger) {


	LabelledUnionGraph<lib::Graph::LabelledGraph> ugHosts;
	for (const auto& g: graphs) {
		ugHosts.push_back(&g->getGraph().getLabelledGraph());
	}
	const std::unique_ptr<Rules::Real> rHosts = lib::Rules::graphToRule(ugHosts, lib::Rules::Membership::Right, "G");
	const Rules::Real& rPatterns = rOrig->getRule();

	const auto maps = computeMorphisms(*rHosts, rPatterns, labelSettings, logger);
	auto rDerivations = composeRules(*rHosts, rPatterns, maps, logger);

	return rDerivations;
}

}
