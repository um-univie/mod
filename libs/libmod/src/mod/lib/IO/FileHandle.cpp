#include "FileHandle.hpp"

#include <mod/Error.hpp>
#include <mod/lib/IO/IO.hpp>

#include <algorithm>

namespace mod {
namespace lib {
namespace IO {

FileHandle::FileHandle(std::string name) : name(name) {
	// at most 1 dot
	if(1 < std::count(name.begin(), name.end(), '.')) MOD_ABORT;
	stream.open(name.c_str());
	if(!stream) {
		IO::log() << "Could not open file '" << name << "'." << std::endl;
		IO::log() << "Does 'out/' exist?" << std::endl;
		std::exit(1);
	}
}
} // namespace IO
} // namespace lib
} // namespace mod