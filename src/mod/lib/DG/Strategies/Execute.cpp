#include "Execute.h"

#include <mod/Config.h>
#include <mod/Function.h>
#include <mod/lib/DG/Strategies/GraphState.h>
#include <mod/lib/Graph/Single.h>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

Execute::Execute(std::shared_ptr<mod::Function<void(const dg::Strategy::GraphState&)> > func)
: Strategy::Strategy(0), func(func) { }

Strategy *Execute::clone() const {
	return new Execute(func->clone());
}

void Execute::preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>) > add) const { }

void Execute::printInfo(std::ostream &s) const {
	s << indent << "Execute";
	s << ":" << std::endl;
	indentLevel++;
	s << indent << "function = ";
	func->print(s);
	s << std::endl;
	indentLevel--;
}

const GraphState &Execute::getOutput() const {
	return *input;
}

bool Execute::isConsumed(const lib::Graph::Single *g) const {
	return false;
}

void Execute::executeImpl(std::ostream &s, const GraphState &input) {
	if(getConfig().dg.calculateVerbose.get()) printInfo(s);
	dg::Strategy::GraphState gs(
			[&input](std::vector<std::shared_ptr<graph::Graph> > &subset) {
				for(const lib::Graph::Single *g : input.getSubset(0)) subset.push_back(g->getAPIReference());
			},
	[&input](std::vector<std::shared_ptr<graph::Graph> > &universe) {
		for(const lib::Graph::Single *g : input.getUniverse()) universe.push_back(g->getAPIReference());
	},
	[this](std::vector<dg::DG::HyperEdge> &edges) {
		getExecutionEnv().fillHyperEdges(edges);
	});
	(*func)(gs);
}

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod