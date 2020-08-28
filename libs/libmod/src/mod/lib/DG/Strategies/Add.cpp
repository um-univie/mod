#include "Add.hpp"

#include <mod/Error.hpp>
#include <mod/Function.hpp>
#include <mod/graph/Graph.hpp>
#include <mod/lib/DG/Strategies/GraphState.hpp>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

Add::Add(const std::vector<std::shared_ptr<graph::Graph> > graphs, bool onlyUniverse, IsomorphismPolicy graphPolicy)
		: Strategy::Strategy(0),
		  graphs(graphs), onlyUniverse(onlyUniverse), graphPolicy(graphPolicy) {}

Add::Add(const std::shared_ptr<mod::Function<std::vector<std::shared_ptr<graph::Graph> >()> > generator,
         bool onlyUniverse, IsomorphismPolicy graphPolicy)
		: Strategy::Strategy(0),
		  generator(generator), onlyUniverse(onlyUniverse), graphPolicy(graphPolicy) {}

Add::~Add() = default;

Strategy *Add::clone() const {
	if(!generator)
		return new Add(graphs, onlyUniverse, graphPolicy);
	else
		return new Add(generator, onlyUniverse, graphPolicy);
}

void Add::preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>, IsomorphismPolicy)> add) const {
	for(const auto &g : graphs)
		add(g, graphPolicy);
}

void Add::printInfo(PrintSettings settings) const {
	settings.indent() << "Add";
	std::ostream &s = settings.s;
	if(onlyUniverse) s << "Universe";
	s << ":";
	if(generator) {
		s << '\n';
		++settings.indentLevel;
		settings.indent() << "function = ";
		generator->print(s);
		--settings.indentLevel;
	} else {
		for(const auto &g : graphs)
			s << " " << g->getName();
	}
	s << '\n';
	++settings.indentLevel;
	printBaseInfo(settings);
}

bool Add::isConsumed(const lib::Graph::Single *g) const {
	return false;
}

void Add::executeImpl(PrintSettings settings, const GraphState &input) {
	if(settings.verbosity >= PrintSettings::V_Add) {
		if(onlyUniverse)
			settings.indent() << "AddUniverse:" << std::endl;
		else
			settings.indent() << "AddSubset:" << std::endl;
	}
	std::vector<std::shared_ptr<graph::Graph> > graphsToAdd;
	if(generator) {
		graphsToAdd = (*generator)();
		if(std::any_of(graphsToAdd.begin(), graphsToAdd.end(), [](const auto &g) {
			return !g;
		}))
			throw LogicError("A graph returned in a dynamic add strategy is a null pointer.");
	} else {
		graphsToAdd = graphs;
	}
	switch(graphPolicy) {
	case IsomorphismPolicy::Check:
		for(const std::shared_ptr<graph::Graph> g : graphsToAdd) {
			// TODO: we need to add it as a vertex for backwards compatibility,
			//       do this more elegantly
			// TODO: actually, if this is a static add strategy, the graphs are probably already added,
			//       at least with the usual execute function.
			getExecutionEnv().tryAddGraph(g);
			getExecutionEnv().trustAddGraphAsVertex(g);
		}
		break;
	case IsomorphismPolicy::TrustMe:
		for(const std::shared_ptr<graph::Graph> g : graphsToAdd)
			getExecutionEnv().trustAddGraphAsVertex(g);
		break;
	}
	output = new GraphState(input);
	if(onlyUniverse) {
		for(const std::shared_ptr<graph::Graph> g : graphsToAdd)
			output->addToUniverse(&g->getGraph());
	} else {
		for(const std::shared_ptr<graph::Graph> g : graphsToAdd)
			output->addToSubset(0, &g->getGraph());
	}
}

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod