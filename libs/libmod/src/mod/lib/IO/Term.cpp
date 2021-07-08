#include "Term.hpp"

#include <mod/Error.hpp>
#include <mod/lib/IO/IO.hpp>
#include <mod/lib/IO/ParsingUtil.hpp>
#include <mod/lib/StringStore.hpp>

#include <boost/spirit/home/x3/auxiliary/eps.hpp>
#include <boost/spirit/home/x3/char.hpp>
#include <boost/spirit/home/x3/directive/lexeme.hpp>
#include <boost/spirit/home/x3/operator/alternative.hpp>
#include <boost/spirit/home/x3/operator/kleene.hpp>
#include <boost/spirit/home/x3/operator/list.hpp>
#include <boost/spirit/home/x3/operator/optional.hpp>
#include <boost/spirit/home/x3/string.hpp>

#include <boost/fusion/adapted/struct.hpp>

#include <boost/variant/static_visitor.hpp>
#include <boost/variant/variant.hpp>

#include <iomanip>
#include <unordered_set>

#include <iostream>

namespace mod::lib::IO::Term::Read {
namespace detail {

struct Structure;

struct Variable {
	std::string name;
};

struct Term : x3::variant<Variable, x3::forward_ast<Structure>> {
	using base_type::base_type;
	using base_type::operator=;
};

struct Structure {
	std::string name;
	std::vector<Term> arguments;
};

namespace {
namespace parser {
#define FIRST "A-Za-z0-9=#:.+-"
#define SECOND "_"

const x3::rule<struct term, Term> term = "term";
const x3::rule<struct function, Structure> function = "function";
const x3::rule<struct termList, std::vector<Term> > termList = "term list";
const x3::rule<struct variable, Variable> variable = "variable";
const x3::rule<struct identifier, std::string> identifier = "identifier";

const auto term_def = function | variable;
const auto function_def = identifier >> -('(' > termList > ')');
const auto termList_def = term % ',';
const auto variable_def = (x3::lexeme['_' > identifier] >> x3::eps) | x3::string("*");
const auto identifier_def = x3::lexeme[x3::char_(FIRST) > *x3::char_(SECOND FIRST)];

BOOST_SPIRIT_DEFINE(term, function, termList, variable, identifier)
} // namespace parser
} // namespace

struct Converter : public boost::static_visitor<lib::Term::RawTerm> {
	Converter(const Converter &) = delete;
	Converter &operator=(const Converter &) = delete;

	Converter(const StringStore &stringStore) : stringStore(stringStore) {}

	lib::Term::RawTerm operator()(const Variable &var) {
		std::size_t stringId;
		if(var.name == "*") {
			for(;; ++nextVar) {
				std::string name = "X" + std::to_string(nextVar) + "_";
				if(!stringStore.hasString(name)) {
					stringId = stringStore.getIndex(name);
					break;
				}
			}
		} else {
			stringId = stringStore.getIndex(var.name);
		}
		return lib::Term::RawVariable{stringId};
	}

	lib::Term::RawTerm operator()(const Structure &str) {
		auto stringId = stringStore.getIndex(str.name);
		std::vector<lib::Term::RawTerm> arguments;
		for(const auto &a : str.arguments)
			arguments.push_back(boost::apply_visitor(*this, a));
		return lib::Term::RawStructure{stringId, std::move(arguments)};
	}
private:
	const StringStore &stringStore;
	std::size_t nextVar = 0;
};

} // namespace detail

lib::Term::RawTerm rawTerm(const std::string &data, const StringStore &stringStore) {
	detail::Term term;
	parse(data.begin(), data.end(), detail::parser::term, term, x3::space);
	detail::Converter converter(stringStore);
	return boost::apply_visitor(converter, term);
}

} // namespace mod::lib::IO::Term::Read
namespace mod::lib::IO::Term::Write {
namespace {

std::ostream &rawVarFromCell(std::ostream &s, lib::Term::Cell cell) {
	using namespace lib::Term;
	assert(cell.tag == Cell::Tag::REF);
	switch(cell.REF.addr.type) {
	case AddressType::Heap:
		return s << "_H" << cell.REF.addr.addr;
	case AddressType::Temp:
		return s << "_T" << cell.REF.addr.addr;
	}
	MOD_ABORT;
}

} // namespace

std::ostream &rawTerm(const lib::Term::RawTerm &term, const StringStore &strings, std::ostream &s) {
	struct Printer {
		void operator()(lib::Term::RawVariable v) const {
			s << "_" << strings.getString(v.name);
		}

		void operator()(const lib::Term::RawStructure &str) const {
			s << strings.getString(str.name);
			if(!str.args.empty()) {
				s << '(';
				std::visit(*this, str.args.front());
				for(int i = 1; i != str.args.size(); i++) {
					s << ", ";
					std::visit(*this, str.args[i]);
				}
				s << ')';
			}
		}
	public:
		std::ostream &s;
		const StringStore &strings;
	};
	std::visit(Printer{s, strings}, term);
	return s;
}

std::ostream &element(lib::Term::Cell cell, const StringStore &strings, std::ostream &s) {
	using namespace lib::Term;
	switch(cell.tag) {
	case Cell::Tag::STR:
		return s << "STR " << cell.STR.addr;
	case Cell::Tag::Structure:
		s << strings.getString(cell.Structure.name);
		if(cell.Structure.arity > 0)
			s << "/" << cell.Structure.arity;
		return s;
	case Cell::Tag::REF:
		return s << "REF " << cell.REF.addr;
	}
	__builtin_unreachable();
}

void wam(const lib::Term::Wam &machine, const StringStore &strings, std::ostream &s) {
	wam(machine, strings, s, [](lib::Term::Address, std::ostream &) {});
}

void wam(const lib::Term::Wam &machine, const StringStore &strings, std::ostream &s,
         std::function<void(lib::Term::Address, std::ostream &s)> addressCallback) {
	using namespace lib::Term;
	s << "Heap:" << std::endl;
	for(std::size_t i = 0; i < machine.getHeap().size(); i++) {
		Cell cell = machine.getHeap()[i];
		s << std::setw(5) << std::left << i;
		element(cell, strings, s);
		addressCallback({AddressType::Heap, i}, s);
		s << std::endl;
	}
	s << "-------------------------------------------------" << std::endl;
	s << "Temp:" << std::endl;
	for(std::size_t i = 0; i < machine.getTemp().size(); i++) {
		Cell cell = machine.getTemp()[i];
		s << std::setw(5) << std::left << i;
		element(cell, strings, s);
		addressCallback({AddressType::Temp, i}, s);
		s << std::endl;
	}
	s << "-------------------------------------------------" << std::endl;
}

std::ostream &
term(const lib::Term::Wam &machine, lib::Term::Address addr, const StringStore &strings, std::ostream &s) {
	using namespace lib::Term;

	struct Printer {
		Printer(const Wam &machine, const StringStore &strings, std::ostream &s)
				: machine(machine), strings(strings), s(s) {
			occurred[0].resize(machine.getHeap().size(), 0);
			occurred[1].resize(machine.getTemp().size(), 0);
		}

		void operator()(Address addr) {
			Cell cell = machine.getCell(addr);
			switch(cell.tag) {
			case Cell::Tag::REF:
				if(cell.REF.addr == addr
				   || occurred[static_cast<int>(cell.REF.addr.type)][cell.REF.addr.addr] != 0
						) {
					rawVarFromCell(s, cell);
				} else (*this)(cell.REF.addr);
				break;
			case Cell::Tag::STR:
				(*this)(cell.STR.addr);
				break;
			case Cell::Tag::Structure:
				if(occurred[static_cast<int>(addr.type)][addr.addr] != 0) {
					wam(machine, strings, std::cout);
					std::cout << "addr.addr = " << addr.addr << std::endl;
					std::cout << "occurred:" << std::endl;
					for(int aType : {0, 1}) {
						for(const auto o : occurred[aType]) {
							if(o == 0) continue;
							std::cout << "   [" << aType << "]: " << o << std::endl;
						}
					}
				}
				assert(occurred[static_cast<int>(addr.type)][addr.addr] == 0);
				s << strings.getString(cell.Structure.name);
				if(cell.Structure.arity > 0) {
					++occurred[static_cast<int>(addr.type)][addr.addr];
					s << "(";
					(*this)(addr + 1);
					for(std::size_t i = 2; i <= cell.Structure.arity; i++) {
						s << ", ";
						(*this)(addr + i);
					}
					s << ")";
					--occurred[static_cast<int>(addr.type)][addr.addr];
				}
				break;
			}
		}
	private:
		const Wam &machine;
		const StringStore &strings;
		std::ostream &s;
		std::array<std::vector<int>, 2> occurred;
	};
	Printer(machine, strings, s)(addr);
	return s;
}

std::ostream &
mgu(const lib::Term::Wam &machine, const lib::Term::MGU &mgu, const StringStore &strings, std::ostream &s) {
	using namespace lib::Term;
	switch(mgu.status) {
	case MGU::Status::Exists:
		s << "Exists: ";
		break;
	case MGU::Status::Fail:
		term(machine, mgu.errorLeft, strings, s << "Fail(") << " != ";
		term(machine, mgu.errorRight, strings, s) << ")";
		return s;
	}
	bool first = true;
	for(auto binding : mgu.bindings) {
		if(binding.type == AddressType::Heap && binding.addr >= mgu.preHeapSize) continue;
		if(!first) s << ", ";
		first = false;
		Cell cell;
		cell.tag = Cell::Tag::REF;
		cell.REF.addr = binding;
		rawVarFromCell(s, cell) << " = ";
		term(machine, binding, strings, s);
	}
	return s;
}

} // namespace mod::lib::IO::Term::Write
namespace mod::lib::Term {

std::ostream &operator<<(std::ostream &s, Address addr) {
	switch(addr.type) {
	case AddressType::Heap:
		s << "H";
		break;
	case AddressType::Temp:
		s << "T";
		break;
	}
	return s << "[" << addr.addr << "]";
}

} // namespace mod::lib::Term

BOOST_FUSION_ADAPT_STRUCT(mod::lib::IO::Term::Read::detail::Variable,
                          (std::string, name))
BOOST_FUSION_ADAPT_STRUCT(mod::lib::IO::Term::Read::detail::Structure,
                          (std::string, name)
		                          (std::vector<mod::lib::IO::Term::Read::detail::Term>, arguments))