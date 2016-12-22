#include "NonHyperDerivations.h"

#include <mod/Derivation.h>
#include <mod/Rule.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/Rules/Real.h>

namespace mod {
namespace lib {
namespace DG {

NonHyperDerivations::NonHyperDerivations(std::vector<Derivation> derivations)
: NonHyper({}), derivations(new std::vector<Derivation>(std::move(derivations))) {
	calculate();
}

std::string NonHyperDerivations::getType() const {
	return "DGDerivations";
}

void NonHyperDerivations::calculateImpl() {
	assert(derivations);
	// add all graphs
	for(const auto &der : *derivations) {
		for(auto g : der.left) this->addGraphAsVertex(g);
		for(auto g : der.right) this->addGraphAsVertex(g);
	}
	// add derivations
	for(const auto &der : *derivations) {
		auto makeSide = [this](const mod::Derivation::GraphList & graphs) -> const lib::Graph::Base* {
			if(graphs.size() == 1) return &graphs.front()->getGraph();
			auto *merge = new lib::Graph::Merge();
			for(auto g : graphs) merge->mergeWith(g->getGraph());
			merge->lock();
			return this->addToMergeStore(merge);
		};
		auto *left = makeSide(der.left);
		auto *right = makeSide(der.right);
		const lib::Rules::Real *rule = nullptr;
		if(der.rule) rule = &der.rule->getRule();
		this->suggestDerivation(left, right, rule);
	}
	derivations.release();
}

void NonHyperDerivations::listImpl(std::ostream &s) const {
	s << "(nothing to list for " << getType() << "(" << getId() << ")" << std::endl;
}

} // namespace DG
} // namespace lib
} // namespace mod