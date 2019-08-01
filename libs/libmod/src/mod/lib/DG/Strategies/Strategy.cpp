#include "Strategy.hpp"

#include <mod/Config.hpp>
#include <mod/lib/DG/Strategies/GraphState.hpp>
#include <mod/lib/Graph/Single.hpp>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

Strategy::Strategy(unsigned int maxComponents)
: env(nullptr), maxComponents(maxComponents), input(nullptr), output(nullptr) { }

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

void Strategy::execute(std::ostream &s, const GraphState &input) {
	assert(env);
	this->input = &input;
	executeImpl(s, input);
}

const GraphState &Strategy::getOutput() const {
	assert(output);
	assert(output->hasSubset(0));
	return *output;
}

ExecutionEnv &Strategy::getExecutionEnv() {
	assert(env);
	return *env;
}

void Strategy::printBaseInfo(std::ostream &s) const {
	s << indent << "input:" << std::endl;
	indentLevel++;
	assert(input);
	for(const GraphState::SubsetStore::value_type &subset : input->getSubsets()) {
		s << indent << "subset " << subset.first << " =";
		for(const Graph::Single *g : subset.second) s << " " << g->getName();
		s << std::endl;
	}
	s << indent << "universe =";
	if(getConfig().dg.listUniverse.get()) for(const Graph::Single * g : input->getUniverse()) s << " " << g->getName();
	else s << " [listing disabled]";
	s << std::endl;
	indentLevel--;
	s << indent << "output:" << std::endl;
	indentLevel++;
	// important to use getOutput(), it might be overwritten

	for(const GraphState::SubsetStore::value_type &subset : getOutput().getSubsets()) {
		s << indent << "subset " << subset.first << " =";
		for(const Graph::Single *g : subset.second) s << " " << g->getName();
		s << std::endl;
	}
	s << indent << "universe =";
	if(getConfig().dg.listUniverse.get()) for(const Graph::Single * g : getOutput().getUniverse()) s << " " << g->getName();
	else s << " [listing disabled]";
	s << std::endl;
	indentLevel--;
}

void Strategy::setExecutionEnvImpl() { }

//------------------------------------------------------------------------------
// Static
//------------------------------------------------------------------------------

unsigned int Strategy::calcMaxNumComponents(const std::vector<Strategy*>& strats) {
	unsigned int res = 0;
	for(const Strategy *strat : strats) res = std::max(res, strat->getMaxComponents());
	return res;
}

unsigned int indentLevel = 0;

std::ostream &indent(std::ostream& s) {
	for(unsigned int i = 0; i < indentLevel; i++) s << "   ";
	return s;
}

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod