#ifndef MOD_LIB_RC_COMMONSG_H
#define MOD_LIB_RC_COMMONSG_H

#include <mod/lib/GraphMorphism/LabelledMorphism.h>
#include <mod/lib/GraphMorphism/McGregorCommonFinder.hpp>
#include <mod/lib/RC/MatchMaker/LabelledMatch.h>
#include <mod/lib/Rules/Real.h>

#include <jla_boost/graph/morphism/callbacks/Unwrapper.hpp>
#include <jla_boost/graph/morphism/models/Vector.hpp>

namespace mod {
namespace lib {
namespace RC {

struct Common {

	Common(bool maximum, bool connected) : maximum(maximum), connected(connected) { }

	template<typename Callback>
	void makeMatches(const lib::Rules::Real &rFirst, const lib::Rules::Real &rSecond, Callback callback, LabelSettings labelSettings) {
		const auto mr = [&rFirst, &rSecond, &callback, &labelSettings](auto &&m, const auto &gSecond, const auto &gFirst) -> bool {
			return callback(rFirst, rSecond, std::move(m));
		};
		const auto &lgDom = get_labelled_left(rSecond.getDPORule());
		const auto &lgCodom = get_labelled_right(rFirst.getDPORule());
		auto finder = lib::GraphMorphism::McGregorCommonFinder(maximum, connected);
		if(labelSettings.relation == LabelRelation::Specialisation) {
			MOD_ABORT;
		}
		if(labelSettings.withStereo && labelSettings.stereoRelation == LabelRelation::Specialisation) {
			MOD_ABORT;
		}
		lib::GraphMorphism::morphismSelectByLabelSettings(lgDom, lgCodom, labelSettings, finder, mr);
	}
private:
	const bool maximum;
	const bool connected;
};

} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_COMMONSG_H */

