#ifndef MOD_LIB_IO_TERM_H
#define	MOD_LIB_IO_TERM_H

#include <mod/lib/Term/RawTerm.h>
#include <mod/lib/Term/WAM.h>

#include <boost/optional/optional.hpp>

namespace mod {
namespace lib {
struct StringStore;
namespace IO {
namespace Term {
namespace Read {
boost::optional<lib::Term::RawTerm> rawTerm(const std::string &data, const StringStore &stringStore, std::ostream &errorStream);
} // namespace Read
namespace Write {
std::ostream &rawTerm(const lib::Term::RawTerm &term, const StringStore &strings, std::ostream &s);
std::ostream &element(lib::Term::Cell cell, const StringStore &strings, std::ostream &s);
void wam(const lib::Term::Wam &machine, const StringStore &strings, std::ostream &s);
void wam(const lib::Term::Wam &machine, const StringStore &strings, std::ostream &s,
		std::function<void(lib::Term::Address, std::ostream &s)> addressCallback);
std::ostream &term(const lib::Term::Wam &machine, lib::Term::Address addr, const StringStore &strings, std::ostream &s);
std::ostream &mgu(const lib::Term::Wam &machine, const lib::Term::MGU &mgu, const StringStore &strings, std::ostream &s);
} // namespace Write
} // namespace Term
} // namespace IO
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_IO_TERM_H */