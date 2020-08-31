#include "Config.hpp"

namespace mod {

void to_json(nlohmann::json &j, LabelType v) {
	switch(v) {
	case LabelType::String:
		j = "string";
		return;
	case LabelType::Term:
		j = "term";
		return;
	}
}

void from_json(const nlohmann::json &j, LabelType &v) {
	if(j == "string") v = LabelType::String;
	else if(j == "term") v = LabelType::Term;
}

void to_json(nlohmann::json &j, LabelRelation v) {
	switch(v) {
	case LabelRelation::Isomorphism:
		j = "isomorphism";
		return;
	case LabelRelation::Specialisation:
		j = "specialisation";
		return;
	case LabelRelation::Unification:
		j = "unification";
		return;
	}
}

void from_json(const nlohmann::json &j, LabelRelation &v) {
	if(j == "isomorphism") v = LabelRelation::Isomorphism;
	else if(j == "specialisation") v = LabelRelation::Specialisation;
	else if(j == "unification") v = LabelRelation::Unification;
}

void to_json(nlohmann::json &j, LabelSettings v) {
	j = nlohmann::json({
			                   {"type",           v.type},
			                   {"relation",       v.relation},
			                   {"withStereo",     v.withStereo},
			                   {"stereoRelation", v.stereoRelation}
	                   });
}

LabelSettings from_json(const nlohmann::json &j) {
	return LabelSettings(j["type"], j["relation"], j["withStereo"], j["stereoRelation"]);
}

} // namespace mod