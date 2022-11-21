#ifndef MOD_LIB_IO_CONFIG_HPP
#define MOD_LIB_IO_CONFIG_HPP

#include <mod/Config.hpp>
#include <mod/lib/IO/Json.hpp>

namespace mod {

void to_json(nlohmann::json &j, LabelType v);
void from_json(const nlohmann::json &j, LabelType &v);
void to_json(nlohmann::json &j, LabelRelation v);
void from_json(const nlohmann::json &j, LabelRelation &v);
void to_json(nlohmann::json &j, LabelSettings v);
LabelSettings from_json(const nlohmann::json &j);

} // namespace mod

#endif // MOD_LIB_IO_CONFIG_HPP