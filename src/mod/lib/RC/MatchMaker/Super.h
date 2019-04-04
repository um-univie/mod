#ifndef MOD_LIB_RC_SUPER_H
#define MOD_LIB_RC_SUPER_H

#include <mod/Config.h>
#include <mod/Misc.h>
#include <mod/lib/Algorithm.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/IO/Rule.h>
#include <mod/lib/RC/MatchMaker/ComponentWiseUtil.h>
#include <mod/lib/RC/MatchMaker/LabelledMatch.h>
#include <mod/lib/Rules/Properties/Term.h>
#include <mod/lib/Term/WAM.h>

#include <jla_boost/graph/dpo/FilteredGraphProjection.hpp>

#include <boost/optional.hpp>

namespace mod {
namespace lib {
namespace RC {

struct Super {
	using GraphDom = lib::Rules::LabelledRule::LeftGraphType;
	using GraphCodom = lib::Rules::LabelledRule::RightGraphType;
	using VertexMapType = jla_boost::GraphMorphism::InvertibleVectorVertexMap<GraphDom, GraphCodom>;
public:

	Super(bool allowPartial, bool enforceConstraints)
	: allowPartial(allowPartial), enforceConstraints(enforceConstraints) { }

	void makeMatches(const auto &rFirst, const auto &rSecond, auto &&mr, LabelSettings labelSettings) const {
		if(allowPartial)
			makeMatchesInternal<true>(rFirst, rSecond, mr, labelSettings);
		else
			makeMatchesInternal<false>(rFirst, rSecond, mr, labelSettings);
	}
private:

	template<bool AllowPartial>
	void makeMatchesInternal(const auto &rFirst, const auto &rSecond, auto &&mr, LabelSettings labelSettings) const {
		if(getConfig().componentSG.verbose.get()) {
			IO::log() << "ComponentSG: " << rFirst.getName() << " -> " << rSecond.getName() << std::endl;
			IO::log() << "ComponentSG: " << "labelSettings = " << labelSettings << std::endl;
		}
		initByLabelSettings(rFirst, rSecond, labelSettings);
		const auto &lgDomPatterns = get_labelled_left(rSecond.getDPORule());
		const auto &lgCodomHosts = get_labelled_right(rFirst.getDPORule());
		if(get_num_connected_components(lgDomPatterns) == 0) {
			IO::log() << "RCSuper: rSecond L has no vertices, rule = " << rSecond.getName() << std::endl;
			MOD_ABORT;
		}
		if(get_num_connected_components(lgCodomHosts) == 0) {
			IO::log() << "RCSuper: rFirst R has no vertices, rule = " << rFirst.getName() << std::endl;
			MOD_ABORT;
		}
		//		IO::log() << "rFirstRight:\n";
		//		for(auto v : asRange(vertices(get_graph(rFirstRight)))) {
		//			IO::log() << v << ":";
		//			for(auto vOut : asRange(adjacent_vertices(v, get_graph(rFirstRight))))
		//				IO::log() << " " << vOut;
		//			IO::log() << "\n";
		//		}
		//		IO::log() << "\n";
		//		IO::log() << "rSecondLeft:\n";
		//		for(auto v : asRange(vertices(get_graph(rSecondLeft)))) {
		//			IO::log() << v << ":";
		//			for(auto vOut : asRange(adjacent_vertices(v, get_graph(rSecondLeft))))
		//				IO::log() << " " << vOut;
		//			IO::log() << "\n";
		//		}
		//		IO::log() << std::endl;
		auto mp = makeRuleRuleComponentMonomorphism(lgDomPatterns, lgCodomHosts, enforceConstraints, labelSettings);
		auto mm = makeMultiDimSelector<AllowPartial>(
				get_num_connected_components(lgDomPatterns),
				get_num_connected_components(lgCodomHosts), mp);
		if(getConfig().componentSG.verbose.get()) {
			IO::log() << "ComponentSG: " << "Match matrix, " << mm.morphisms.size() << " x " << mm.morphisms.front().size() << std::endl;
			for(int i = 0; i != mm.morphisms.size(); ++i) {
				IO::log() << "ComponentSG: ";
				if(mm.preDisabled[i]) IO::log() << "x:";
				else IO::log() << " :";
				const auto &m = mm.morphisms[i];
				for(int j = 0; j != m.size(); ++j)
					IO::log() << " " << std::setw(2) << m[j].size();
				IO::log() << std::endl;
			}
		}
		for(const auto &position : mm) {
			auto maybeMap = matchFromPosition(rFirst, rSecond, position);
			if(!maybeMap) {
				if(getConfig().componentSG.verbose.get())
					IO::log() << "ComponentSG: matchFromPosition returned none." << std::endl;
				continue;
			}
			auto map = *std::move(maybeMap);
			bool continue_ = handleMapByLabelSettings(rFirst, rSecond, std::move(map), mr, labelSettings);
			if(!continue_) break;
		}
	}
public:
	template<typename Position>
	boost::optional<VertexMapType> matchFromPosition(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, const std::vector<Position> &position) const;
private:
	bool allowPartial;
	bool enforceConstraints;
};

template<typename Position>
inline boost::optional<Super::VertexMapType>
Super::matchFromPosition(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, const std::vector<Position> &position) const {
	const auto &lgDom = get_labelled_left(rSecond.getDPORule());
	const auto &gDom = get_graph(lgDom);
	const auto &gCodom = get_graph(get_labelled_right(rFirst.getDPORule()));
	const auto vNullDom = boost::graph_traits<Super::GraphDom>::null_vertex();
	const auto vNullCodom = boost::graph_traits<Super::GraphCodom>::null_vertex();
	auto map = VertexMapType(gDom, gCodom);
	for(std::size_t pId = 0; pId < position.size(); ++pId) {
		if(position[pId].disabled) continue;
		if(position[pId].host == rFirst.getDPORule().numRightComponents) {
			if(!allowPartial) MOD_ABORT; // we should have gotten this
			continue;
		}
		const auto &gDomPattern = get_component_graph(pId, lgDom);
		assert(position[pId].iterMorphism != position[pId].iterMorphismEnd);
		auto &&morphism = *position[pId].iterMorphism;
		assert(morphism.size() == num_vertices(gDomPattern));
		//	{
		//		IO::log() << "from:";
		//		for(unsigned int i = 0; i < subMatch.size(); i++) IO::log() << "\t" << i;
		//		IO::log() << std::endl;
		//		IO::log() << "to:  ";
		//		for(unsigned int i = 0; i < subMatch.size(); i++) {
		//			IO::log() << "\t";
		//			if(subMatch[i] == boost::graph_traits<Rule::Real::ComponentGraph>::null_vertex()) IO::log() << "-";
		//			else IO::log() << get(boost::vertex_index_t(), patterns[pattern], subMatch[i]);
		//		}
		//		IO::log() << std::endl;
		//	}
		for(const auto vDomPattern : asRange(vertices(gDomPattern))) {
			assert(get(map, gDom, gCodom, vDomPattern) == vNullCodom);
			const auto vCodomHost = get(morphism, gDom, gCodom, vDomPattern);
			assert(vCodomHost != vNullCodom);
			if(get_inverse(map, gDom, gCodom, vCodomHost) != vNullDom)
				return boost::none; // the combined match is not injective
			put(map, gDom, gCodom, vDomPattern, vCodomHost);
		}
	}
	return map;
}

} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_SUPER_H */
