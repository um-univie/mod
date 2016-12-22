#ifndef MOD_LIB_RC_COMMONSG_H
#define MOD_LIB_RC_COMMONSG_H

#include <mod/lib/GraphMorphism/LabelledMorphism.h>
#include <mod/lib/GraphMorphism/McGregorCommonFinder.hpp>
#include <mod/lib/Rules/Real.h>

#include <jla_boost/graph/morphism/VectorVertexMap.hpp>

namespace mod {
namespace lib {
namespace RC {
namespace detail {

template<typename Callback>
struct CallbackWrapper {

	CallbackWrapper(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, Callback callback)
	: rFirst(rFirst), rSecond(rSecond), callback(callback) { }

	template<typename Morphism, typename GraphLeft, typename GraphRight>
	bool operator()(Morphism &&m, const GraphRight &gSecond, const GraphLeft &gFirst) const {
		BOOST_CONCEPT_ASSERT((jla_boost::GraphMorphism::InvertibleVertexMapConcept<Morphism>));

		jla_boost::GraphMorphism::InvertibleVectorVertexMap<lib::Rules::GraphType, lib::Rules::GraphType>
				match(rSecond.getGraph(), rFirst.getGraph());
		for(auto vSecond : asRange(vertices(gSecond))) {
			auto vFirst = get(m, gSecond, gFirst, vSecond);
			put(match, rSecond.getGraph(), rFirst.getGraph(), vSecond, vFirst);
		}
		return callback(rFirst, rSecond, std::move(match));
	}
private:
	const lib::Rules::Real &rFirst;
	const lib::Rules::Real &rSecond;
	Callback callback;
};

} // namespace detail

struct Common {

	Common(bool maximum, bool connected) : maximum(maximum), connected(connected) { }

	template<typename Callback>
	void makeMatches(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, Callback callback) {
		detail::CallbackWrapper<Callback> mr(rFirst, rSecond, callback);
		const auto &gOuterSecondLeft = get_labelled_left(rSecond.getDPORule());
		const auto &gOuterFirstRight = get_labelled_right(rFirst.getDPORule());
		auto finder = lib::GraphMorphism::McGregorCommonFinder(maximum, connected);
		lib::GraphMorphism::morphismSelectByLabelSettings(gOuterSecondLeft, gOuterFirstRight, finder, mr);
	}
private:
	const bool maximum;
	const bool connected;
};

} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_COMMONSG_H */

