#ifndef MOD_LIB_RC_MATCH_MAKER_PARALLEL_H
#define MOD_LIB_RC_MATCH_MAKER_PARALLEL_H

#include <mod/lib/RC/MatchMaker/LabelledMatch.hpp>
#include <mod/lib/Rules/Real.hpp>

#include <jla_boost/graph/morphism/models/Vector.hpp>

namespace mod {
namespace lib {
namespace RC {

struct Parallel {

	template<typename Callback>
	void makeMatches(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, Callback callback, LabelSettings labelSettings) {
		using GraphDom = lib::Rules::LabelledRule::LeftGraphType;
		using GraphCodom = lib::Rules::LabelledRule::RightGraphType;
		using Map = jla_boost::GraphMorphism::InvertibleVectorVertexMap<GraphDom, GraphCodom>;
		const auto &gDom = get_graph(get_labelled_left(rSecond.getDPORule()));
		const auto &gCodom = get_graph(get_labelled_right(rFirst.getDPORule()));
		handleMapByLabelSettings(rFirst, rSecond, Map(gDom, gCodom), callback, labelSettings);
	}
};

} // namespace RC
} // namespace lib
} // namespace mod


#endif /* MOD_LIB_RC_MATCH_MAKER_PARALLEL_H */