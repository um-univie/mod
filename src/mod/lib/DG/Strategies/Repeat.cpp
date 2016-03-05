#include "Repeat.h"

#include <mod/Config.h>
#include <mod/lib/DG/Strategies/GraphState.h>

#include <iostream>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

Repeat::Repeat(Strategy* strat, std::size_t limit)
: Strategy(strat->getMaxComponents()), strat(strat), limit(limit) { }

Repeat::~Repeat() {
	delete strat;
	for(Strategy *s : subStrats) delete s;
}

Strategy *Repeat::clone() const {
	return new Repeat(strat->clone(), limit);
}

void Repeat::printInfo(std::ostream &s) const {
	s << indent << "Repeat, limit = " << limit << std::endl;
	indentLevel++;
	for(unsigned int i = 0; i < subStrats.size(); i++) {
		s << indent << "Substrat " << i << ":" << std::endl;
		indentLevel++;
		subStrats[i]->printInfo(s);
		indentLevel--;
	}
	printBaseInfo(s);
	indentLevel--;
}

const GraphState &Repeat::getOutput() const {
	assert(!subStrats.empty());
	if(subStrats.back()->getOutput().getSubset(0).empty()) {
		if(subStrats.size() == 1) return *input;
		else return subStrats[subStrats.size() - 2]->getOutput();
	} else return subStrats.back()->getOutput();
}

bool Repeat::isConsumed(const Graph::Single *g) const {
	for(const Strategy *strat : subStrats) {
		if(strat->isConsumed(g)) return true;
	}
	return false;
}

void Repeat::setExecutionEnvImpl() {
	strat->setExecutionEnv(getExecutionEnv());
}

void Repeat::executeImpl(std::ostream& s, const GraphState& input) {
	s << indent << "Repeat, limit = " << limit << std::endl;
	if(limit == 0) return;
	indentLevel++;
	// handle the first
	Strategy *firstStrat = strat->clone();
	firstStrat->setExecutionEnv(getExecutionEnv());
	s << indent << "Substrat 0" << std::endl;
	indentLevel++;
	firstStrat->execute(s, input);
	indentLevel--;
	s << indent << "Got " << firstStrat->getOutput().getSubset(0).size() << " graphs" << std::endl;
	subStrats.push_back(firstStrat);
	if(firstStrat->getOutput().getSubset(0).empty()) {
		indentLevel--;
		return;
	}
	for(unsigned int i = 0; i < limit - 1; i++) {
		Strategy *subStrat = strat->clone();
		subStrat->setExecutionEnv(getExecutionEnv());
		s << indent << "Substrat " << (i + 1) << std::endl;
		indentLevel++;
		subStrat->execute(s, subStrats.at(i)->getOutput());
		indentLevel--;
		s << indent << "Got " << subStrat->getOutput().getSubset(0).size() << " graphs" << std::endl;
		subStrats.push_back(subStrat);
		if(!getConfig().dg.ignoreSubset.get()) {
			if(subStrat->getOutput().getSubset(0).empty()) {
				if(getConfig().dg.calculateVerbose.get()) s << indent << "Breaking repeat due to empty subset" << std::endl;
				break;
			}
		}
		if(!getConfig().dg.disableRepeatFixedPointCheck.get()) {
			if(subStrat->getOutput() == subStrats[i]->getOutput()) {
				if(getConfig().dg.calculateVerbose.get()) s << indent << "Breaking repeat due to fixed point" << std::endl;
				break;
			}
		}
	}
	indentLevel--;
}

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod