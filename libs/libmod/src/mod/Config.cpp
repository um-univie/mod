#include "Config.hpp"

#include <iostream>

namespace mod {

std::ostream &operator<<(std::ostream &s, const LabelType &lt) {
	switch(lt) {
	case LabelType::String:
		s << "string";
		break;
	case LabelType::Term:
		s << "term";
		break;
	}
	return s;
}

std::ostream &operator<<(std::ostream &s, const LabelSettings &ls) {
	const auto printRelation = [&](LabelRelation rel) -> std::ostream& {
		switch(rel) {
		case LabelRelation::Isomorphism: return s << "Iso";
		case LabelRelation::Specialisation: return s << "Spec";
		case LabelRelation::Unification: return s << "Uni";
		}
		return s;
	};
	s << "LS{";
	switch(ls.type) {
	case LabelType::String:
		s << "string";
		break;
	case LabelType::Term:
		s << "term(";
		printRelation(ls.relation);
		s << ")";
		break;
	}
	if(ls.withStereo) {
		s << ", stereo(";
		printRelation(ls.stereoRelation);
		s << ")";
	}
	return s << "}";
}

Config &getConfig() {
	static Config config;
	return config;
}

} // namespace mod
