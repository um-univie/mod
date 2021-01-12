#include "Parallel.hpp"

#include <mod/Config.hpp>
#include <mod/lib/DG/Strategies/GraphState.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/IO/IO.hpp>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

unsigned int calcMaxNumComponents(const std::vector<Strategy *> &strats) {
	unsigned int res = 0;
	for(const Strategy *strat : strats) res = std::max(res, strat->getMaxComponents());
	return res;
}

Parallel::Parallel(const std::vector<Strategy *> &strats)
		: Strategy::Strategy(calcMaxNumComponents(strats)), strats(strats) {
	assert(!strats.empty());
}

Parallel::~Parallel() {
	for(Strategy *s : strats) delete s;
}

Strategy *Parallel::clone() const {
	std::vector<Strategy *> subStrats;
	for(const Strategy *s : strats) subStrats.push_back(s->clone());
	return new Parallel(subStrats);
}

void Parallel::preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>, IsomorphismPolicy)> add) const {
	for(const auto *s : strats) s->preAddGraphs(add);
}

void Parallel::forEachRule(std::function<void(const lib::Rules::Real &)> f) const {
	for(const auto *s : strats) s->forEachRule(f);
}

void Parallel::printInfo(PrintSettings settings) const {
	settings.indent() << "Parallel: " << strats.size() << " substrategies" << std::endl;
	++settings.indentLevel;
	for(int i = 0; i != strats.size(); i++) {
		settings.indent() << "Parallel: substrategy " << (i + 1) << ":" << std::endl;
		++settings.indentLevel;
		strats[i]->printInfo(settings);
		--settings.indentLevel;
	}
	printBaseInfo(settings);
}

bool Parallel::isConsumed(const Graph::Single *g) const {
	for(const Strategy *strat : strats)
		if(strat->isConsumed(g)) return true;
	return false;
}

void Parallel::setExecutionEnvImpl() {
	for(Strategy *strat : strats) strat->setExecutionEnv(getExecutionEnv());
}

void Parallel::executeImpl(PrintSettings settings, const GraphState &input) {
	if(settings.verbosity >= PrintSettings::V_Parallel) {
		settings.indent() << "Parallel: " << strats.size() << " substrategies" << std::endl;
		++settings.indentLevel;
	}
	for(unsigned int i = 0; i != strats.size(); i++) {
		Strategy *strat = strats[i];
		if(settings.verbosity >= PrintSettings::V_Parallel) {
			settings.indent() << "Parallel, substrategy " << (i + 1) << ":" << std::endl;
			++settings.indentLevel;
		}
		strat->execute(settings, input);
		if(settings.verbosity >= PrintSettings::V_Parallel)
			--settings.indentLevel;
	}
	std::vector<const GraphState *> outputs;
	for(const Strategy *strat : strats)
		outputs.push_back(&strat->getOutput());
	output = new GraphState(outputs);
	output->sortUniverse(Graph::Single::nameLess);
	output->sortSubset(Graph::Single::nameLess);
}

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod
