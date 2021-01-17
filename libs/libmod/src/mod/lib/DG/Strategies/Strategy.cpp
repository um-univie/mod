#include "Strategy.hpp"

#include <mod/Config.hpp>
#include <mod/lib/DG/Strategies/GraphState.hpp>
#include <mod/lib/Graph/Single.hpp>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

Strategy::Strategy(unsigned int maxComponents)
		: env(nullptr), maxComponents(maxComponents), input(nullptr), output(nullptr) {}

Strategy::~Strategy() {
	delete output;
}

void Strategy::setExecutionEnv(ExecutionEnv &env) {
	this->env = &env;
	setExecutionEnvImpl();
}

unsigned int Strategy::getMaxComponents() const {
	return maxComponents;
}

void Strategy::execute(PrintSettings settings, const GraphState &input) {
	assert(env);
	this->input = &input;
	executeImpl(settings, input);
}

const GraphState &Strategy::getOutput() const {
	assert(output);
	return *output;
}

ExecutionEnv &Strategy::getExecutionEnv() {
	assert(env);
	return *env;
}

void Strategy::printBaseInfo(PrintSettings settings) const {
	settings.indent() << "input:\n";
	++settings.indentLevel;
	std::ostream &s = settings.s;
	assert(input);
	settings.indent() << "subset =";
	for(const auto *g : input->getSubset())
		s << " " << g->getName();
	s << '\n';
	settings.indent() << "universe =";
	if(settings.withUniverse)
		for(const auto *g : input->getUniverse())
			s << " " << g->getName();
	else s << " [listing disabled]";
	s << '\n';
	--settings.indentLevel;
	settings.indent() << "output:\n";
	++settings.indentLevel;
	// important to use getOutput(), it might be overwritten
	settings.indent() << "subset =";
	for(const auto *g : getOutput().getSubset())
		s << " " << g->getName();
	s << '\n';
	settings.indent() << "universe =";
	if(settings.withUniverse)
		for(const auto *g : getOutput().getUniverse())
			s << " " << g->getName();
	else s << " [listing disabled]";
	s << '\n';
}

void Strategy::setExecutionEnvImpl() {}

//------------------------------------------------------------------------------
// Static
//------------------------------------------------------------------------------

unsigned int Strategy::calcMaxNumComponents(const std::vector<Strategy *> &strats) {
	unsigned int res = 0;
	for(const Strategy *strat : strats) res = std::max(res, strat->getMaxComponents());
	return res;
}

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod