#include "Add.h"

#include <mod/Function.h>
#include <mod/Graph.h>
#include <mod/lib/DG/NonHyperRuleComp.h>
#include <mod/lib/DG/Strategies/GraphState.h>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

Add::Add(const std::vector<std::shared_ptr<mod::Graph> > graphs, bool onlyUniverse)
: Strategy::Strategy(0), graphs(graphs), generator(nullptr), onlyUniverse(onlyUniverse) { }

Add::Add(const std::shared_ptr<mod::Function<std::vector<std::shared_ptr<mod::Graph> >() > > generator, bool onlyUniverse)
: Strategy::Strategy(0), graphs(), generator(generator), onlyUniverse(onlyUniverse) { }

Add::~Add() { }

Strategy *Add::clone() const {
	if(!generator) return new Add(graphs, onlyUniverse);
	else return new Add(generator, onlyUniverse);
}

void Add::printInfo(std::ostream &s) const {
	s << indent << "Add";
	if(onlyUniverse) s << "Universe";
	s << ":";
	if(generator) {
		indentLevel++;
		s << std::endl << indent << "function = ";
		generator->print(s);
		indentLevel--;
	} else {
		for(const std::shared_ptr<mod::Graph> g : graphs) s << " " << g->getName();
	}
	s << std::endl;
	indentLevel++;
	printBaseInfo(s);
	indentLevel--;
}

bool Add::isConsumed(const lib::Graph::Single *g) const {
	return false;
}

void Add::executeImpl(std::ostream &s, const GraphState &input) {
	std::vector<std::shared_ptr<mod::Graph> > graphsToAdd = generator ? (*generator)() : graphs;
	for(const std::shared_ptr<mod::Graph> g : graphsToAdd) getExecutionEnv().addGraphAsVertex(g);
	output = new GraphState(input);
	if(onlyUniverse) for(const std::shared_ptr<mod::Graph> g : graphsToAdd) output->addToUniverse(&g->getGraph());
	else {
		for(const std::shared_ptr<mod::Graph> g : graphsToAdd) output->addToSubset(0, &g->getGraph());
	}
}

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod