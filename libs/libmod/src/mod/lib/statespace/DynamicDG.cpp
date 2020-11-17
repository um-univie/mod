#include <mod/lib/statespace/DynamicDG.hpp>
#include <mod/lib/statespace/ComponentMap.hpp>
#include <mod/lib/Rules/Real.hpp>
#include <mod/lib/RC/MatchMaker/ComponentWiseUtil.hpp>
#include <mod/lib/Graph/LabelledGraph.hpp>
#include <mod/lib/LabelledUnionGraph.hpp>
#include <mod/lib/Rules/GraphToRule.hpp>
#include <mod/lib/DG/RuleApplicationUtils.hpp>
#include <mod/lib/IO/IO.hpp>
#include <mod/rule/Rule.hpp>
#include <iostream>
#include <mod/lib/DG/RuleApplicationUtils.hpp>
#include <perm_group/group/schreier_stabilizer.hpp>
#include <perm_group/transversal/explicit.hpp>
#include <mod/lib/statespace/GeneratingSystem.hpp>
#include <mod/lib/statespace/CanonRule.hpp>

#include <perm_group/orbit.hpp>

#include <mod/lib/RC/ComposeRuleRealGeneric.hpp>

namespace mod::lib::statespace {

void createLHSGraphs(const Rules::Real& rRule) {

}

void printKey(const DG::GraphMultiset& key) {
	std::cout << "Key: ";
	for (const Graph::Single *g : key) {
		std::cout << g->getName() << ", ";
	}
}

struct DerivationRule {
	std::vector<const lib::Graph::Single *> leftGraphs;
	std::unique_ptr<Rules::Real> rDerivation;
};

class CompositionMap {
	using LeftGraphType = Rules::LabelledRule::LeftGraphType;
	using RightGraphType = Rules::LabelledRule::RightGraphType;
	using InvertibleVertexMap = jla_boost::GraphMorphism::InvertibleVectorVertexMap<LeftGraphType, RightGraphType>;
public:
	CompositionMap(const std::vector<Graph::Single *>& graphs,
	               const Rules::Real& rHosts,
	               const Rules::Real& rPatterns,
	               IO::Logger& logger):
	        graphs(graphs),
	        rHosts(rHosts), rPatterns(rPatterns),
	        m(get_graph(get_labelled_left(rPatterns.getDPORule())),
	          get_graph(get_labelled_right(rHosts.getDPORule()))),
	        logger(logger), timesGraphAdded(graphs.size(), 0) {
	}

	bool addComponentMap(const ComponentMap& partialMap) {
		std::cout << "Adding Component Map: " << partialMap.patternId << ", " << partialMap.hostId << std::endl;
		const auto &lgPatterns = get_labelled_left(rPatterns.getDPORule());
		const auto &gPatterns = get_graph(lgPatterns);
		const auto &lgHosts = get_labelled_right(rHosts.getDPORule());
		const auto &gHosts = get_graph(lgHosts);
		const auto vNullHosts = boost::graph_traits<RightGraphType>::null_vertex();
		const auto vNullPatterns = boost::graph_traits<LeftGraphType>::null_vertex();

		timesGraphAdded[partialMap.hostId] += 1;
		const auto &gp = get_component_graph(partialMap.patternId, lgPatterns);
		for (const auto vp : asRange(vertices(gp))) {
//			std::cout << vp << " >> " << get(m, gPatterns, gHosts, vp) << std::endl;
			assert(get(m, gPatterns, gHosts, vp) == vNullPatterns);
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
		                         partialMap.patternId,
		                         m,
		                         logger);
	}

	void removeComponentMap(const ComponentMap& partialMap) {
		const auto &lgPatterns = get_labelled_left(rPatterns.getDPORule());
		const auto &gPatterns = get_graph(lgPatterns);
		const auto &lgHosts = get_labelled_right(rHosts.getDPORule());
		const auto &gHosts = get_graph(lgHosts);
		const auto vNullHosts = boost::graph_traits<RightGraphType>::null_vertex();
		const auto vNullPatterns = boost::graph_traits<LeftGraphType>::null_vertex();

		timesGraphAdded[partialMap.hostId] -= 1;

		const auto &gp = get_component_graph(partialMap.patternId, lgPatterns);
		for (const auto vp : asRange(vertices(gp))) {
			put(m, gPatterns, gHosts, vp, vNullHosts);
		}
	}

	DerivationRule compose() {
		std::vector<size_t> copyVertices;
		std::vector<const Graph::Single  *> lhs;
		const auto &lgHosts = get_labelled_right(rHosts.getDPORule());
		for (size_t i = 0; i < graphs.size(); i++) {
			if (timesGraphAdded[i] > 0)  {
				lhs.push_back(graphs[i]);
				const auto &gh = get_component_graph(i, lgHosts);
				for (const auto vh : asRange(vertices(gh))) {
					copyVertices.push_back(vh);
				}
			}
		}

		using HasTerm = GraphMorphism::HasTermData<InvertibleVertexMap>;
		using HasStereo = GraphMorphism::HasStereoData<InvertibleVertexMap>;
		constexpr LabelType labelType = HasTerm::value ? LabelType::Term : LabelType::String;
		auto rDer =  RC::composeRuleRealByMatch<labelType, HasStereo::value>(
		            rHosts, rPatterns, m, false, logger, &copyVertices);
		return DerivationRule{ std::move(lhs), std::move(rDer) };

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
	const std::vector<Graph::Single *> graphs;
	const Rules::Real& rHosts;
	const Rules::Real& rPatterns;
	InvertibleVertexMap m;
	IO::Logger logger;

	std::vector<size_t> timesGraphAdded;
};


std::vector<std::vector<ComponentMap>> computeMorphisms(const Rules::Real& rHosts,
                                                        const Rules::Real& rPatterns,
                                                        const LabelSettings& labelSettings,
                                                        IO::Logger& logger)  {

	size_t nPatterns = rPatterns.getDPORule().numLeftComponents;
	size_t nHosts = rHosts.getDPORule().numRightComponents;
	std::vector<std::vector<ComponentMap>> maps(nPatterns);


	const auto& patterns = get_labelled_left(rPatterns.getDPORule());
	const auto& hosts = get_labelled_right(rHosts.getDPORule());
	auto mm = RC::makeRuleRuleComponentMonomorphism(patterns, hosts, true, labelSettings, 0, logger);

	for (size_t pid = 0; pid < nPatterns; pid++) {
		for (size_t hid = 0; hid < nHosts; hid++) {
			auto morphisms = mm(pid, hid);
			for (auto& m : morphisms) {
				bool isValid = isValidComponentMap(rHosts.getDPORule(),
				                                   rPatterns.getDPORule(),
				                                   pid, m, logger);


				std::cout << "MAP " << pid << ", " << hid << ": ";
				const auto &gp = get_component_graph(pid, patterns);
				for (const auto vp : asRange(vertices(gp))) {
					const auto vh = get(m, get_graph(patterns), get_graph(hosts), vp);
					std::cout << vp << " -> " << vh << ", ";

				}
				std::cout << std::endl;
				if (isValid) {
					maps[pid].push_back(ComponentMap(std::move(m), pid, hid));
				}
			}
		}
	}
	return maps;
}

std::vector<DerivationRule> composeRules(const std::vector<Graph::Single *>& hosts,
                                                       const Rules::Real& rHosts,
                                                       const Rules::Real& rPatterns,
                                                       const std::vector<std::vector<ComponentMap>>& maps,
                                                       IO::Logger& logger) {
	std::vector<DerivationRule> rDerivations;
	size_t nPatterns = rPatterns.getDPORule().numLeftComponents;
	size_t nHosts = rHosts.getDPORule().numRightComponents;

	for (size_t pid = 0; pid < nPatterns; pid++) {
		if (maps[pid].size() == 0) {
			return rDerivations;
		}
	}

	CompositionMap compositionMap(hosts, rHosts, rPatterns, logger);
	std::vector<size_t> stack;
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
				stack.back() += 1;
			}
			continue;
		}
//		std::cout << "Adding map: " << cid << ", " << mid << std::endl;
		bool success = compositionMap.addComponentMap(maps[cid][mid]);
		if (success)  {
//			std::cout << "SUCCESS" << std::endl;
			if (stack.size() == maps.size()){
				std::cout << "found full" << std::endl;
				std::cout << compositionMap << std::endl;
				auto res = compositionMap.compose();
				assert(res.rDerivation);
				rDerivations.push_back(std::move(res));
				compositionMap.removeComponentMap(maps[cid][mid]);
				stack.back() += 1;
			} else {
//				std::cout << "moving" << std::endl;
				stack.push_back(0);
			}
		} else {
			compositionMap.removeComponentMap(maps[cid][mid]);
			stack.back() += 1;
		}

	}

	return rDerivations;
}

DynamicDG::DynamicDG(DG::Builder& builder, std::vector<const Rules::Real*> rules,
                     LabelSettings labelSettings):
    dgBuilder(builder), rules(rules), labelSettings(labelSettings),
    logger(std::cout) {
	for (const Rules::Real *r : rules)  {
		cachedRules.emplace_back(r, *this);
	}
}


std::vector<DG::NonHyper::Edge>
DynamicDG::findNewEdges(const std::vector<const Graph::Single *>& graphs,
                        const Rules::Real& rHosts,
                        const Rules::Real& rPatterns,
                        std::vector<std::vector<ComponentMatch>>& compMatches,
                        const std::vector<bool>& isNewHost) {

	size_t nPatterns = rPatterns.getDPORule().numLeftComponents;

	std::vector<DG::HyperEdge> edges;
	for (size_t pid = 0; pid < nPatterns; pid++) {
		if (compMatches[pid].size() == 0) {
			return edges;
		}
	}

	struct DirtyMatch {
		const ComponentMatch *cm;
		size_t pid, mid;
	};

	std::vector<std::vector<bool>> isNewMatch(nPatterns);
	std::vector<DirtyMatch> newMatches;
	for (size_t pid = 0; pid < compMatches.size(); pid++) {
		for (size_t mid = 0; mid < compMatches[pid].size();  mid++) {
			isNewMatch[pid].push_back(isNewHost[compMatches[pid][mid].getHostId()]);
			if (isNewHost[compMatches[pid][mid].getHostId()]) {
				newMatches.push_back(DirtyMatch{&compMatches[pid][mid], pid, mid});
				if (verbosity > 0) {
					std::cout << "\t DIRTY MATCH: " << compMatches[pid][mid] << std::endl;
				}
			}
		}
	}
	if (newMatches.size() == 0) {
		return edges;
	}

	assert(newMatches.size() > 0);
	size_t curPid = newMatches[0].pid;
	PartialMatch partialMatch(rHosts, rPatterns, graphs, labelSettings, logger);
	std::vector<size_t> stack;
	for (const auto& dm : newMatches) {
		if (dm.pid > curPid) {
			if (verbosity > 0) {
				std::cout << "\t Filtering Maps\n";
			}
			std::vector<ComponentMatch> filteredMatches;
			for (size_t mid = 0; mid < compMatches[curPid].size(); mid++) {
				if (!isNewMatch[curPid][mid]) {
					filteredMatches.push_back(std::move(compMatches[curPid][mid]));
				}
			}
			if (filteredMatches.size() == 0) {
				if (verbosity > 0) {
					std::cout << "No more untouched maps left!\n";
				}
				break;
			}
			compMatches[curPid] = std::move(filteredMatches);
			curPid = dm.pid;
		}
		bool success = partialMatch.push(*dm.cm);
		if (!success) {
			partialMatch.pop();
			continue;
		}
		stack.push_back(0);
		assert(success);

		if (verbosity > 0) {
			std::cout << "\tAPPLYING DM " << dm.pid << " " << dm.mid << std::endl;
		}
		while (stack.size() > 0) {
			assert(stack.size() > 0);
			size_t pid = stack.size() - 1;
			size_t mid = stack.back();

			if (verbosity > 0) {
				std::cout << "\t\t pid: " << pid << "/" <<compMatches.size() << " mid: " << mid << "/" << compMatches[pid].size()<< std::endl;
				std::cout << partialMatch << std::endl;
			}

			if (pid == dm.pid && mid == 1) {
				if (verbosity > 0) {
					std::cout << "\t\tdone with dirty map!" << std::endl;
				}
				stack.pop_back();
				if (stack.size() > 0) {
					stack.back() += 1;
					partialMatch.pop();
				}
				continue;
			}

			if (mid == compMatches[pid].size()) {
				stack.pop_back();

				if (verbosity > 0) {
					std::cout << "Popping map: " << pid << ", " << mid << std::endl;
				}
				if (stack.size() > 0) {
					if (pid-1 != dm.pid) {
						partialMatch.pop();
					}
					stack.back() += 1;
				}
				if (verbosity > 0) {
					std::cout << partialMatch << std::endl;
				}
				continue;
			}

			bool success = true;
			if (pid != dm.pid) {
				if (verbosity > 0) {
					std::cout << "pushing " << pid << ", " << mid << compMatches[pid][mid] << std::endl;
				}
				success = partialMatch.push(compMatches[pid][mid]);
			}
			if (success) {
				if (stack.size() == compMatches.size()){
					if (verbosity > 0) {
						std::cout << "found full" << std::endl;
						std::cout << partialMatch << std::endl;
					}
					auto res = partialMatch.apply();
					assert(res);
					edges.push_back(dgBuilder.applyDerivationRule(
					                    partialMatch.lhs(graphs), *res, rPatterns).first);
					if (pid != dm.pid) {
						partialMatch.pop();
					}
					stack.back() += 1;
				} else {
					if (verbosity > 0) {
						//				std::cout << "moving" << std::endl;
						std::cout << "not full yet!" << std::endl;
					}
					stack.push_back(0);
				}
			} else {
				if (verbosity > 0) {
					std::cout << "bad push, popping!" << std::endl;
				}
				partialMatch.pop();
				stack.back() += 1;
			}

		}
		partialMatch.pop();
		assert(partialMatch.empty());
	}
	return edges;
}

struct GraphData {
	using SideVertex = boost::graph_traits<lib::Rules::DPOProjection>::vertex_descriptor;
public:
	GraphData() : gPtr(new lib::Graph::GraphType()), pStringPtr(new lib::Graph::PropString(*gPtr)) {}

public:
	std::unique_ptr<lib::Graph::GraphType> gPtr;
	std::unique_ptr<lib::Graph::PropString> pStringPtr;
	std::unique_ptr<lib::Graph::PropStereo> pStereoPtr;
	std::vector<SideVertex> vertexMap;
};

DynamicDG::CachedRule::CachedRule(const Rules::Real *rule, DynamicDG& ddg):
    rule(rule), ddg(ddg), hasMatch(rule->getDPORule().numLeftComponents) {
	using Vertex = lib::Graph::Vertex;
	using Edge = lib::Graph::Edge;
	using SideVertex = boost::graph_traits<lib::Rules::DPOProjection>::vertex_descriptor;
	using SideEdge = boost::graph_traits<lib::Rules::DPOProjection>::edge_descriptor;

	const auto& rDPO = rule->getDPORule();

	std::vector<GraphData> products(rDPO.numLeftComponents);
	const auto &compMap = rDPO.leftComponents;
	const auto &gRight = get_left(rDPO);
	auto rpString = get_string(get_labelled_left(rDPO));
	assert(num_vertices(gRight) == num_vertices(get_graph(rDPO)));
	std::vector<Vertex> vertexMap(num_vertices(gRight));
	for(const auto vSide : asRange(vertices(gRight))) {
		const auto comp = compMap[get(boost::vertex_index_t(), gRight, vSide)];
		auto &p = products[comp];
		const auto v = add_vertex(*p.gPtr);
		vertexMap[get(boost::vertex_index_t(), gRight, vSide)] = v;
		p.pStringPtr->addVertex(v, rpString[vSide]);
	}
	for(const auto eSide : asRange(edges(gRight))) {
		const auto vSideSrc = source(eSide, gRight);
		const auto vSideTar = target(eSide, gRight);
		const auto comp = compMap[get(boost::vertex_index_t(), gRight, vSideSrc)];
		assert(comp == compMap[get(boost::vertex_index_t(), gRight, vSideTar)]);
		const auto vCompSrc = vertexMap[get(boost::vertex_index_t(), gRight, vSideSrc)];
		const auto vCompTar = vertexMap[get(boost::vertex_index_t(), gRight, vSideTar)];
		const auto epComp = add_edge(vCompSrc, vCompTar, *products[comp].gPtr);
		assert(epComp.second);
		products[comp].pStringPtr->addEdge(epComp.first, rpString[eSide]);
	}

	if(ddg.labelSettings.withStereo && has_stereo(rDPO)) {
		// make the inverse vertex maps
		for(auto &p : products)
			p.vertexMap.resize(num_vertices(*p.gPtr));
		for(const auto vSide : asRange(vertices(gRight))) {
			const auto comp = compMap[get(boost::vertex_index_t(), gRight, vSide)];
			auto &p = products[comp];
			const auto v = vertexMap[get(boost::vertex_index_t(), gRight, vSide)];
			p.vertexMap[get(boost::vertex_index_t(), *p.gPtr, v)] = vSide;
		}

		for(auto &p : products) {
			const auto &lgRight = get_labelled_left(rDPO);
			assert(has_stereo(lgRight));
			const auto vertexMap = [&p](const auto &vProduct) {
				return p.vertexMap[get(boost::vertex_index_t(), *p.gPtr, vProduct)];
			};
			const auto edgeMap = [&p, &lgRight](const auto &eProduct) {
				const auto &g = *p.gPtr;
				const auto &gSide = get_graph(lgRight);
				const auto vSrc = source(eProduct, g);
				const auto vTar = target(eProduct, g);
				const auto vSrcSide = p.vertexMap[get(boost::vertex_index_t(), g, vSrc)];
				const auto vTarSide = p.vertexMap[get(boost::vertex_index_t(), g, vTar)];
				const auto epSide = edge(vSrcSide, vTarSide, gSide);
				assert(epSide.second);
				return epSide.first;
			};
			const auto inf = Stereo::makeCloner(lgRight, *p.gPtr, vertexMap, edgeMap);
			p.pStereoPtr = std::make_unique<lib::Graph::PropStereo>(*p.gPtr, inf);
		} // end foreach product
	} // end of stereo prop
	// wrap them
	for(auto &g : products) {
		// check against the database
		lhsGraphs.push_back(std::make_unique<lib::Graph::Single>(std::move(g.gPtr), std::move(g.pStringPtr),
		                                                  std::move(g.pStereoPtr)));
	}

}

bool DynamicDG::CachedRule::isValid(const std::vector<const Graph::Single *>& graphs) {
	size_t nComponents = rule->getDPORule().numLeftComponents;
	namespace GM = jla_boost::GraphMorphism;
	for (size_t pid = 0; pid < nComponents; pid++) {
		bool isValid = false;
		for (const Graph::Single *g : graphs) {
			if (hasMatch[pid].find(g) == hasMatch[pid].end()) {
				hasMatch[pid][g] = (Graph::Single::monomorphism(*lhsGraphs[pid], *g, 1, ddg.labelSettings) > 0);
			}
			if (hasMatch[pid][g]) {
				isValid = true;
				break;
			}
		}
		if (!isValid) {
			return false;
		}
	}
	return true;
}

std::ostream& operator << (std::ostream& os, const std::vector<size_t>& vs) {
	for (size_t v : vs) {
		os << v << ", ";
	}
	return os;
}

const Rules::AutGroup& DynamicDG::CachedRule::getAutsGroup() {
	if (!rAuts) {
		rAuts = Rules::getRuleAutomorphisms(*rule);
	}
	return *rAuts;
}

bool DynamicDG::CachedRule::isCanonMatch(size_t pid, const ComponentMatch::VertexMap& match,
                    const Graph::Single* g) {
	const auto& gens = this->getAutsGroup();
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
	const auto& dpoRule = rule->getDPORule();
	const auto& dpoHost = g->getBindRule()->getRule().getDPORule();
	const auto& lgRule = get_labelled_left(dpoRule);;
	const auto& lgHost = get_labelled_right(dpoHost);
	const auto& gRule = get_graph(lgRule);
	const auto& gHost = get_graph(lgHost);
//	std::cout << "fix order: ";
	for (auto vp : asRange(vertices(get_component_graph(pid, lgRule)))) {
//		std::cout << vp << ", ";
		fv.fixes.push_back(vp);
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
	for (auto vp : asRange(vertices(get_component_graph(pid, lgRule)))) {
//		std::cout << "-------" << std::endl;
		auto vh = get(match, gRule, gHost, vp);
		bool isCanon = true;
		if (chain == nullptr) {
			const auto genPtrs = gens.generator_ptrs();
			perm_group::orbit(vp, genPtrs.begin(), genPtrs.end(),
			                  [&] (int w, int img,
			                        decltype(genPtrs.begin())) {
				auto vhImg = get(match, gRule, gHost, img);
//				std::cout << "orbit: " << w << ", " << img << ", vh: " << vhImg << std::endl;
				if (vh > vhImg) {
					isCanon = false;
				}
			});
			chain = stab.getChain();
		} else {
			const auto genPtrs = chain->generator_ptrs();
			perm_group::orbit(vp, genPtrs.begin(), genPtrs.end(),
			                  [&] (int w, int img,
			                        decltype(genPtrs.begin())) {
				auto vhImg = get(match, gRule, gHost, img);
//				std::cout << "orbit: " << w << ", " << img << ", vh: " << vhImg << std::endl;
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

void DynamicDG::CachedRule::storeMatches(const Graph::Single *graph) {
	const auto& dpoRule = rule->getDPORule();
	const auto& dpoHost = graph->getBindRule()->getRule().getDPORule();
	const auto& lgRule = get_labelled_left(dpoRule);;
	const auto& lgHost = get_labelled_right(dpoHost);
	const auto& gRule = get_graph(lgRule);
	const auto& gHost = get_graph(lgHost);

	auto getReactionCenter = [&] (size_t pid, const ComponentMatch::VertexMap& map) {
		using jla_boost::GraphDPO::Membership;
		const auto& gp = get_component_graph(pid, lgRule);
		std::vector<size_t> reactionCenter;
		for (auto v : asRange(vertices(gp))) {
			auto vHost = get(map, gRule, gHost, v);
			bool isRC = false;
			if (membership(dpoRule, v) != Membership::Context) {
				isRC = true;
			} else {
				for (auto e : asRange(out_edges(v, get_graph(dpoRule)))){
					if (membership(dpoRule, e) != Membership::Context) {
						isRC = true;
						break;
					}
				}
			}
			if (!isRC) {
				if (dpoRule.pString->getLeft()[v] != dpoRule.pString->getRight()[v])  {
					isRC = true;
				} else {
					for (auto e : asRange(out_edges(v, get_graph(dpoRule)))){
						if (dpoRule.pString->getLeft()[e] != dpoRule.pString->getRight()[e])  {
							        isRC = true;
									break;
						}
					}

				}
			}
			if (isRC) {
				reactionCenter.push_back(vHost);
			}

		}
		return reactionCenter;
	};

	if (ddg.verbosity > 0) {
		std::cout << "SAVING MATCHES FOR " << graph->getName() << std::endl;
	}
	auto mm = RC::makeRuleRuleComponentMonomorphism(lgRule, lgHost, true,
	                                                ddg.labelSettings, 0,
	                                                ddg.logger);

	struct hash {
		size_t operator ()(const std::vector<size_t>& vs) const {
			return boost::hash_value(vs);
		}
	};

	for (size_t pid = 0; pid < dpoRule.numLeftComponents; pid++) {
		//const Graph::Single::AutGroup& autGroup = this->lhsGraphs[pid]->getAutGroup(ddg.labelSettings.type, ddg.labelSettings.withStereo);

		//std::unordered_set<std::vector<size_t>, hash> usedReactionCenters;
		std::vector<ComponentMatch::VertexMap> compMatches;
		auto morphisms = mm(pid, 0);

		for (auto& m : morphisms) {
			if (!isCanonMatch(pid, m, graph)) {
//				std::cout << "IS NOT CANON" << std::endl;
				continue;
			}

//			auto rc = getReactionCenter(pid, m);
//			    std::cout << "Match: " << ComponentMatch(pid, 0, gRule, gHost, m, 0) << std::endl;
//				std::cout << "RC: " << rc << std::endl;
			// ddg.logger.indent() << "Storing new reaction center.\n";
			// usedReactionCenters.insert(std::move(rc));
			if (isValidComponentMap(dpoHost, dpoRule, pid, m, ddg.logger)) {
				compMatches.push_back(std::move(m));
			}
		}

		graphMatches[graph].push_back(std::move(compMatches));
	}
}

std::vector<std::vector<ComponentMatch>>
DynamicDG::CachedRule::getMatches(const std::vector<const Graph::Single *>& graphs) {
	size_t nComponents = rule->getDPORule().numLeftComponents;
	std::vector<std::vector<ComponentMatch>> matches(nComponents);

	Rules::getRuleAutomorphisms(*rule);

	size_t offset = 0;
	size_t hid = 0;
	for (const Graph::Single *g : graphs) {
		const auto& dpoRule = rule->getDPORule();
		const auto& dpoHost = g->getBindRule()->getRule().getDPORule();
		const auto& lgRule = get_labelled_left(dpoRule);;
		const auto& lgHost = get_labelled_right(dpoHost);

		if (graphMatches.find(g) == graphMatches.end()) {
			storeMatches(g);
		} else {
			if (ddg.verbosity > 0) {
				std::cout << "MATCHES ALREADY COMPUTED\n";
			}
		}

		const std::vector<std::vector<ComponentMatch::VertexMap>>& compMatches = graphMatches[g];

		for (size_t pid = 0; pid < nComponents; pid++) {
			for (const auto& vm : compMatches[pid]) {
				matches[pid].emplace_back(pid, hid, get_graph(lgRule), get_graph(lgHost),
				                          vm, offset);
				if (ddg.verbosity > 0) {
					std::cout << matches[pid].back() << std::endl;
				}
			}
		}

		hid += 1;
		offset += num_vertices(g->getGraph());
	}

	return matches;
}
std::vector<DG::NonHyper::Edge>
DynamicDG::findReusableEdges(const std::vector<DG::NonHyper::Edge>& foundEdges,
                             const std::vector<const Graph::Single *>& graphs,
                             const std::vector<bool>& isNewHost) {
	std::vector<DG::NonHyper::Edge> edges;
	std::unordered_map<const Graph::Single *, int> graphMap;
	for (const Graph::Single *g : graphs) {
		graphMap[g] += 1;
	}

	for (const auto& e : foundEdges)  {
		bool isValid = true;
		const auto lhs = dgBuilder.lhs(e);
		for (const Graph::Single *g : lhs)  {
			graphMap[g] -= 1;
			if (graphMap[g] < 0) {
				isValid = false;
			}
		}
		for (const Graph::Single *g : lhs)  {
			graphMap[g] += 1;
		}
		if (isValid) {
			edges.push_back(e);
		}
		/*
		bool isValid = true;
		for (const Graph::Single *g : lhs) {
			auto it = std::find(graphs.begin(), graphs.end(), g);
			if (it == graphs.end()) {
				isValid = false;
				break;
			}
			auto idx = std::distance(graphs.begin(), it);
			if (isNewHost[idx]) {
				isValid = false;
				break;
			}
		}
		if (isValid) {
			edges.push_back(e);
		}
		*/
	}
	return edges;

}

std::vector<DG::NonHyper::Edge> DynamicDG::apply(const std::vector<const Graph::Single *>& hosts) {
	LabelledUnionGraph<lib::Graph::LabelledGraph> ugHosts;
	for (const Graph::Single* g : hosts) {
		ugHosts.push_back(&g->getLabelledGraph());
	}
	const std::unique_ptr<Rules::Real> rHosts = lib::Rules::graphToRule(ugHosts, lib::Rules::Membership::Right, "G");

	CachedState& state = cacheAndGetState(hosts);

	std::set<DG::NonHyper::Edge> res;


	std::vector<bool> isNewHost(hosts.size(), true);
	std::vector<DG::NonHyper::Edge> cachedEdges;
	if (state.parent.size() > 0) {
		std::vector<const Graph::Single *> newGraphs = dgBuilder.rhs(state.usedEdge);
		for (size_t hid = 0; hid < hosts.size(); hid++) {
			if (std::find(newGraphs.begin(), newGraphs.end(), hosts[hid]) == newGraphs.end()){
				isNewHost[hid] = false;
			}
		}
//		newGraphs = dgBuilder.lhs(state.usedEdge);
//		for (size_t hid = 0; hid < hosts.size(); hid++) {
//			if (std::find(newGraphs.begin(), newGraphs.end(), hosts[hid]) == newGraphs.end()){
//				isNewHost[hid] = false;
//			}
//		}
		cachedEdges = cachedStates[state.parent].edges;
	}

	const auto reusedEdges = findReusableEdges(cachedEdges, hosts, isNewHost);
	res.insert(reusedEdges.begin(), reusedEdges.end());
	if (verbosity > 0) {
		logger.indent() << " >> Reusable Edges: " << reusedEdges.size() << std::endl;
	}

	int tmp_i = 0;
	int total_edges = 0;

	for (CachedRule& cr : cachedRules) {
		tmp_i += 1;
		if (!cr.isValid(hosts)) { continue; }
		auto compMatches = cr.getMatches(hosts);
		const auto newEdges = findNewEdges(hosts, *rHosts, *cr.rule,
		                                    compMatches, isNewHost);
		res.insert(newEdges.begin(), newEdges.end());
		std::set<DG::NonHyper::Edge> res2(newEdges.begin(), newEdges.end());
//		std::cout << "Actual new Edges(" << tmp_i-1 << ") " << res2.size() << "/" <<  newEdges.size() << std::endl;
		total_edges += newEdges.size();
		if (verbosity > 0) {
			logger.indent() << " >> New Edges: " << newEdges.size() << std::endl;
		}
	}
//	std::cout << "found: " << res.size() << "/" << total_edges << std::endl;

	DG::GraphMultiset parent(hosts);
	if (verbosity > 0) {
		std::cout << "UPDATING CACHE: " << std::endl;
	}
	for (auto e : res) {
		const auto lhs = DG::GraphMultiset(dgBuilder.lhs(e));
		const auto rhs = dgBuilder.rhs(e);

		size_t cur_lhs = 0;
		std::vector<const Graph::Single *> newState;
		for (const Graph::Single* g : parent) {
			if (cur_lhs < lhs.size() && g == *(lhs.begin()+cur_lhs)) {
				cur_lhs += 1;
				continue;
			}
			newState.push_back(g);
		}
		newState.insert(newState.end(), rhs.begin(), rhs.end());

		DG::GraphMultiset newStateKey(std::move(newState));
		if (cachedStates.find(newStateKey) == cachedStates.end()) {
			if (verbosity > 0) {
				std::cout << "SAVING ";
				printKey(newStateKey);
				std::cout << std::endl;

			}

			cachedStates[newStateKey] = CachedState(parent, e);
		} else {
			if (verbosity >  0) {
				std::cout << "KEY ALREADY EXISTS ";
				printKey(newStateKey);
				std::cout << std::endl;
			}

		}
	}
	state.edges = std::vector<DG::NonHyper::Edge>(res.begin(), res.end());

//	for (const Rules::Real *rPatterns : rules) {
//		std::cout << "COMPUTING " << rPatterns->getName() << std::endl;
//		const auto maps = computeMorphisms(*rHosts, *rPatterns, labelSettings, logger);
//		const auto rDerivations = composeRules(hosts, *rHosts, *rPatterns, maps, logger);
//		std::cout << "Found Ders: " << rDerivations.size() << std::endl;
//		for (const auto& dr : rDerivations) {
//			auto edge = dgBuilder.applyDerivationRule(dr.leftGraphs, *dr.rDerivation, *rPatterns).first;
//			res.push_back(edge);
//		}
//	}
	return state.edges;
}


void printDerivation(const std::vector<const Graph::Single *>& lhs,
                     const std::vector<const Graph::Single *>& rhs) {

	    std::cout << "[ ";
		for (const Graph::Single *g : lhs)  {
			std::cout << g->getGraphDFS().first << ", ";
		}
		std::cout << " ] -> [ ";

		for (const Graph::Single *g : rhs)  {
			std::cout << g->getGraphDFS().first << ", ";
		}
		std::cout << " ]" << std::endl;
}

std::vector<std::shared_ptr<RuleApplication>>
DynamicDG::updateRuleAndCache( const Rules::Real& rHosts,
                                      const Rules::Real& rPatterns,
                                      const std::vector<std::vector<ComponentMap>>& maps,
                                      const RuleApplicationMap& usedAppMap,
                                      const std::vector<std::shared_ptr<RuleApplication>>& foundApplications,
                                      IO::Logger& logger
                                      ) {

	std::vector<const Graph::Single *> hosts = dgBuilder.getGraphs(rHosts);
	printKey(DG::GraphMultiset(hosts));
	std::cout << std::endl;
	std::vector<std::shared_ptr<RuleApplication>> out;
	std::cout << "UPDATING RULE WITH MAP " << usedAppMap << std::endl;

	const auto &lgHosts = get_labelled_right(rHosts.getDPORule());
	const auto &gHosts = get_graph(get_labelled_right(rHosts.getDPORule()));
	std::vector<size_t> reactionCenterVertices = usedAppMap.getProjectedReactionCenter();
	std::cout << "FOUND REACTION CENTER: ";
	for (auto v : reactionCenterVertices) {
		std::cout << v << ", ";
	}
	std::cout << std::endl;
	std::sort(reactionCenterVertices.begin(), reactionCenterVertices.end());
	std::vector<bool> usedHost(hosts.size(), false);
	const auto &usedMap = usedAppMap.getMap();
	const auto &gUsedRule = get_graph(get_labelled_left(usedAppMap.getRule().getDPORule()));
	for (size_t v : asRange(vertices(gUsedRule))) {
		    auto vHost = get(usedMap, gUsedRule, gHosts, v);
			auto compHost = rHosts.getDPORule().rightComponents[vHost];
			usedHost[compHost] = true;
	}

	std::cout << "USED HOSTS ";
	for (auto v : usedHost) {
		std::cout << ((v) ? "TRUE" : "FALSE") << ", ";
	}
	std::cout << std::endl;

	for (const std::shared_ptr<RuleApplication>& ra : foundApplications) {
		const auto& raMap = ra->map.getMap();
		std::unordered_set<size_t> hostIds;
		const auto &gPatterns = get_graph(get_labelled_left(rPatterns.getDPORule()));
		bool shouldUpdate = false;
		bool shouldDelete = false;
		std::cout << "DETERMINING IF WE SHOULD UPDATE/DELETE" << std::endl;
		for (size_t v : asRange(vertices(gPatterns)))  {
			std::cout << "UPDATING MAP " << rHosts.getName() << " " << rPatterns.getName() << " " << ra->map << std::endl;
			auto vHost = get(raMap, gPatterns, gHosts, v);
			auto lower = std::lower_bound(reactionCenterVertices.begin(), reactionCenterVertices.end(), vHost);
			std::cout << "COMPARING: " << vHost << " " << *lower << std::endl;
			if (lower != reactionCenterVertices.end() && *lower == vHost) {
				shouldDelete = true;
				break;
			}
			auto compHost = rHosts.getDPORule().rightComponents[vHost];
			hostIds.insert(compHost);
			if (usedHost[compHost]) {
				shouldUpdate = true;
//				shouldDelete = true; //YOYO
//				break;
			}
		}

		if (shouldDelete) {
			std::cout << "DELETING APP" << std::endl;
			continue;
		}
		if (shouldUpdate) {
			std::vector<size_t> copyVertices;
			std::vector<const Graph::Single *> lhs;
			const auto &lgHosts = get_labelled_right(rHosts.getDPORule());
			for (size_t hid : hostIds)  {
				lhs.push_back(hosts[hid]);
				const auto &gh = get_component_graph(hid, lgHosts);
				for (const auto vh : asRange(vertices(gh))) {
					copyVertices.push_back(vh);
				}

				std::cout << "USING GRAPHS: " << hid << " " << lhs.back()->getName() << std::endl;
			}
			std::cout << "HOST VERTICES: ";
			for (auto v : copyVertices) {
				std::cout << v << ", ";
			}
			std::cout << std::endl;
			using InvertibleVertexMap = RuleApplicationMap::InvertibleVertexMap;
			using HasTerm = GraphMorphism::HasTermData<InvertibleVertexMap>;
			using HasStereo = GraphMorphism::HasStereoData<InvertibleVertexMap>;
			constexpr LabelType labelType = HasTerm::value ? LabelType::Term : LabelType::String;
			auto rDer = RC::composeRuleRealByMatch<labelType, HasStereo::value>(
			            rHosts, rPatterns, raMap, false, logger, &copyVertices);
			assert(rDer);
			auto derResult = dgBuilder.applyDerivationRule(lhs, *rDer, rPatterns);
			out.push_back(std::make_shared<RuleApplication>(ra->map, lhs,
			                                                derResult.second,
			                                                derResult.first));
			printDerivation(lhs, derResult.second);
			DG::GraphMultiset derKey(out.back()->transition(hosts));
			if (states.find(derKey) == states.end()) {
				std::cout << "SAVING ";
				printKey(derKey);
				std::cout << std::endl;
				using HasTerm = GraphMorphism::HasTermData<InvertibleVertexMap>;
				using HasStereo = GraphMorphism::HasStereoData<InvertibleVertexMap>;
				constexpr LabelType labelType = HasTerm::value ? LabelType::Term : LabelType::String;
				auto rState = RC::composeRuleRealByMatch<labelType, HasStereo::value>(
				            rHosts, rPatterns, raMap, false, logger);
				states[derKey] = State{DG::GraphMultiset(hosts),
				        std::move(rState), &rPatterns, &out.back()->map};
			} else {
				std::cout << "KEY EXISTS ";
				printKey(derKey);
				std::cout << std::endl;
			}

		} else {
			std::cout << " >> CAN REUSE APP" << std::endl;
			out.push_back(ra);
		}
	}

	std::cout << "Finding dirtyCompMaps" << std::endl;

	std::vector<const ComponentMap *> dirtyCompMaps;
	std::vector<size_t> mapsi;
	for (size_t pid = 0; pid < maps.size(); pid++) {
		size_t i = 0;
		for (const auto& compMap : maps[pid]) {
			i += 1;
			assert(compMap.m.size() > 0);
			if (!usedHost[compMap.hostId]) {
				continue;
			}
//			else { YOYO
//				    dirtyCompMaps.push_back(&compMap);
//					mapsi.push_back(i-1);
//					continue;

//			}

			const auto& gPatterns = get_graph(get_labelled_left(rPatterns.getDPORule()));
			const auto& gp = get_component_graph(pid, get_labelled_left(rPatterns.getDPORule()));
			for (auto vp : asRange(vertices(gp))) {
				auto vh = get(compMap.m, gPatterns, gHosts, vp);
				auto lower = std::lower_bound(reactionCenterVertices.begin(), reactionCenterVertices.end(), vh);
				if (lower != reactionCenterVertices.end() && *lower == vh) {
					dirtyCompMaps.push_back(&compMap);
					mapsi.push_back(i-1);
					break;
				}
			}
		}
	}

	std::cout << "FOUND " << dirtyCompMaps.size() << " DIRTY MAPS" << std::endl;

	RuleApplicationMap newMap(rHosts, rPatterns);
	size_t i = 0;
	for (const ComponentMap *dcm : dirtyCompMaps) {
		std::vector<size_t> stack, pidMap;
		for (size_t i = 0; i < maps.size(); i++) {
			pidMap.push_back(i);
		}
		stack.push_back(mapsi[i]);
		i += 1;
		std::swap(pidMap[0], pidMap[dcm->patternId]);

		std::cout << "	Pushing DCM FIRST (" << dcm->patternId << ", " << dcm->hostId << ")\n";

		while (stack.size() > 0) {
	//		std::cout << compositionMap << std::endl;
			size_t cid = pidMap[stack.size() - 1];
			size_t mid = stack.back();
			if (stack.size() == 1 && mid == mapsi[i] + 1) {
				break;
			}
			if (mid == maps[cid].size()) {
				stack.pop_back();
	//			std::cout << "Popping map: " << cid << ", " << mid << std::endl;
				if (stack.size() > 0) {
					newMap.popComponentMap();
					stack.back() += 1;
				}
				continue;
			}
			std::cout << "Adding map: " << cid << ", " << mid << "/" << maps[cid].size() <<  std::endl;
			assert(cid < maps.size() && mid < maps[cid].size());
			bool success = newMap.pushComponentMap(maps[cid][mid]);
			if (success)  {
	//			std::cout << "SUCCESS" << std::endl;
				if (stack.size() == maps.size()){
					std::cout << "found full" << std::endl;
					std::cout << newMap << std::endl;
					auto lhs = newMap.getLeftGraphs(hosts);
					auto rDer = newMap.computeDerivation(logger);
					assert(rDer);
					auto derResult = dgBuilder.applyDerivationRule(lhs, *rDer, rPatterns);
					out.push_back(std::make_shared<RuleApplication>(newMap, lhs,
					                                                derResult.second,
					                                                derResult.first));
					DG::GraphMultiset test(derResult.second);
					std::cout << "RHS ";
					printKey(test);
					std::cout << derResult.second[0]->getGraphDFS().first << std::endl;
					std::cout << std::endl;
					printDerivation(lhs, derResult.second);
					DG::GraphMultiset derKey(out.back()->transition(hosts));
					if (states.find(derKey) == states.end()) {
						std::cout << "SAVING ";
						printKey(derKey);
						std::cout << std::endl;

						states[derKey] = State{DG::GraphMultiset(hosts), newMap.getRuleState(logger),
						        &rPatterns, &out.back()->map};
					} else {
						std::cout << "KEY EXISTS ";
						printKey(derKey);
						std::cout << std::endl;
					}
					newMap.popComponentMap();
					stack.back() += 1;
				} else {
	//				std::cout << "moving" << std::endl;
					stack.push_back(0);
				}
			} else {
				newMap.popComponentMap();
				stack.back() += 1;
			}

		}
	}

	// Now for the updating of the new maps



	return out;


}

std::vector<std::shared_ptr<RuleApplication>>
DynamicDG::applyRuleAndCache(const std::vector<const Graph::Single *>& hosts,
                                      const Rules::Real& rHosts,
                                      const Rules::Real& rPatterns,
                                      const std::vector<std::vector<ComponentMap>>& maps,
                                      IO::Logger& logger
                                      ) {
	size_t nPatterns = rPatterns.getDPORule().numLeftComponents;
	size_t nHosts = rHosts.getDPORule().numRightComponents;
	std::vector<std::shared_ptr<RuleApplication>> out;
	std::cout << "NUM Patterns: " << nPatterns << " npattern vertices: " << num_vertices(get_graph(rPatterns.getDPORule())) << std::endl;

	for (size_t pid = 0; pid < nPatterns; pid++) {
		if (maps[pid].size() == 0) {
			return out;
		}
	}

	RuleApplicationMap raMap(rHosts, rPatterns);
	std::vector<size_t> stack;
	stack.push_back(0);

	while (stack.size() > 0) {
		std::cout << raMap << std::endl;
		size_t cid = stack.size() - 1;
		size_t mid = stack.back();
		if (mid == maps[cid].size()) {
			stack.pop_back();
//			std::cout << "Popping map: " << cid << ", " << mid << std::endl;
			if (stack.size() > 0) {
				raMap.popComponentMap();
				stack.back() += 1;
			}
			continue;
		}
		std::cout << "Adding map: " << cid << ", " << mid << std::endl;
		bool success = raMap.pushComponentMap(maps[cid][mid]);
		if (success)  {
			std::cout << "SUCCESS" << std::endl;
			if (stack.size() == maps.size()){
				std::cout << "found full" << std::endl;
				std::cout << raMap << std::endl;
				auto lhs = raMap.getLeftGraphs(hosts);
				auto rDer = raMap.computeDerivation(logger);
				assert(rDer);
				auto derResult = dgBuilder.applyDerivationRule(lhs, *rDer, rPatterns);
				out.push_back(std::make_shared<RuleApplication>(raMap, lhs,
				                                                derResult.second,
				                                                derResult.first));
				DG::GraphMultiset test(derResult.second);
				std::cout << "RHS ";
				printKey(test);
				std::cout << std::endl;
				DG::GraphMultiset derKey(out.back()->transition(hosts));
				if (states.find(derKey) == states.end()) {
					std::cout << "SAVING ";
					printKey(derKey);
					std::cout << std::endl;
					states[derKey] = State{DG::GraphMultiset(hosts),
					        raMap.getRuleState(logger), &rPatterns,
					        &out.back()->map};
				} else {
					std::cout << "KEY EXISTS ";
					printKey(derKey);
					std::cout << std::endl;
				}
				raMap.popComponentMap();
				stack.back() += 1;
			} else {
//				std::cout << "moving" << std::endl;
				stack.push_back(0);
			}
		} else {
			raMap.popComponentMap();
			stack.back() += 1;
		}

	}
	return out;
}

DynamicDG::CachedState& DynamicDG::cacheAndGetState(const std::vector<const Graph::Single *>& graphs) {
	DG::GraphMultiset stateKey(graphs);
	if (cachedStates.find(stateKey) != cachedStates.end()) {
		if (verbosity > 0) {
		std::cout << "FOUND STATE ";
		printKey(stateKey);
		std::cout<<std::endl;
		}
		return cachedStates[stateKey];
	}

	if (verbosity > 0) {

	std::cout << "INIT STATE ";
	printKey(stateKey);
	std::cout<<std::endl;
	}
	cachedStates[stateKey] = CachedState();
	return cachedStates[stateKey];
}

const DynamicDG::State& DynamicDG::getState(const std::vector<const Graph::Single *>& graphs) {
	DG::GraphMultiset stateKey(graphs);
	if (states.find(stateKey) != states.end()) {
		std::cout << "FOUND STATE " << std::endl;
		return states[stateKey];
	}

	LabelledUnionGraph<lib::Graph::LabelledGraph> ugHosts;
	for (const Graph::Single* g : graphs) {
		ugHosts.push_back(&g->getLabelledGraph());
	}
	std::cout << " >> CREATING NEW STATE ";
	printKey(stateKey);
	std::cout << std::endl;

	std::unique_ptr<Rules::Real> rState = lib::Rules::graphToRule(ugHosts, lib::Rules::Membership::Right, "G");
	states[stateKey] = State{DG::GraphMultiset(), std::move(rState), nullptr, nullptr};

	return states[stateKey];
}

std::vector<DG::NonHyper::Edge> DynamicDG::applyAndCache(const std::vector<const Graph::Single *>& hosts) {
	IO::Logger logger(std::cout);
	const State& state = getState(hosts);

	std::vector<DG::NonHyper::Edge> res;
	std::vector<std::vector<std::shared_ptr<RuleApplication>>> applications;

	DG::GraphMultiset key(hosts);

	if (state.parent.size() == 0) {
		for (const Rules::Real *rPatterns : rules) {
			std::cout << "COMPUTING " << rPatterns->getName() << std::endl;
			const auto maps = computeMorphisms(*state.state, *rPatterns, labelSettings, logger);
			std::cout << "APPLYING" << std::endl;
			const auto rDerivations = applyRuleAndCache(hosts, *state.state, *rPatterns, maps, logger);
			applications.push_back(rDerivations);
			std::cout << "Found Ders: " << rDerivations.size() << std::endl;
			for (const auto& dr : rDerivations) {
				res.push_back(dr->hyperEdge);
			}
		}

		cachedApplications[key] = std::move(applications);
	} else {
		std::cout << "COMPUTINIG CACHED!" << std::endl;
		std::cout << "PARENT: ";
		printKey(state.parent);
		std::cout << std::endl;
		assert(cachedApplications.find(state.parent) != cachedApplications.end());
		for (const Rules::Real *rPatterns : rules) {
			std::cout << "COMPUTING " << rPatterns->getName() << std::endl;
			const auto maps = computeMorphisms(*state.state, *rPatterns, labelSettings, logger);
			const auto rDerivations = updateRuleAndCache(*state.state, *rPatterns, maps,
			                                             *state.map, cachedApplications[state.parent][applications.size()],
			                                             logger);
			applications.push_back(rDerivations);
			std::cout << "Found Ders: " << rDerivations.size() << std::endl;
			for (const auto& dr : rDerivations) {
				res.push_back(dr->hyperEdge);
			}
		}
		cachedApplications[key] = std::move(applications);

	}
	return res;
}

}
