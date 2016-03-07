#include "Config.h"

namespace mod {

Config &getConfig() {
	static Config config;
	return config;
}

} // namespace mod