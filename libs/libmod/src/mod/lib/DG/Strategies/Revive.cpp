#include "Revive.hpp"

#include <mod/Config.hpp>
#include <mod/lib/DG/Strategies/GraphState.hpp>
#include <mod/lib/Graph/Single.hpp>

#include <iostream>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

Revive::Revive(Strategy *strat) : Strategy(strat->getMaxComponents()), strat(strat) {}

Revive::~Revive() {
	delete strat;
}

Strategy *Revive::clone() const {
	return new Revive(strat->clone());
}

void Revive::preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>, IsomorphismPolicy)> add) const {
	strat->preAddGraphs(add);
}

void Revive::forEachRule(std::function<void(const lib::Rules::Real &)> f) const {
	strat->forEachRule(f);
}

void Revive::printInfo(PrintSettings settings) const {
	settings.indent() << "Revive:\n";
	++settings.indentLevel;
	strat->printInfo(settings);
	printBaseInfo(settings);
	settings.indent() << "revived =";
	for(const auto *g : revivedGraphs)
		settings.s << " " << g->getName();
	settings.s << '\n';
}

bool Revive::isConsumed(const Graph::Single *g) const {
	return strat->isConsumed(g);
}

void Revive::setExecutionEnvImpl() {
	strat->setExecutionEnv(getExecutionEnv());
}

void Revive::executeImpl(PrintSettings settings, const GraphState &input) {
	if(settings.verbosity >= PrintSettings::V_Revive) {
		settings.indent() << "Revive:" << std::endl;
		++settings.indentLevel;
	}
	strat->execute(settings, input);
	output = new GraphState(strat->getOutput());

	for(const auto *g : input.getSubset(0)) {
		if(!strat->isConsumed(g) && output->isInUniverse(g)) {
			output->addToSubset(0, g);
			revivedGraphs.push_back(g);
		}
	}
}

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod
