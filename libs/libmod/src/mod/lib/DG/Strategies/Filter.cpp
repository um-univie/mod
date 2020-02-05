#include "Filter.hpp"

#include <mod/Config.hpp>
#include <mod/Function.hpp>
#include <mod/lib/DG/Strategies/GraphState.hpp>
#include <mod/lib/Graph/Single.hpp>

namespace mod {
namespace lib {
namespace DG {
namespace Strategies {

Filter::Filter(std::shared_ptr<mod::Function<bool(std::shared_ptr<graph::Graph>, const dg::Strategy::GraphState &,
																  bool)> > filterFunc, bool filterUniverse)
		: Strategy(1), filterFunc(filterFunc), filterUniverse(filterUniverse) {
	assert(filterFunc);
}

Strategy *Filter::clone() const {
	return new Filter(filterFunc->clone(), filterUniverse);
}

void Filter::preAddGraphs(std::function<void(std::shared_ptr<graph::Graph>, IsomorphismPolicy)> add) const {}

void Filter::printInfo(PrintSettings settings) const {
	settings.indent() << "Filter";
	std::ostream &s = settings.s;
	if(filterUniverse) s << "Universe";
	else s << "Subset";
	s << '\n';
	++settings.indentLevel;
	settings.indent() << "predicate = ";
	filterFunc->print(s);
	s << '\n';
	printBaseInfo(settings);
	--settings.indentLevel;
}

bool Filter::isConsumed(const Graph::Single *g) const {
	return false;
}

void Filter::executeImpl(PrintSettings settings, const GraphState &input) {
	if(settings.verbosity >= PrintSettings::V_Filter) {
		if(filterUniverse)
			settings.indent() << "FilterUniverse: ";
		else
			settings.indent() << "FilterSubset: ";
		if(settings.verbosity >= PrintSettings::V_FilterPred)
			filterFunc->print(settings.s);
		settings.s << std::endl;
		++settings.indentLevel;
	}
	assert(!output);
	dg::Strategy::GraphState graphState(
			[&input](std::vector<std::shared_ptr<graph::Graph> > &subset) {
				for(const lib::Graph::Single *g : input.getSubset(0)) subset.push_back(g->getAPIReference());
			},
			[&input](std::vector<std::shared_ptr<graph::Graph> > &universe) {
				for(const lib::Graph::Single *g : input.getUniverse()) universe.push_back(g->getAPIReference());
			});
	if(!filterUniverse) {
		output = new GraphState(input.getUniverse());
		assert(input.getSubsets().size() == 1); // TODO: fix when filter is parameterized by subset
		assert(input.hasSubset(0));
		unsigned int subsetIndex = 0;

		bool first = true;
		for(const lib::Graph::Single *g : input.getSubset(subsetIndex)) {
			// TODO: the adding to the subset could be faster as the index is the same as in the input ResultSet
			if((*filterFunc)(g->getAPIReference(), graphState, first)) {
				assert(output->isInUniverse(g));
				output->addToSubset(subsetIndex, g);
			}
			first = false;
		}
		// TODO: add the complete other subsets
		assert(input.getSubsets().size() == 1); // TODO: fix when filter is parameterized by subset
		assert(input.hasSubset(0));
	} else {
		std::map<const lib::Graph::Single *, bool> copyToOutput;
		bool first = true;
		for(const lib::Graph::Single *g : input.getUniverse()) {
			copyToOutput[g] = (*filterFunc)(g->getAPIReference(), graphState, first);
			first = false;
		}
		std::vector<const lib::Graph::Single *> newUniverse;

		for(const lib::Graph::Single *g : input.getUniverse()) {
			if(copyToOutput[g]) newUniverse.push_back(g);
		}
		output = new GraphState(newUniverse);

		for(const GraphState::SubsetStore::value_type &p : input.getSubsets()) {
			for(const lib::Graph::Single *g : p.second) {
				// TODO: this could be faster if we had access to the individual indices, as we could map old to new
				if(copyToOutput[g]) {
					assert(output->isInUniverse(g));
					output->addToSubset(p.first, g);
				}
			}
		}
	}
}

} // namespace Strategies
} // namespace DG
} // namespace lib
} // namespace mod