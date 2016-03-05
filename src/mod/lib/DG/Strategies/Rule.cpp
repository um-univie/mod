#include "Rule.h"

#include <mod/Config.h>
#include <mod/Derivation.h>
#include <mod/Misc.h>
#include <mod/Rule.h>
#include <mod/lib/DG/NonHyperRuleComp.h>
#include <mod/lib/DG/Strategies/GraphState.h>
#include <mod/lib/Graph/Merge.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/RC/Core.h>
#include <mod/lib/RC/MatchMaker/Super.h>
#include <mod/lib/Rule/Real.h>

#include <jla_boost/Memory.hpp>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

Rule::Rule(std::shared_ptr<mod::Rule> r)
: Strategy(std::max(r->getReal()->getNumLeftComponents(), r->getReal()->getNumRightComponents())), r(r), rRaw(r->getReal()) {
	assert(rRaw->getNumLeftComponents() > 0);
}

Rule::Rule(const lib::Rule::Real* r)
: Strategy(std::max(r->getNumLeftComponents(), r->getNumRightComponents())), rRaw(r) {
	assert(r->getNumLeftComponents() > 0);
}

Strategy *Rule::clone() const {
	if(r) return new Rule(r);
	else return new Rule(rRaw);
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
	const lib::Rule::Real *rule;
	std::vector<const lib::Graph::Single*> boundGraphs;
};

struct Context {
	const std::shared_ptr<mod::Rule> &r;
	ExecutionEnv &executionEnv;
	GraphState *output;
	std::unordered_set<const lib::Graph::Single*> &consumedGraphs;
};

struct BoundRuleStorage {

	BoundRuleStorage(std::vector<BoundRule> &ruleStore, const BoundRule &rule, const lib::Graph::Single *graph)
	: ruleStore(ruleStore), rule(rule), graph(graph) { }

	void add(lib::Rule::Real *r) {
		BoundRule p{r, rule.boundGraphs};
		p.boundGraphs.push_back(graph);
		bool found = false;
		bool doBoundRulesDuplicateCheck = true;
		if(doBoundRulesDuplicateCheck && !r->isOnlyRightSide()) {
			// if it's only right side, we will rather split it instead
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
				found = 1 == lib::Rule::Real::isomorphism(*r, *rp.rule, 1);
				if(found) break;
			}
		}
		if(found) delete r;
		else {
			ruleStore.push_back(p);
			if(getConfig().dg.calculateDetailsVerbose.get()) {
				IO::log() << "DG::RuleComp\tadded <"
						<< r->getName() << ", {";
				for(const lib::Graph::Single *g : p.boundGraphs)
					IO::log() << " " << g->getName();
				IO::log() << " }> onlyRight: " << std::boolalpha << r->isOnlySide(lib::Rule::Membership::Right) << std::endl;
			}
		}
	}
private:
	std::vector<BoundRule> &ruleStore;
	const BoundRule &rule;
	const lib::Graph::Single *graph;
};

void handleBoundRulePair(Context context, const BoundRule &brp) {
	assert(brp.rule);
	// use a smart pointer so the rule for sure is deallocated, even though we do a 'continue'
	const lib::Rule::Real *theRule = brp.rule;
	assert(theRule->isOnlyRightSide()); // otherwise, it should have been deallocated. All max component results should be only right side
	mod::Derivation d;
	d.rule = context.r;
	for(const lib::Graph::Single *g : brp.boundGraphs) d.left.push_back(g->getAPIReference());
	{ // left predicate
		bool result = context.executionEnv.checkLeftPredicate(d);
		if(!result) {
			if(getConfig().dg.calculatePredicatesVerbose.get())
				IO::log() << indent << "Skipping " << theRule->getName() << " due to leftPredicate" << std::endl;
			return;
		}
	}
	if(getConfig().dg.calculateDetailsVerbose.get()) IO::log() << "Splitting " << theRule->getName() << " into "
		<< theRule->getNumRightComponents() << " graphs" << std::endl;
	const std::vector<const lib::Graph::Single*> &educts = brp.boundGraphs;
	if(theRule->getNumRightComponents() == 0) MOD_ABORT; // continue;
	using Vertex = lib::Graph::Vertex;
	using Edge = lib::Graph::Edge;
	std::vector<std::pair<std::unique_ptr<lib::Graph::GraphType>, std::unique_ptr<lib::Graph::PropStringType> > > products(theRule->getNumRightComponents());
	for(unsigned int i = 0; i < products.size(); i++) {
		auto g = make_unique<lib::Graph::GraphType>();
		auto pString = make_unique<lib::Graph::PropStringType>(*g);
		products[i] = std::make_pair(std::move(g), std::move(pString));
	}
	const auto &compMap = theRule->getCompMapRight();
	const auto &gRight = theRule->getRight();
	auto labelRight = theRule->getStringState().getRight();
	typedef boost::graph_traits<lib::Rule::DPOProjection>::vertex_descriptor SideVertex;
	typedef boost::graph_traits<lib::Rule::DPOProjection>::edge_descriptor SideEdge;
	std::unordered_map<SideVertex, Vertex> vertexMap;
	for(SideVertex vSide : asRange(vertices(gRight))) {
		unsigned int comp = compMap[get(boost::vertex_index_t(), gRight, vSide)];
		Vertex v = add_vertex(*products[comp].first);
		vertexMap[vSide] = v;
		products[comp].second->addVertex(v, labelRight[vSide]);
	}
	for(SideEdge eSide : asRange(edges(gRight))) {
		SideVertex vSideSrc = source(eSide, gRight);
		SideVertex vSideTar = target(eSide, gRight);
		unsigned int comp = compMap[get(boost::vertex_index_t(), gRight, vSideSrc)];
		assert(comp == compMap[get(boost::vertex_index_t(), gRight, vSideTar)]);
		assert(vertexMap.find(vSideSrc) != end(vertexMap));
		assert(vertexMap.find(vSideTar) != end(vertexMap));
		Vertex vCompSrc = vertexMap[vSideSrc];
		Vertex vCompTar = vertexMap[vSideTar];
		std::pair<Edge, bool> eComp = add_edge(vCompSrc, vCompTar, *products[comp].first);
		assert(eComp.second);
		products[comp].second->addEdge(eComp.first, labelRight[eSide]);
	}
	// wrap them
	for(auto &g : products) {
		// check against the database
		std::shared_ptr<mod::Graph> gWrapped = context.executionEnv.checkIfNew(std::move(g.first), std::move(g.second));
		// checkIfNew does not add the graph, so we must check against the previous products as well
		for(auto gPrev : d.right) {
			if(1 == lib::Graph::Single::isomorphism(gPrev->getGraph(), gWrapped->getGraph(),
					1)) {
				gWrapped = gPrev;
				break;
			}
		}
		d.right.push_back(gWrapped);
	}
	if(getConfig().dg.onlyProduceMolecules.get()) {
		for(std::shared_ptr<mod::Graph> g : d.right) {
			if(!g->getIsMolecule()) {
				IO::log() << "Error: non-molecule produced; '" << g->getName() << "'" << std::endl
						<< "Derivation is:" << std::endl
						<< "\tEducts:" << std::endl;
				for(const lib::Graph::Single *g : educts) IO::log() << "\t\t'" << g->getName() << "'\t" << g->getGraphDFS().first << std::endl;
				IO::log() << "\tProducts:" << std::endl;
				for(std::shared_ptr<mod::Graph> g : d.right) IO::log() << "\t\t'" << g->getName() << "'\t" << g->getGraphDFS() << std::endl;
				IO::log() << "Rule is '" << context.r->getName() << "'" << std::endl;
				std::exit(1);
			}
		}
	}
	{ // right predicates
		bool result = context.executionEnv.checkRightPredicate(d);
		if(!result) {
			if(getConfig().dg.calculatePredicatesVerbose.get())
				IO::log() << indent << "Skipping " << theRule->getName() << " due to rightPredicate" << std::endl;
			return;
		}
	}
	{ // now the derivation is good, so add the products to output
		if(getConfig().dg.putAllProductsInSubset.get()) {
			for(std::shared_ptr<mod::Graph> g : d.right)
				context.output->addToSubset(0, &g->getGraph());
		} else {
			for(std::shared_ptr<mod::Graph> g : d.right) {
				if(!context.output->isInUniverse(&g->getGraph()))
					context.output->addToSubset(0, &g->getGraph());
			}
		}
		for(unsigned int i = 0; i < d.right.size(); i++) {
			auto g = d.right[i];
			context.executionEnv.addProduct(g);
		}
	}
	const lib::Graph::Base *educt = nullptr;
	if(educts.size() == 1) educt = educts[0];
	else {
		lib::Graph::Merge *eductSub = new lib::Graph::Merge();
		for(const lib::Graph::Single *g : educts) eductSub->mergeWith(*g);
		eductSub->lock();
		educt = context.executionEnv.addToMergeStore(eductSub);
	}
	const lib::Graph::Base *product = nullptr;
	if(d.right.size() == 1) product = &d.right[0]->getGraph();
	else {
		lib::Graph::Merge *productSub = new lib::Graph::Merge();
		for(std::shared_ptr<mod::Graph> g : d.right) productSub->mergeWith(g->getGraph());
		productSub->lock();
		product = context.executionEnv.addToMergeStore(productSub);
	}
	bool inserted = context.executionEnv.suggestDerivation(educt, product, context.r->getReal());
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
			BoundRuleStorage ruleStore(resultRules, p, g);
			auto reporter = [&ruleStore] (std::unique_ptr<lib::Rule::Real> r) {
				ruleStore.add(r.release());
			};
			assert(g->getBindRule()->getReal());
			assert(p.rule);
			const lib::Rule::Real &rFirst = *g->getBindRule()->getReal();
			const lib::Rule::Real &rSecond = *p.rule;
			lib::RC::Super mm(true, true);
			lib::RC::composeRules(rFirst, rSecond, mm, reporter);
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

void Rule::executeImpl(std::ostream& s, const GraphState &input) {
	const bool Verbose = getConfig().dg.calculateVerbose.get();
	output = new GraphState(input.getUniverse());
	if(Verbose) s << indent << "Rule: " << r->getName() << std::endl;
	if(getExecutionEnv().doExit()) {
		if(Verbose) s << indent << "(skipping)" << std::endl;
		return;
	}
	std::vector<std::vector<BoundRule> > intermediaryRules(rRaw->getNumLeftComponents() + 1);
	{
		BoundRule p;
		p.rule = rRaw;
		intermediaryRules[0].push_back(p);
	}
	Context context{r, getExecutionEnv(), output, consumedGraphs};
	const auto &subset = input.getSubset(0);
	const auto &universe = input.getUniverse();
	for(unsigned int i = 1; i <= rRaw->getNumLeftComponents(); i++) {
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
