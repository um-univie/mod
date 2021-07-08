#ifndef MOD_LIB_TERM_RAWTERM_HPP
#define MOD_LIB_TERM_RAWTERM_HPP

#include <limits>
#include <list>
#include <map>
#include <ostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace mod::lib {
struct StringStore;
} // namespace mod::lib
namespace mod::lib::Term {
struct Address;
struct Wam;
struct RawVariable;
struct RawStructure;

using RawTerm = std::variant<RawVariable, RawStructure>;

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

} // namespace mod::lib::Term

#endif // MOD_LIB_TERM_RAWTERM_HPP