#ifndef MOD_LIB_IO_IO_H
#define MOD_LIB_IO_IO_H

#include <iosfwd>
#include <string>

namespace mod {
namespace lib {
namespace IO {

std::string getUniqueFilePrefix();
std::string escapeForLatex(const std::string &str);
std::string asLatexMath(const std::string &str);

std::ostream &nullStream();
std::ostream &post();
std::ostream &log();

struct Logger {
	explicit Logger(std::ostream &s) : s(s) {}

	std::ostream &indent() const;
	std::ostream &sep(char c) const;
public:
	std::ostream &s;
	int indentLevel = 0;
};

} // namespace IO
} // namespace lib
} // namespace mod

#endif   /* MOD_LIB_IO_IO_H */
