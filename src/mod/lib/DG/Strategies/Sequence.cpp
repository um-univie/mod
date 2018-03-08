#include "Sequence.h"

#include <mod/Config.h>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

Sequence::Sequence(const std::vector<Strategy*>& strats)
: Strategy(calcMaxNumComponents(strats)), strats(strats) {
	assert(strats.size() > 0);
}

Sequence::~Sequence() {
	for(Strategy *s : strats) delete s;
}

Strategy *Sequence::clone() const {
	std::vector<Strategy*> subStrats;
	for(const Strategy *s : strats) subStrats.push_back(s->clone());
	return new Sequence(subStrats);
}

void Sequence::preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>) > add) const {
	for(const auto *s : strats) s->preAddGraphs(add);
}

void Sequence::forEachRule(std::function<void(const lib::Rules::Real&)> f) const {
	for(const auto *s : strats) s->forEachRule(f);
}

void Sequence::printInfo(std::ostream& s) const {
	s << indent << "Sequence: " << strats.size() << " substrats" << std::endl;
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

const GraphState &Sequence::getOutput() const {
	return strats.back()->getOutput();
}

bool Sequence::isConsumed(const Graph::Single *g) const {
	for(const Strategy *strat : strats) {
		if(strat->isConsumed(g)) return true;
	}
	return false;
}

void Sequence::setExecutionEnvImpl() {
	for(Strategy *strat : strats) strat->setExecutionEnv(getExecutionEnv());
}

void Sequence::executeImpl(std::ostream& s, const GraphState &input) {
	if(getConfig().dg.calculateVerbose.get()) {
		s << indent << "Sequence: " << strats.size() << " substrats" << std::endl;
		indentLevel++;
	}

	// the first
	{
		Strategy *strat = strats.front();
		if(getConfig().dg.calculateVerbose.get()) {
			s << indent << "substrat 0:" << std::endl;
			indentLevel++;
		}
		strat->execute(s, input);
		if(getConfig().dg.calculateVerbose.get()) indentLevel--;
	}

	// the rest
	for(unsigned int i = 1; i < strats.size(); i++) {
		Strategy *strat = strats[i];
		if(getConfig().dg.calculateVerbose.get()) {
			s << indent << "substrat " << i << ":" << std::endl;
			indentLevel++;
		}
		strat->execute(s, strats[i - 1]->getOutput());
		if(getConfig().dg.calculateVerbose.get()) indentLevel--;
	}

	if(getConfig().dg.calculateVerbose.get()) indentLevel--;
}

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod
