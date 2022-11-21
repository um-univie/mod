#ifndef MOD_LIB_IO_JSON_HPP
#define MOD_LIB_IO_JSON_HPP

// We don't control the symbol visibility of the json library and json-schema library,
// so we need to restore the default when using their header files.
// For example typeinfo objects for exceptions _must_ be exported.
#pragma GCC visibility push(default)

#include <nlohmann/json-schema.hpp>

#pragma GCC visibility pop

#include <optional>

namespace mod::lib::IO {

void writeJsonFile(const std::string &name, const nlohmann::json &j);
std::optional<nlohmann::json> readJson(const std::vector<std::uint8_t> &data, std::ostream &err);

bool validateJson(const nlohmann::json &j,
                  const nlohmann::json_schema::json_validator &validator,
                  std::ostream &err,
                  const std::string &msg);

} // namespace mod::lib::IO

#endif // MOD_LIB_IO_JSON_HPP
