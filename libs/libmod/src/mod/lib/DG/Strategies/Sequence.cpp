#include "Sequence.hpp"

#include <mod/Config.hpp>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

Sequence::Sequence(const std::vector<Strategy *> &strats)
		: Strategy(calcMaxNumComponents(strats)), strats(strats) {
	assert(strats.size() > 0);
}

Sequence::~Sequence() {
	for(Strategy *s : strats) delete s;
}

Strategy *Sequence::clone() const {
	std::vector<Strategy *> subStrats;
	for(const Strategy *s : strats) subStrats.push_back(s->clone());
	return new Sequence(subStrats);
}

void Sequence::preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>, IsomorphismPolicy)> add) const {
	for(const auto *s : strats) s->preAddGraphs(add);
}

void Sequence::forEachRule(std::function<void(const lib::Rules::Real &)> f) const {
	for(const auto *s : strats) s->forEachRule(f);
}

void Sequence::printInfo(PrintSettings settings) const {
	settings.indent() << "Sequence: " << strats.size() << " substrategies\n";
	++settings.indentLevel;
	for(int i = 0; i != strats.size(); i++) {
		settings.indent() << "Substrategy " << (i + 1) << ":\n";
		++settings.indentLevel;
		strats[i]->printInfo(settings);
		--settings.indentLevel;
	}
	printBaseInfo(settings);
}

const GraphState &Sequence::getOutput() const {
	return strats.back()->getOutput();
}

bool Sequence::isConsumed(const Graph::Single *g) const {
	for(const Strategy *strat : strats)
		if(strat->isConsumed(g)) return true;
	return false;
}

void Sequence::setExecutionEnvImpl() {
	for(Strategy *strat : strats) strat->setExecutionEnv(getExecutionEnv());
}

void Sequence::executeImpl(PrintSettings settings, const GraphState &input) {
	if(settings.verbosity >= PrintSettings::V_Sequence) {
		settings.indent() << "Sequence: " << strats.size() << " substrategies" << std::endl;
		++settings.indentLevel;
	}
	for(int i = 0; i != strats.size(); ++i) {
		Strategy *strat = strats[i];
		if(settings.verbosity >= PrintSettings::V_Sequence) {
			settings.indent() << "Sequence, substrategy " << (i + 1) << ":" << std::endl;
			++settings.indentLevel;
		}
		if(i == 0)
			strat->execute(settings, input);
		else
			strat->execute(settings, strats[i - 1]->getOutput());
		if(settings.verbosity >= PrintSettings::V_Sequence)
			--settings.indentLevel;
	}
}

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod
