#include "Parallel.h"

#include <mod/Config.h>
#include <mod/lib/DG/Strategies/GraphState.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/IO/IO.h>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

unsigned int calcMaxNumComponents(const std::vector<Strategy*>& strats) {
	unsigned int res = 0;
	for(const Strategy *strat : strats) res = std::max(res, strat->getMaxComponents());
	return res;
}

Parallel::Parallel(const std::vector<Strategy*>& strats) : Strategy::Strategy(calcMaxNumComponents(strats)), strats(strats) {
	if(strats.size() == 0) {
		if(getConfig().dg.disallowEmptyParallelStrategies.get()) {
			IO::log() << "ERROR: parallel strategy is empty." << std::endl;
			IO::log() << "Use the config variable " << getConfig().dg.disallowEmptyParallelStrategies.getName() << " to allow it." << std::endl;
			throw std::exception();
		} else {
			IO::log() << "WARNING: parallel strategy is empty." << std::endl;
		}
	}
}

Parallel::~Parallel() {
	for(Strategy *s : strats) delete s;
}

Strategy *Parallel::clone() const {
	std::vector<Strategy*> subStrats;
	for(const Strategy *s : strats) subStrats.push_back(s->clone());
	return new Parallel(subStrats);
}

void Parallel::preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>) > add) const {
	for(const auto *s : strats) s->preAddGraphs(add);
}

void Parallel::forEachRule(std::function<void(const lib::Rules::Real&)> f) const {
	for(const auto *s : strats) s->forEachRule(f);
}

void Parallel::printInfo(std::ostream& s) const {
	s << indent << "Parallel: " << strats.size() << " substrats" << std::endl;
	indentLevel++;
	for(unsigned int i = 0; i < strats.size(); i++) {
		s << indent << "Substrat " << i << ":" << std::endl;
		indentLevel++;
		strats[i]->printInfo(s);
		indentLevel--;
	}
	printBaseInfo(s);
	indentLevel--;
}

bool Parallel::isConsumed(const Graph::Single *g) const {
	for(const Strategy *strat : strats) {
		if(strat->isConsumed(g)) return true;
	}
	return false;
}

void Parallel::setExecutionEnvImpl() {
	for(Strategy *strat : strats) strat->setExecutionEnv(getExecutionEnv());
}

void Parallel::executeImpl(std::ostream& s, const GraphState &input) {
	if(getConfig().dg.calculateVerbose.get()) {
		s << indent << "parallel: " << strats.size() << " substrats" << std::endl;
		indentLevel++;
	}

	for(unsigned int i = 0; i < strats.size(); i++) {
		Strategy *strat = strats[i];
		if(getConfig().dg.calculateVerbose.get()) {
			s << indent << "substrat " << i << ":" << std::endl;
			indentLevel++;
		}
		strat->execute(s, input);
		if(getConfig().dg.calculateVerbose.get()) indentLevel--;
	}
	std::vector<const GraphState*> outputs;
	for(const Strategy *strat : strats) outputs.push_back(&strat->getOutput());
	output = new GraphState(outputs);
	output->sortUniverse(Graph::Single::nameLess);
	for(const GraphState::SubsetStore::value_type &vt : output->getSubsets()) {
		output->sortSubset(vt.first, Graph::Single::nameLess);
	}
	if(getConfig().dg.calculateVerbose.get()) indentLevel--;
}

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod
