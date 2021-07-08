#include "Result.hpp"

#include <ostream>

namespace mod::lib::IO {

std::ostream &operator<<(std::ostream &s, const Warnings &ws) {
	for(const auto &[msg, print] : ws.warnings)
		if(print) s << "WARNING: " << msg << "\n";
	return s;
}

} // namespace mod::lib::IO