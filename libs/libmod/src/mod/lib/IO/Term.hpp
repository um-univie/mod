#ifndef MOD_LIB_IO_TERM_HPP
#define MOD_LIB_IO_TERM_HPP

#include <mod/lib/Term/RawTerm.hpp>
#include <mod/lib/Term/WAM.hpp>

#include <functional>
#include <optional>

namespace mod::lib {
struct StringStore;
} // namespace mod::lib
namespace mod::lib::IO::Term::Read {
// throws lib::IO::ParsingError on error
lib::Term::RawTerm rawTerm(const std::string &data, const StringStore &stringStore);
} // namespace mod::lib::IO::Term::Read
namespace mod::lib::IO::Term::Write {
std::ostream &rawTerm(const lib::Term::RawTerm &term, const StringStore &strings, std::ostream &s);
std::ostream &element(lib::Term::Cell cell, const StringStore &strings, std::ostream &s);
void wam(const lib::Term::Wam &machine, const StringStore &strings, std::ostream &s);
void wam(const lib::Term::Wam &machine, const StringStore &strings, std::ostream &s,
		std::function<void(lib::Term::Address, std::ostream &s)> addressCallback);
std::ostream &term(const lib::Term::Wam &machine, lib::Term::Address addr, const StringStore &strings, std::ostream &s);
std::ostream &mgu(const lib::Term::Wam &machine, const lib::Term::MGU &mgu, const StringStore &strings, std::ostream &s);
} // namespace mod::lib::IO::Term::Write

#endif // MOD_LIB_IO_TERM_HPP