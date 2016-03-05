#include "Execute.h"

#include <mod/Config.h>
#include <mod/Function.h>
#include <mod/lib/DG/Strategies/GraphState.h>
#include <mod/lib/Graph/Single.h>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

Execute::Execute(std::shared_ptr<mod::Function<void(const mod::DGStrat::GraphState&)> > func)
: Strategy::Strategy(0), func(func) { }

Strategy *Execute::clone() const {
	return new Execute(func->clone());
}

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
	mod::DGStrat::GraphState gs(
			[&input](std::vector<std::shared_ptr<mod::Graph> > &subset) {
				for(const lib::Graph::Single *g : input.getSubset(0)) subset.push_back(g->getAPIReference());
			},
	[&input](std::vector<std::shared_ptr<mod::Graph> > &universe) {
		for(const lib::Graph::Single *g : input.getUniverse()) universe.push_back(g->getAPIReference());
	},
	[this](std::vector<mod::DerivationRef> &derivationRefs) {
		getExecutionEnv().fillDerivationRefs(derivationRefs);
	});
	(*func)(gs);
}

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod