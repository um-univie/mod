#ifndef MOD_LIB_RC_MATCH_MAKER_PARALLEL_H
#define MOD_LIB_RC_MATCH_MAKER_PARALLEL_H

namespace mod {
namespace lib {
namespace RC {

struct Parallel {

	template<typename Callback>
	void makeMatches(const Rule::Real &rFirst, const Rule::Real &rSecond, Callback callback) {
		using Map = jla_boost::GraphMorphism::InvertibleVectorVertexMap<lib::Rule::GraphType, lib::Rule::GraphType>;
		const auto &gFirst = rFirst.getGraph();
		const auto &gSecond = rSecond.getGraph();
		Map map(gSecond, gFirst);
		callback(rFirst, rSecond, std::move(map));
	}
};

} // namespace RC
} // namespace lib
} // namespace mod


#endif /* MOD_LIB_RC_MATCH_MAKER_PARALLEL_H */