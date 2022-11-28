#ifndef MOD_LIB_TERM_IO_READ_HPP
#define MOD_LIB_TERM_IO_READ_HPP

#include <mod/lib/Term/RawTerm.hpp>
#include <mod/lib/Term/WAM.hpp>

#include <functional>
#include <optional>

namespace mod::lib {
struct StringStore;
} // namespace mod::lib
namespace mod::lib::Term::Read {

// throws lib::IO::ParsingError on error
RawTerm rawTerm(const std::string &data, const StringStore &stringStore);

} // namespace mod::lib::Term::Read

#endif // MOD_LIB_TERM_IO_READ_HPP