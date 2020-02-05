#ifndef MOD_LIB_RC_COMMONSG_H
#define MOD_LIB_RC_COMMONSG_H

#include <mod/lib/GraphMorphism/LabelledMorphism.hpp>
#include <mod/lib/GraphMorphism/McGregorCommonFinder.hpp>
#include <mod/lib/RC/MatchMaker/LabelledMatch.hpp>
#include <mod/lib/Rules/Real.hpp>

#include <jla_boost/graph/morphism/callbacks/Unwrapper.hpp>
#include <jla_boost/graph/morphism/models/Vector.hpp>

namespace mod {
namespace lib {
namespace RC {

struct Common {

	Common(int verbosity, IO::Logger logger, bool maximum, bool connected)
			: verbosity(verbosity), logger(logger), maximum(maximum), connected(connected) {}

	template<typename Callback>
	void makeMatches(const lib::Rules::Real &rFirst,
						  const lib::Rules::Real &rSecond,
						  Callback callback,
						  LabelSettings labelSettings) {
		const auto mr = [&rFirst, &rSecond, &callback, this](auto &&m, const auto &gSecond, const auto &gFirst) -> bool {
			return callback(rFirst, rSecond, std::move(m), verbosity, logger);
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
	const int verbosity;
	IO::Logger logger;
	const bool maximum;
	const bool connected;
};

} // namespace RC
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RC_COMMONSG_H */

