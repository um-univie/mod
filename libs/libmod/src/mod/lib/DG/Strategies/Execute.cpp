#include "Execute.hpp"

#include <mod/Config.hpp>
#include <mod/Function.hpp>
#include <mod/lib/DG/Strategies/GraphState.hpp>
#include <mod/lib/Graph/Single.hpp>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

Execute::Execute(std::shared_ptr<mod::Function<void(const dg::Strategy::GraphState &)> > func)
		: Strategy::Strategy(0), func(func) {}

Strategy *Execute::clone() const {
	return new Execute(func->clone());
}

void Execute::preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>, IsomorphismPolicy)> add) const {}

void Execute::printInfo(PrintSettings settings) const {
	settings.indent() << "Execute:\n";
	++settings.indentLevel;
	settings.indent() << "function = ";
	func->print(settings.s);
	settings.s << '\n';
}

const GraphState &Execute::getOutput() const {
	return *input;
}

bool Execute::isConsumed(const lib::Graph::Single *g) const {
	return false;
}

void Execute::executeImpl(PrintSettings settings, const GraphState &input) {
	if(settings.verbosity >= PrintSettings::V_Execute)
		printInfo(settings);
	dg::Strategy::GraphState gs(
			[&input](std::vector<std::shared_ptr<graph::Graph> > &subset) {
				for(const lib::Graph::Single *g : input.getSubset())
					subset.push_back(g->getAPIReference());
			},
			[&input](std::vector<std::shared_ptr<graph::Graph> > &universe) {
				for(const lib::Graph::Single *g : input.getUniverse())
					universe.push_back(g->getAPIReference());
			});
	(*func)(gs);
}

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod