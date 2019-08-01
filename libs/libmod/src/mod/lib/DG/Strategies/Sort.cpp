#include "Sort.hpp"

#include <mod/Config.hpp>
#include <mod/Function.hpp>
#include <mod/lib/DG/Strategies/GraphState.hpp>
#include <mod/lib/Graph/Single.hpp>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

Sort::Sort(std::shared_ptr<mod::Function<bool(std::shared_ptr<graph::Graph>, std::shared_ptr<graph::Graph>, const dg::Strategy::GraphState&)> > less, bool doUniverse)
: Strategy(0), less(less), doUniverse(doUniverse) { }

Strategy *Sort::clone() const {
	return new Sort(less->clone(), doUniverse);
}

void Sort::preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>) > add) const { }

void Sort::printInfo(std::ostream &s) const {
	s << indent << "Sort";
	if(doUniverse) s << " universe";
	s << std::endl;
	indentLevel++;
	s << indent << "evaluation function = ";
	less->print(s);
	s << std::endl;
	printBaseInfo(s);
	indentLevel--;
}

bool Sort::isConsumed(const lib::Graph::Single *g) const {
	return false;
}

void Sort::executeImpl(std::ostream &s, const GraphState &input) {
	if(getConfig().dg.calculateVerbose.get()) {
		s << "Sort: ";
		less->print(s);
		s << std::endl;
		indentLevel++;
	}

	assert(!output);
	dg::Strategy::GraphState gs(
			[&input](std::vector<std::shared_ptr<graph::Graph> > &subset) {
				for(const lib::Graph::Single *g : input.getSubset(0)) subset.push_back(g->getAPIReference());
			},
	[&input](std::vector<std::shared_ptr<graph::Graph> > &universe) {
		for(const lib::Graph::Single *g : input.getUniverse()) universe.push_back(g->getAPIReference());
	});
	auto comp = [&gs, this](const lib::Graph::Single *g1, const lib::Graph::Single * g2) -> bool {
		return (*less)(g1->getAPIReference(), g2->getAPIReference(), gs);
	};

	output = new GraphState(input);
	assert(output->hasSubset(0)); // TODO: remove when the sorting is parameterized with the subset index
	assert(output->getSubsets().size() == 1);
	if(doUniverse) output->sortUniverse(comp);
	else output->sortSubset(0, comp); // TODO: put the subset index here
	if(getConfig().dg.calculateVerbose.get()) indentLevel--;
}

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod