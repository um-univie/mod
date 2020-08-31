#ifndef MOD_LIB_TERM_RAWTERM_H
#define	MOD_LIB_TERM_RAWTERM_H

#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/variant.hpp>

#include <limits>
#include <list>
#include <map>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace mod {
namespace lib {
struct StringStore;
namespace Term {
struct Address;
struct Wam;
struct RawVariable;
struct RawStructure;

typedef boost::variant<RawVariable, RawStructure> RawTerm;

struct RawVariable {
	std::size_t name;
};

struct RawStructure {
	// Functor or Atom
	std::size_t name;
	std::vector<RawTerm> args;
};

using RawAppendStore = std::unordered_map<std::size_t, Address>;
Address append(Wam &machine, const RawTerm &term, RawAppendStore &varToAddr);

} // namespace Term
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_TERM_RAWTERM_H */