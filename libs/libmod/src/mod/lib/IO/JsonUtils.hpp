#ifndef MOD_LIB_IO_JSONUTILS_HPP
#define MOD_LIB_IO_JSONUTILS_HPP

#include <boost/optional.hpp>

#include <nlohmann/json-schema.hpp>

namespace mod {
namespace lib {
namespace IO {

// returns the full filename
std::string writeJsonFile(const std::string &name, const nlohmann::json &j);
boost::optional<nlohmann::json> readJson(const std::vector<std::uint8_t> &data, std::ostream &err);

bool validateJson(const nlohmann::json &j,
                  const nlohmann::json_schema::json_validator &validator,
                  std::ostream &err,
                  const std::string &msg);

} // namespace IO
} // namespace lib
} // namespace mod

#endif // MOD_LIB_IO_JSONUTILS_HPP
