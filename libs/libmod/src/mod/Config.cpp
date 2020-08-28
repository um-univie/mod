#include "Config.hpp"

#include <iostream>
#include <tuple>

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

bool operator==(LabelSettings a, LabelSettings b) {
	return std::tie(a.type, a.relation, a.withStereo, a.stereoRelation)
	       == std::tie(b.type, b.relation, b.withStereo, b.stereoRelation);
}

bool operator!=(LabelSettings a, LabelSettings b) {
	return !(a == b);
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

std::ostream &operator<<(std::ostream &s, SmilesClassPolicy p) {
	switch(p) {
	case SmilesClassPolicy::NoneOnDuplicate:
		return s << "noneOnDuplicate";
	case SmilesClassPolicy::ThrowOnDuplicate:
		return s << "throwOnDuplicate";
	case SmilesClassPolicy::MapUnique:
		return s << "mapUnique";
	}
	return s;
}

Config &getConfig() {
	static Config config;
	return config;
}

} // namespace mod
