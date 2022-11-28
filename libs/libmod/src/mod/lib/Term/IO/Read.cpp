#include "Read.hpp"

#include <mod/Error.hpp>
#include <mod/lib/IO/IO.hpp>
#include <mod/lib/IO/Parsing.hpp>
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

#include <iostream>

namespace mod::lib::Term::Read {
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

struct Converter : public boost::static_visitor<RawTerm> {
	Converter(const Converter &) = delete;
	Converter &operator=(const Converter &) = delete;

	Converter(const StringStore &stringStore) : stringStore(stringStore) {}

	RawTerm operator()(const Variable &var) {
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
		return RawVariable{stringId};
	}

	RawTerm operator()(const Structure &str) {
		auto stringId = stringStore.getIndex(str.name);
		std::vector<RawTerm> arguments;
		for(const auto &a : str.arguments)
			arguments.push_back(boost::apply_visitor(*this, a));
		return RawStructure{stringId, std::move(arguments)};
	}
private:
	const StringStore &stringStore;
	std::size_t nextVar = 0;
};

} // namespace detail

RawTerm rawTerm(const std::string &data, const StringStore &stringStore) {
	detail::Term term;
	IO::parse(data.begin(), data.end(), detail::parser::term, term, true, x3::space);
	detail::Converter converter(stringStore);
	return boost::apply_visitor(converter, term);
}

} // namespace mod::lib::Term::Read

BOOST_FUSION_ADAPT_STRUCT(mod::lib::Term::Read::detail::Variable,
                          (std::string, name))
BOOST_FUSION_ADAPT_STRUCT(mod::lib::Term::Read::detail::Structure,
                          (std::string, name)
		                          (std::vector<mod::lib::Term::Read::detail::Term>, arguments))