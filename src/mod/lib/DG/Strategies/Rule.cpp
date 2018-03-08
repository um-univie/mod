#include "Rule.h"

#include <mod/Config.h>
#include <mod/Derivation.h>
#include <mod/Misc.h>
#include <mod/rule/Rule.h>
#include <mod/lib/DG/NonHyperRuleComp.h>
#include <mod/lib/DG/Strategies/GraphState.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/Graph/Properties/Stereo.h>
#include <mod/lib/Graph/Properties/String.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/RC/ComposeRuleReal.h>
#include <mod/lib/RC/MatchMaker/Super.h>
#include <mod/lib/Rules/Real.h>
#include <mod/lib/Stereo/CloneUtil.h>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

Rule::Rule(std::shared_ptr<rule::Rule> r)
: Strategy(std::max(r->getRule().getDPORule().numLeftComponents, r->getRule().getDPORule().numRightComponents)),
r(r), rRaw(&r->getRule()) {
	assert(rRaw->getDPORule().numLeftComponents > 0);
}

Rule::Rule(const lib::Rules::Real* r)
: Strategy(std::max(r->getDPORule().numLeftComponents, r->getDPORule().numRightComponents)), rRaw(r) {
	assert(r->getDPORule().numLeftComponents > 0);
}

Strategy *Rule::clone() const {
	if(r) return new Rule(r);
	else return new Rule(rRaw);
}

void Rule::preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>) > add) const { }

void Rule::forEachRule(std::function<void(const lib::Rules::Real&)> f) const {
	f(*this->rRaw);
}

void Rule::printInfo(std::ostream &s) const {
	s << indent << "Rule: " << r->getName() << std::endl;
	indentLevel++;
	printBaseInfo(s);
	s << indent << "consumed =";
	std::vector<const lib::Graph::Single*> temp(begin(consumedGraphs), end(consumedGraphs));
	std::sort(begin(temp), end(temp), lib::Graph::Single::nameLess);
	for(const lib::Graph::Single *g : temp) s << " " << g->getName();
	s << std::endl;
	indentLevel--;
}

bool Rule::isConsumed(const lib::Graph::Single *g) const {
	return consumedGraphs.find(g) != consumedGraphs.end();
}

namespace {

struct BoundRule {
	const lib::Rules::Real *rule;
	std::vector<const lib::Graph::Single*> boundGraphs;
};

struct Context {
	const std::shared_ptr<rule::Rule> &r;
	ExecutionEnv &executionEnv;
	GraphState *output;
	std::unordered_set<const lib::Graph::Single*> &consumedGraphs;
};

struct BoundRuleStorage {

	BoundRuleStorage(LabelType labelType, bool withStereo, std::vector<BoundRule> &ruleStore, const BoundRule &rule, const lib::Graph::Single *graph)
	: labelType(labelType), withStereo(withStereo), ruleStore(ruleStore), rule(rule), graph(graph) { }

	void add(lib::Rules::Real *r) {
		BoundRule p{r, rule.boundGraphs};
		p.boundGraphs.push_back(graph);
		bool found = false;
		const bool doBoundRulesDuplicateCheck = true;
		// if it's only right side, we will rather split it instead
		if(doBoundRulesDuplicateCheck && !r->isOnlyRightSide()) {
			std::vector<const lib::Graph::Single*> &rThis = p.boundGraphs;

			std::sort(begin(rThis), end(rThis), [](const lib::Graph::Single *g1, const lib::Graph::Single * g2) {
				return g1->getId() < g2->getId();
			});

			for(BoundRule &rp : ruleStore) {
				if(rThis.size() != rp.boundGraphs.size()) continue;
				std::vector<const lib::Graph::Single*> &rOther = rp.boundGraphs;

				std::sort(rOther.begin(), rOther.end(), [](const lib::Graph::Single *g1, const lib::Graph::Single * g2) {
					return g1->getId() < g2->getId();
				});
				bool doContinue = false;
				for(unsigned int i = 0; i < rThis.size(); i++) {
					if(rThis[i] != rOther[i]) {
						doContinue = true;
						break;
					}
				}
				if(doContinue) continue;
				found = 1 == lib::Rules::Real::isomorphism(*r, *rp.rule, 1,{labelType, LabelRelation::Isomorphism, withStereo, LabelRelation::Isomorphism});
				if(found) break;
			}
		}
		if(found) {
			//			IO::log() << "Duplicate BRP found" << std::endl;
			delete r;
		} else {
			ruleStore.push_back(p);
			if(getConfig().dg.calculateDetailsVerbose.get()) {
				IO::log() << "DG::RuleComp\tadded <"
						<< r->getName() << ", {";
				for(const lib::Graph::Single *g : p.boundGraphs)
					IO::log() << " " << g->getName();
				IO::log() << " }> onlyRight: " << std::boolalpha << r->isOnlySide(lib::Rules::Membership::Right) << std::endl;
			}
		}
	}
private:
	const LabelType labelType;
	const bool withStereo;
	std::vector<BoundRule> &ruleStore;
	const BoundRule &rule;
	const lib::Graph::Single *graph;
};

void handleBoundRulePair(Context context, const BoundRule &brp) {
	assert(brp.rule);
	// use a smart pointer so the rule for sure is deallocated, even though we do a 'continue'
	const lib::Rules::Real &r = *brp.rule;
	const auto &rDPO = r.getDPORule();
	assert(r.isOnlyRightSide()); // otherwise, it should have been deallocated. All max component results should be only right side
	mod::Derivation d;
	d.r = context.r;
	for(const lib::Graph::Single *g : brp.boundGraphs) d.left.push_back(g->getAPIReference());
	{ // left predicate
		bool result = context.executionEnv.checkLeftPredicate(d);
		if(!result) {
			if(getConfig().dg.calculatePredicatesVerbose.get())
				IO::log() << indent << "Skipping " << r.getName() << " due to leftPredicate" << std::endl;
			return;
		}
	}
	if(getConfig().dg.calculateDetailsVerbose.get())
		IO::log() << "Splitting " << r.getName() << " into " << rDPO.numRightComponents << " graphs" << std::endl;
	const std::vector<const lib::Graph::Single*> &educts = brp.boundGraphs;
	if(rDPO.numRightComponents == 0) MOD_ABORT; // continue;
	using Vertex = lib::Graph::Vertex;
	using Edge = lib::Graph::Edge;
	using SideVertex = boost::graph_traits<lib::Rules::DPOProjection>::vertex_descriptor;
	using SideEdge = boost::graph_traits<lib::Rules::DPOProjection>::edge_descriptor;

	struct GraphData {

		GraphData() : gPtr(new lib::Graph::GraphType()), pStringPtr(new lib::Graph::PropString(*gPtr)) { }
	public:
		std::unique_ptr<lib::Graph::GraphType> gPtr;
		std::unique_ptr<lib::Graph::PropString> pStringPtr;
		std::unique_ptr<lib::Graph::PropStereo> pStereoPtr;
		std::vector<SideVertex> vertexMap;
	};
	std::vector<GraphData> products(rDPO.numRightComponents);
	const auto &compMap = rDPO.rightComponents;
	const auto &gRight = get_right(rDPO);
	auto rpString = get_string(get_labelled_right(rDPO));
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
	if(context.executionEnv.labelSettings.withStereo && has_stereo(rDPO)) {
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
			const auto &lgRight = get_labelled_right(rDPO);
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
		auto gCand = std::make_unique<lib::Graph::Single>(std::move(g.gPtr), std::move(g.pStringPtr), std::move(g.pStereoPtr));
		std::shared_ptr<graph::Graph> gWrapped = context.executionEnv.checkIfNew(std::move(gCand));
		// checkIfNew does not add the graph, so we must check against the previous products as well
		for(auto gPrev : d.right) {
			const auto ls = mod::LabelSettings(context.executionEnv.labelSettings.type, LabelRelation::Isomorphism, context.executionEnv.labelSettings.withStereo, LabelRelation::Isomorphism);
			const auto numIso = lib::Graph::Single::isomorphism(gPrev->getGraph(), gWrapped->getGraph(), 1, ls);
			if(numIso == 1) {
				if(getConfig().dg.calculateDetailsVerbose.get())
					IO::log() << "Discarding product " << gWrapped->getName() << ", isomorphic to other product " << gPrev->getName() << std::endl;
				gWrapped = gPrev;
				break;
			}
		}
		d.right.push_back(gWrapped);
	}
	if(getConfig().dg.onlyProduceMolecules.get()) {
		for(std::shared_ptr<graph::Graph> g : d.right) {
			if(!g->getIsMolecule()) {
				IO::log() << "Error: non-molecule produced; '" << g->getName() << "'" << std::endl
						<< "Derivation is:" << std::endl
						<< "\tEducts:" << std::endl;
				for(const lib::Graph::Single *g : educts) IO::log() << "\t\t'" << g->getName() << "'\t" << g->getGraphDFS().first << std::endl;
				IO::log() << "\tProducts:" << std::endl;
				for(std::shared_ptr<graph::Graph> g : d.right) IO::log() << "\t\t'" << g->getName() << "'\t" << g->getGraphDFS() << std::endl;
				IO::log() << "Rule is '" << context.r->getName() << "'" << std::endl;
				std::exit(1);
			}
		}
	}
	{ // right predicates
		bool result = context.executionEnv.checkRightPredicate(d);
		if(!result) {
			if(getConfig().dg.calculatePredicatesVerbose.get())
				IO::log() << indent << "Skipping " << r.getName() << " due to rightPredicate" << std::endl;
			return;
		}
	}
	{ // now the derivation is good, so add the products to output
		if(getConfig().dg.putAllProductsInSubset.get()) {
			for(std::shared_ptr<graph::Graph> g : d.right)
				context.output->addToSubset(0, &g->getGraph());
		} else {
			for(std::shared_ptr<graph::Graph> g : d.right) {
				if(!context.output->isInUniverse(&g->getGraph()))
					context.output->addToSubset(0, &g->getGraph());
			}
		}
		for(unsigned int i = 0; i < d.right.size(); i++) {
			auto g = d.right[i];
			context.executionEnv.addProduct(g);
		}
	}
	std::vector<const lib::Graph::Single*> rightGraphs;
	rightGraphs.reserve(d.right.size());
	for(const std::shared_ptr<graph::Graph> &g : d.right)
		rightGraphs.push_back(&g->getGraph());
	lib::DG::GraphMultiset gmsLeft(educts), gmsRight(std::move(rightGraphs));
	bool inserted = context.executionEnv.suggestDerivation(gmsLeft, gmsRight, &context.r->getRule());
	if(inserted) {
		for(const lib::Graph::Single *g : educts)
			context.consumedGraphs.insert(g);
	}
}

template<typename GraphRange>
unsigned int bindGraphs(Context context, const GraphRange &graphRange, const std::vector<BoundRule> &rules, std::vector<BoundRule>& outputRules) {
	unsigned int processedRules = 0;

	for(const lib::Graph::Single *g : graphRange) {
		if(context.executionEnv.doExit()) break;
		for(const BoundRule &p : rules) {
			if(context.executionEnv.doExit()) break;
			if(getConfig().dg.calculateDetailsVerbose.get()) IO::log() << "NonHyperRuleComp\ttrying " << p.rule->getName() << " . " << g->getName() << std::endl;
			std::vector<BoundRule> resultRules;
			BoundRuleStorage ruleStore(context.executionEnv.labelSettings.type, context.executionEnv.labelSettings.withStereo, resultRules, p, g);
			auto reporter = [&ruleStore] (std::unique_ptr<lib::Rules::Real> r) {
				ruleStore.add(r.release());
			};
			assert(p.rule);
			const lib::Rules::Real &rFirst = g->getBindRule()->getRule();
			const lib::Rules::Real &rSecond = *p.rule;
			lib::RC::Super mm(true, true);
			lib::RC::composeRuleRealByMatchMaker(rFirst, rSecond, mm, reporter, context.executionEnv.labelSettings);
			for(const BoundRule &brp : resultRules) {
				processedRules++;
				if(context.executionEnv.doExit()) delete brp.rule;
				else if(brp.rule->isOnlyRightSide()) {
					handleBoundRulePair(context, brp);
					delete brp.rule;
				} else outputRules.push_back(brp);
			}
		}
	}
	return processedRules;
}

} // namespace 

void Rule::executeImpl(std::ostream &s, const GraphState &input) {
	if(getExecutionEnv().labelSettings.withStereo) {
		// let's trigger deduction errors early
		try {
			get_stereo(rRaw->getDPORule());
		} catch(StereoDeductionError &e) {
			std::stringstream ss;
			ss << "\nStereo deduction error in rule '" << rRaw->getName() << "'.";
			e.append(ss.str());
			throw;
		}
	}
	if(getExecutionEnv().labelSettings.type == LabelType::Term) {
		const auto &term = get_term(rRaw->getDPORule());
		if(!isValid(term)) {
			std::string msg = "Parsing failed for rule '" + rRaw->getName() + "'. " + term.getParsingError();
			throw TermParsingError(std::move(msg));
		}
	}
	const bool Verbose = getConfig().dg.calculateVerbose.get();
	output = new GraphState(input.getUniverse());
	if(Verbose) s << indent << "Rule: " << r->getName() << std::endl;
	if(getExecutionEnv().doExit()) {
		if(Verbose) s << indent << "(skipping)" << std::endl;
		return;
	}
	std::vector<std::vector<BoundRule> > intermediaryRules(rRaw->getDPORule().numLeftComponents + 1);
	{
		BoundRule p;
		p.rule = rRaw;
		intermediaryRules[0].push_back(p);
	}
	Context context{r, getExecutionEnv(), output, consumedGraphs};
	const auto &subset = input.getSubset(0);
	const auto &universe = input.getUniverse();
	for(unsigned int i = 1; i <= rRaw->getDPORule().numLeftComponents; i++) {
		if(Verbose) {
			IO::log() << indent << "Binding component " << i << " with ";
			if(i == 1) {
				if(!getConfig().dg.ignoreSubset.get()) {
					IO::log() << subset.size() << " input graphs" << std::endl;
				} else {
					IO::log() << universe.size() << " input graphs" << std::endl;
				}
			} else {
				IO::log() << intermediaryRules[i - 1].size() << " intermediaries" << std::endl;
			}
		}

		std::size_t processedRules = 0;
		if(i == 1) {
			if(!getConfig().dg.ignoreSubset.get()) {
				processedRules = bindGraphs(context, subset, intermediaryRules[0], intermediaryRules[1]);
			} else {
				processedRules = bindGraphs(context, universe, intermediaryRules[0], intermediaryRules[1]);
			}
		} else {
			processedRules = bindGraphs(context, universe, intermediaryRules[i - 1], intermediaryRules[i]);
			for(BoundRule &p : intermediaryRules[i - 1]) {
				delete p.rule;
				p.rule = nullptr;
			}
		}
		if(Verbose)
			IO::log() << indent << "Processing of " << processedRules << " intermediary rules done" << std::endl;
		if(context.executionEnv.doExit()) break;
	}
	assert(intermediaryRules.back().empty());
}

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod
