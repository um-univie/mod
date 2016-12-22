#ifndef MOD_LIB_IO_IO_H
#define	MOD_LIB_IO_IO_H

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

} // namespace IO
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_IO_IO_H */
