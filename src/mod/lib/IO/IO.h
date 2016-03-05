#ifndef MOD_LIB_IO_IO_H
#define	MOD_LIB_IO_IO_H

#include <fstream>
#include <iosfwd>
#include <utility>

namespace mod {
namespace lib {
namespace IO {

struct FileHandle {
	explicit FileHandle(std::string name);

	operator std::ostream &() {
		return stream;
	}

	operator std::string() {
		return name;
	}

	template<typename T>
	friend std::ostream &operator<<(FileHandle &s, T &&t) {
		return s.stream << std::forward<T>(t);
	}
private:
	std::ofstream stream;
	std::string name;
};

std::string getUniqueFilePrefix();
std::string escapeForLatex(const std::string &str);
std::string asLatexMath(const std::string &str);

std::ostream &nullStream();
std::ostream &post();
std::ostream &log();

} // namespace IO
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_IO_IO_H */
