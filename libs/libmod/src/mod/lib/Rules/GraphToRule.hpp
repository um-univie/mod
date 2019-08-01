#ifndef MOD_LIB_RULES_GRAPHTORULE_H
#define MOD_LIB_RULES_GRAPHTORULE_H

#include <mod/lib/Rules/Real.hpp>
#include <mod/lib/Stereo/CloneUtil.hpp>

#include <iostream>

namespace mod {
namespace lib {
namespace Rules {

template<typename LGraph>
std::unique_ptr<Real> graphToRule(const LGraph &lg, Membership membership, const std::string &name) {
	using EdgeCat = lib::Stereo::EdgeCategory;
	const auto &g = get_graph(lg);
	const auto &pStringGraph = get_string(lg);
	assert(membership == Membership::Left || membership == Membership::Context || membership == Membership::Right);
	lib::Rules::LabelledRule rule;
	auto &gCore = get_graph(rule);
	rule.pString = std::make_unique<PropStringCore>(gCore);
	auto &pString = *rule.pString;

	for(const auto v : asRange(vertices(g))) {
		const auto vCore = add_vertex(gCore);
		assert(get(boost::vertex_index_t(), g, v) == get(boost::vertex_index_t(), gCore, vCore));
		gCore[vCore].membership = membership;
		const auto &label = pStringGraph[v];
		switch(membership) {
		case Membership::Left:
			pString.add(vCore, label, "");
			break;
		case Membership::Right:
			pString.add(vCore, "", label);
			break;
		case Membership::Context:
			pString.add(vCore, label, label);
			break;
		}
	}

	for(const auto e : asRange(edges(g))) {
		const auto vSrcCore = vertex(get(boost::vertex_index_t(), g, source(e, g)), gCore);
		const auto vTarCore = vertex(get(boost::vertex_index_t(), g, target(e, g)), gCore);
		const auto eCore = add_edge(vSrcCore, vTarCore, gCore).first;
		assert(get(boost::edge_index_t(), g, e) == get(boost::edge_index_t(), gCore, eCore));
		gCore[eCore].membership = membership;
		const auto &label = pStringGraph[e];
		switch(membership) {
		case Membership::Left:
			pString.add(eCore, label, "");
			break;
		case Membership::Right:
			pString.add(eCore, "", label);
			break;
		case Membership::Context:
			pString.add(eCore, label, label);
			break;
		}
	}

	if(has_stereo(lg)) {
		const auto &pStereoGraph = get_stereo(lg);
		std::vector<EdgeCat> eStereo(num_edges(g));

		for(const auto e : asRange(edges(g))) {
			const auto eId = get(boost::edge_index_t(), g, e);
			const auto eCore = *(edges(gCore).first + eId);
			const auto eIdCore = get(boost::edge_index_t(), gCore, eCore);
			assert(eId == eIdCore);
			eStereo[eIdCore] = pStereoGraph[e];
		}

		const auto vertexMap = [&gCore, &lg](const auto &vCore) {
			const auto vIdCore = get(boost::vertex_index_t(), gCore, vCore);
			const auto &g = get_graph(lg);
			return vertex(vIdCore, g);
		};
		const auto edgeMap = [&gCore, &lg](const auto &eCore) {
			const auto &g = get_graph(lg);
			const auto eIdCore = get(boost::edge_index_t(), gCore, eCore);
			const auto e = *std::next(edges(g).first, eIdCore);
			assert(get(boost::edge_index_t(), g, e) == eIdCore);
			return e;
		};
		const auto inf = Stereo::makeCloner(lg, gCore, vertexMap, edgeMap);
		rule.pStereo = std::make_unique<PropStereoCore>(gCore, inf, inf, jla_boost::AlwaysTrue(), jla_boost::AlwaysTrue());
	}

	rule.initComponents();
	std::string completeName;
	switch(membership) {
	case Membership::Left:
		completeName += "unbind";
		break;
	case Membership::Context:
		completeName += "id";
		break;
	case Membership::Right:
		completeName += "bind";
		break;
	default:
		MOD_ABORT;
		break;
	}
	completeName += "<";
	completeName += name;
	completeName += ">";
	auto res = std::make_unique<Real>(std::move(rule), boost::none);
	res->setName(completeName);
	return res;
}

} // namespace Rules
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RULES_GRAPHTORULE_H */
