#ifndef MOD_LIB_RC_SUB_H
#define MOD_LIB_RC_SUB_H

#include <mod/Config.hpp>
#include <mod/Misc.hpp>
#include <mod/lib/Algorithm.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/IO/IO.hpp>
#include <mod/lib/IO/Rule.hpp>
#include <mod/lib/RC/MatchMaker/ComponentWiseUtil.hpp>
#include <mod/lib/RC/MatchMaker/LabelledMatch.hpp>
#include <mod/lib/Rules/Properties/Term.hpp>
#include <mod/lib/Term/WAM.hpp>

#include <jla_boost/graph/dpo/FilteredGraphProjection.hpp>

#include <boost/optional.hpp>

namespace mod {
namespace lib {
namespace RC {

struct Sub {
	using GraphDom = lib::Rules::LabelledRule::LeftGraphType;
	using GraphCodom = lib::Rules::LabelledRule::RightGraphType;
	using VertexMapType = jla_boost::GraphMorphism::InvertibleVectorVertexMap<GraphDom, GraphCodom>;
public:

	explicit Sub(int verbosity, IO::Logger logger, bool allowPartial)
			: verbosity(verbosity), logger(logger), allowPartial(allowPartial) {}

	template<typename MR>
	void makeMatches(const lib::Rules::Real &rFirst,
						  const lib::Rules::Real &rSecond,
						  MR mr,
						  LabelSettings labelSettings) const {
		if(allowPartial)
			makeMatchesInternal<true>(rFirst, rSecond, mr, labelSettings);
		else
			makeMatchesInternal<false>(rFirst, rSecond, mr, labelSettings);
	}

private:

	template<bool AllowPartial, typename MR>
	void makeMatchesInternal(const lib::Rules::Real &rFirst,
									 const lib::Rules::Real &rSecond,
									 MR mr,
									 LabelSettings labelSettings) const {
		initByLabelSettings(rFirst, rSecond, labelSettings);
		const auto &lgCodomPatterns = get_labelled_right(rFirst.getDPORule());
		const auto &lgDomHosts = get_labelled_left(rSecond.getDPORule());
		IO::Logger logger(IO::log());
		auto mp = makeRuleRuleComponentMonomorphism(lgCodomPatterns, lgDomHosts, false, labelSettings,
																  false, logger);
		auto mm = makeMultiDimSelector<AllowPartial>(
				get_num_connected_components(lgCodomPatterns),
				get_num_connected_components(lgDomHosts), mp);
		for(const auto &position : mm) {
			auto maybeMap = matchFromPosition(rFirst, rSecond, position);
			if(!maybeMap) continue;
			auto map = *std::move(maybeMap);
			bool continue_ = handleMapByLabelSettings(rFirst, rSecond, std::move(map), mr, labelSettings,
																	verbosity, logger);
			if(!continue_) break;
		}
	}

public:
	template<typename Position>
	boost::optional<VertexMapType> matchFromPosition(const lib::Rules::Real &rFirst,
																	 const lib::Rules::Real &rSecond,
																	 const std::vector<Position> &position) const;
private:
	const int verbosity;
	IO::Logger logger;
	const bool allowPartial;
};

template<typename Position>
inline boost::optional<Sub::VertexMapType>
Sub::matchFromPosition(const lib::Rules::Real &rFirst,
							  const lib::Rules::Real &rSecond,
							  const std::vector<Position> &position) const {
	const auto &gDom = get_graph(get_labelled_left(rSecond.getDPORule()));
	const auto &lgCodom = get_labelled_right(rFirst.getDPORule());
	const auto &gCodom = get_graph(lgCodom);
	const auto vNullDom = boost::graph_traits<Sub::GraphDom>::null_vertex();
	(void) vNullDom;
	const auto vNullCodom = boost::graph_traits<Sub::GraphCodom>::null_vertex();
	auto map = VertexMapType(gDom, gCodom);
	for(std::size_t pId = 0; pId < position.size(); pId++) {
		if(position[pId].disabled) continue;
		if(position[pId].host == rSecond.getDPORule().numLeftComponents) {
			if(!allowPartial) MOD_ABORT; // we should have gotten this
			continue;
		}
		const auto &gCodomPattern = get_component_graph(pId, lgCodom);
		assert(position[pId].iterMorphism != position[pId].iterMorphismEnd);
		auto &&morphism = *position[pId].iterMorphism;
		assert(morphism.size() == num_vertices(gCodomPattern));
		for(const auto vCodomPattern : asRange(vertices(gCodomPattern))) {
			assert(get_inverse(map, gDom, gCodom, vCodomPattern) == vNullDom);
			const auto vDomHost = get(morphism, gCodom, gDom, vCodomPattern);
			assert(vDomHost != vNullDom);
			if(get(map, gDom, gCodom, vDomHost) != vNullCodom)
				return boost::none; // the combined match is not injective
			put(map, gDom, gCodom, vDomHost, vCodomPattern);
		}
	}
	return map;
}


} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_SUB_H */