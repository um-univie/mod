#include "Parallel.hpp"

#include <mod/Config.hpp>
#include <mod/lib/DG/Strategies/GraphState.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/IO/IO.hpp>

namespace mod::lib::DG::Strategies {

Parallel::Parallel(std::vector<std::unique_ptr<Strategy>> strats)
		: Strategy::Strategy(calcMaxNumComponents(strats)), strats(std::move(strats)) {
	assert(!this->strats.empty());
}

Parallel::~Parallel() = default;

std::unique_ptr<Strategy> Parallel::clone() const {
	std::vector<std::unique_ptr<Strategy>> subStrats;
	for(const auto &s : strats) subStrats.push_back(s->clone());
	return std::make_unique<Parallel>(std::move(subStrats));
}

void Parallel::preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>, IsomorphismPolicy)> add) const {
	for(const auto &s : strats) s->preAddGraphs(add);
}

void Parallel::forEachRule(std::function<void(const lib::Rules::Real &)> f) const {
	for(const auto &s : strats) s->forEachRule(f);
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
	for(const auto &s : strats)
		if(s->isConsumed(g)) return true;
	return false;
}

void Parallel::setExecutionEnvImpl() {
	for(auto &s : strats) s->setExecutionEnv(getExecutionEnv());
}

void Parallel::executeImpl(PrintSettings settings, const GraphState &input) {
	if(settings.verbosity >= PrintSettings::V_Parallel) {
		settings.indent() << "Parallel: " << strats.size() << " substrategies" << std::endl;
		++settings.indentLevel;
	}
	for(unsigned int i = 0; i != strats.size(); i++) {
		auto &s = strats[i];
		if(settings.verbosity >= PrintSettings::V_Parallel) {
			settings.indent() << "Parallel, substrategy " << (i + 1) << ":" << std::endl;
			++settings.indentLevel;
		}
		s->execute(settings, input);
		if(settings.verbosity >= PrintSettings::V_Parallel)
			--settings.indentLevel;
	}
	std::vector<const GraphState *> outputs;
	for(const auto &s : strats) outputs.push_back(&s->getOutput());
	output = new GraphState(outputs);
	output->sortUniverse(Graph::Single::nameLess);
	output->sortSubset(Graph::Single::nameLess);
}

} // namespace mob::lib::DG::Strategies