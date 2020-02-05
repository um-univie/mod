#include "Config.hpp"

#include <iostream>

namespace mod {

std::ostream &operator<<(std::ostream &s, const LabelType lt) {
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

std::ostream &operator<<(std::ostream &s, const LabelRelation lt) {
	switch(lt) {
	case LabelRelation::Isomorphism:
		return s << "isomorphism";
	case LabelRelation::Specialisation:
		return s << "specialisation";
	case LabelRelation::Unification:
		return s << "unification";
	}
	return s;
}

std::ostream &operator<<(std::ostream &s, const LabelSettings ls) {
	s << "LabelSettings{" << ls.type;
	switch(ls.type) {
	case LabelType::String:
		break;
	case LabelType::Term:
		s << "(" << ls.relation << ")";
		break;
	}
	if(ls.withStereo)
		s << ", stereo(" << ls.stereoRelation << ")";
	return s << "}";
}

std::ostream &operator<<(std::ostream &s, IsomorphismPolicy p) {
	switch(p) {
	case IsomorphismPolicy::Check:
		return s << "check";
	case IsomorphismPolicy::TrustMe:
		return s << "trustMe";
	}
	return s;
}

Config &getConfig() {
	static Config config;
	return config;
}

} // namespace mod
