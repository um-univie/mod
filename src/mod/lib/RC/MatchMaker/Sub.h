#ifndef MOD_LIB_RC_SUB_H
#define MOD_LIB_RC_SUB_H

#include <mod/Config.h>
#include <mod/Misc.h>
#include <mod/lib/Algorithm.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/IO/Rule.h>
#include <mod/lib/RC/MatchMaker/ComponentWiseUtil.h>

#include <jla_boost/graph/dpo/FilteredGraphProjection.hpp>

#include <boost/optional.hpp>

namespace mod {
namespace lib {
namespace RC {

struct Sub {
	using VertexMapType = jla_boost::GraphMorphism::InvertibleVectorVertexMap<lib::Rules::GraphType, lib::Rules::GraphType>;
public:

	explicit Sub(bool allowPartial) : allowPartial(allowPartial) { }

	template<typename MR>
	void makeMatches(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, MR mr) const {
		if(allowPartial)
			makeMatchesInternal<true>(rFirst, rSecond, mr);
		else
			makeMatchesInternal<false>(rFirst, rSecond, mr);
	}

private:

	template<bool AllowPartial, typename MR>
	void makeMatchesInternal(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, MR mr) const {
		const auto &rFirstRight = get_labelled_right(rFirst.getDPORule());
		const auto &rSecondLeft = get_labelled_left(rSecond.getDPORule());
		auto mp = makeRuleRuleComponentMonomorphism(rFirstRight, rSecondLeft, false);
		auto mm = makeMultiDimSelector<AllowPartial>(
				get_num_connected_components(rFirstRight),
				get_num_connected_components(rSecondLeft), mp);
		for(const auto &position : mm) {
			auto maybeMap = matchFromPosition(rFirst, rSecond, position);
			if(!maybeMap) continue;
			auto map = *std::move(maybeMap);
			bool continue_ = mr(rFirst, rSecond, std::move(map));
			if(!continue_) break;
		}
	}
public:
	template<typename Position>
	boost::optional<VertexMapType> matchFromPosition(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, const std::vector<Position> &position) const;
private:
	const bool allowPartial;
};

template<typename Position>
inline boost::optional<Sub::VertexMapType>
Sub::matchFromPosition(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, const std::vector<Position> &position) const {
	const auto &coreFirst = rFirst.getGraph();
	const auto &coreSecond = rSecond.getGraph();
	auto nullFirst = boost::graph_traits<lib::Rules::GraphType>::null_vertex();
	auto nullSecond = boost::graph_traits<lib::Rules::GraphType>::null_vertex();
	VertexMapType map(coreSecond, coreFirst);
	for(std::size_t pId = 0; pId < position.size(); pId++) {
		if(position[pId].disabled) continue;
		if(position[pId].host == rSecond.getDPORule().numLeftComponents) {
			if(!allowPartial) MOD_ABORT; // we should have gotten this
			continue;
		}
		const auto &pattern = get_component_graph(pId, get_labelled_right(rFirst.getDPORule()));
		assert(position[pId].iterMorphism != position[pId].iterMorphismEnd);
		auto &&morphism = *position[pId].iterMorphism;
		assert(morphism.size() == num_vertices(pattern));
		//	{ // TODO: this is not adapted from the Super version
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
		for(auto vFirst : asRange(vertices(pattern))) {
			assert(get_inverse(map, coreSecond, coreFirst, vFirst) == nullSecond);
			auto vSecond = get(morphism, coreFirst, coreSecond, vFirst);
			assert(vSecond != nullSecond);
			if(get(map, coreSecond, coreFirst, vSecond) != nullFirst)
				return boost::none; // the combined match is not injective
			put(map, coreSecond, coreFirst, vSecond, vFirst);
		}
	}
	return map;
}

} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_SUB_H */