#include "MatchBuilder.hpp"

#include <mod/lib/GraphMorphism/LabelledMorphism.hpp>
#include <mod/lib/RC/ComposeRuleRealGeneric.hpp>

namespace mod::lib::RC {

bool MatchBuilder::VertexPred::operator()(lib::Rules::Vertex vSecond, lib::Rules::Vertex vFirst) {
	return lib::GraphMorphism::predicateSelectByLabelSeetings(
			get_labelled_left(rSecond.getDPORule()), get_labelled_right(rFirst.getDPORule()),
			vSecond, vFirst, labelSettings);
}

// ----------------------------------------------------------------------------

bool MatchBuilder::EdgePred::operator()(lib::Rules::Edge eFirst, lib::Rules::Edge eSecond) {
	return lib::GraphMorphism::predicateSelectByLabelSeetings(
			get_labelled_left(rSecond.getDPORule()), get_labelled_right(rFirst.getDPORule()),
			eSecond, eFirst, labelSettings);
}

// ----------------------------------------------------------------------------

MatchBuilder::MatchBuilder(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, LabelSettings labelSettings)
		: rFirst(rFirst), rSecond(rSecond), labelSettings(labelSettings),
		  match(get_left(rSecond.getDPORule()), get_right(rFirst.getDPORule()),
		        EdgePred{rFirst, rSecond, labelSettings}, VertexPred{rFirst, rSecond, labelSettings}) {}

lib::Rules::Vertex MatchBuilder::getSecondFromFirst(lib::Rules::Vertex v) const {
	// the match is from rSecond to rFirst,
	// so rSecond is left and rFirst is right
	return match.leftFromRight(v);
}

lib::Rules::Vertex MatchBuilder::getFirstFromSecond(lib::Rules::Vertex v) const {
	// the match is from rSecond to rFirst,
	// so rFirst is right and rSecond is left
	return match.rightFromLeft(v);
}

std::size_t MatchBuilder::size() const {
	assert(match.getPreForcedStackSize() == 0); // should have no forced
	return match.getPreStackSize();
}

bool MatchBuilder::push(lib::Rules::Vertex vFirst, lib::Rules::Vertex vSecond) {
	// the match is from rSecond to rFirst
	return match.preTryPush(vSecond, vFirst);
}

void MatchBuilder::pop() {
	assert(match.getPreStackSize() != 0); // there must be something
	assert(match.getPreStackSize() != match.getPreForcedStackSize()); // and it must be something checked
	assert(match.getPreStackSize() == match.getTotalStackSize()); // and there should be no enumeration going on
	match.prePop();
}

std::unique_ptr<lib::Rules::Real> MatchBuilder::compose(bool verbose) const {
	auto ls = labelSettings;
	// specialization for the morphisms R1 <- M -> L2 means the direct map L2 -> R1 should use unification
	// so it models doing the pushout of the span
	if(ls.relation == LabelRelation::Specialisation)
		ls.relation = LabelRelation::Unification;
	std::unique_ptr<lib::Rules::Real> res;
	const auto mr = [this, verbose, &res](auto &&m, const auto &gSecond, const auto &gFirst) -> bool {
		return detail::MatchMakerCallback([&res](std::unique_ptr<lib::Rules::Real> rr) {
			res = std::move(rr);
			return true;
		})(rFirst, rSecond, std::move(m), verbose, IO::Logger(std::cout));
	};
	[[maybe_unused]] const bool cont = lib::GraphMorphism::matchSelectByLabelSettings(
			get_labelled_left(rSecond.getDPORule()), get_labelled_right(rFirst.getDPORule()),
			match.getVertexMap(), ls, mr);
	assert(cont == bool(res));
	return res;
}

std::vector<std::unique_ptr<lib::Rules::Real>> MatchBuilder::composeAll(bool maximum, bool verbose) const {
	auto ls = labelSettings;
	// specialization for the morphisms R1 <- M -> L2 means the direct map L2 -> R1 should use unification
	// so it models doing the pushout of the span
	if(ls.relation == LabelRelation::Specialisation)
		ls.relation = LabelRelation::Unification;
	std::vector<std::unique_ptr<lib::Rules::Real>> res;
	auto m = match; // make a copy we can modify
	auto mrCompose = [this, verbose, &res](auto &&m, const auto &gSecond, const auto &gFirst) -> bool {
		return detail::MatchMakerCallback([&res](std::unique_ptr<lib::Rules::Real> rr) {
			res.push_back(std::move(rr));
			return true;
		})(rFirst, rSecond, std::move(m), verbose, IO::Logger(std::cout));
	};
	const auto &gLeft = get_labelled_left(rSecond.getDPORule());
	const auto &gRight = get_labelled_right(rFirst.getDPORule());
	if(maximum) {
		auto mr = jla_boost::GraphMorphism::makeMaxmimumSubgraphCallback(get_graph(gLeft), get_graph(gRight), mrCompose);
		// the non-extended match, and importantly send it through mr to make sure maximum is respected
		lib::GraphMorphism::matchSelectByLabelSettings(gLeft, gRight, match.getSizedVertexMap(), ls, std::ref(mr));
		// enumerate the rest
		m(std::ref(mr));
		// and now release the maximum ones
		mr.outputMatches();
	} else {
		// the non-extended match
		lib::GraphMorphism::matchSelectByLabelSettings(gLeft, gRight, match.getSizedVertexMap(), ls, mrCompose);
		// enumerate the rest
		m(mrCompose);
	}
	return res;
}

} // namespace mod::lib::RC