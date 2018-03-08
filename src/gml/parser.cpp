#include "parser.hpp"

#include <boost/spirit/home/x3/auxiliary/attr.hpp>
#include <boost/spirit/home/x3/auxiliary/eol.hpp>
#include <boost/spirit/home/x3/char.hpp>
#include <boost/spirit/home/x3/core/parse.hpp>
#include <boost/spirit/home/x3/directive/lexeme.hpp>
#include <boost/spirit/home/x3/nonterminal.hpp>
#include <boost/spirit/home/x3/numeric/int.hpp>
#include <boost/spirit/home/x3/numeric/real.hpp>
#include <boost/spirit/home/x3/operator/alternative.hpp>
#include <boost/spirit/home/x3/operator/difference.hpp>
#include <boost/spirit/home/x3/operator/kleene.hpp>
#include <boost/spirit/home/x3/operator/sequence.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

namespace spirit = boost::spirit;
namespace x3 = boost::spirit::x3;

namespace gml {
namespace parser {

struct LocationHandler {

	template<typename Iterator, typename Context>
	void on_success(const Iterator &first, const Iterator &last, ast::LocationInfo &locInfo, const Context&) {
		const auto &pos = first.get_position();
		locInfo.line = pos.line;
		locInfo.column = pos.column;
	}
};

struct keyValue_class : LocationHandler {
};

struct value_class : LocationHandler {
};

const x3::rule<struct skipper> skipper = "skipper";
const x3::rule<struct gml, ast::List> gml = "gml";
const x3::rule<struct list, ast::List> list = "list";
const x3::rule<struct listInner, std::vector<ast::KeyValue> > listInner = "listInner";
const x3::rule<struct keyValue_class, ast::KeyValue> keyValue = "keyValue";
const x3::rule<struct key, std::string> key = "key";
const x3::rule<struct value_class, ast::Value> value = "value";
const x3::rule<struct valueInner, ast::Value> valueInner = "valueInner";
const x3::rule<struct listOuter, ast::List> listOuter = "listOuter";
const x3::rule<struct string, std::string> string = "string";
#define GML_PARSER_CHAR_DEF(name) const x3::rule<struct name, char> name = #name;
GML_PARSER_CHAR_DEF(escaped);
GML_PARSER_CHAR_DEF(plain);
GML_PARSER_CHAR_DEF(tab);
GML_PARSER_CHAR_DEF(explicitBackslash);
GML_PARSER_CHAR_DEF(implicitBackslash);
#undef GML_PARSER_CHAR_DEF

const auto skipper_def = x3::space | (x3::lit('#') >> *(x3::char_ - x3::eol) >> x3::eol);
const auto gml_def = list;
const auto list_def = listInner;
const auto listInner_def = *keyValue;
const auto keyValue_def = key > value;
const auto key_def = x3::lexeme[x3::char_("a-zA-Z") >> *x3::char_("a-zA-Z0-9")];
const auto value_def = valueInner;
const auto valueInner_def = x3::real_parser<double, x3::strict_real_policies<double> >() | x3::int_ | string | listOuter;
const auto listOuter_def = x3::lit('[') > (list >> ']'); // TODO: remove parens when the move_to mess in X3 has been fixed
const auto string_def = x3::lexeme[x3::lit('"') > *(escaped | plain) > x3::lit('"')];
const auto escaped_def = '\\' >> (x3::char_('"') | tab | explicitBackslash | implicitBackslash);
const auto plain_def = (x3::char_ - x3::char_('"') - x3::char_('\n'));
const auto tab_def = 't' >> x3::attr('\t');
const auto explicitBackslash_def = '\\' >> x3::attr('\\');
const auto implicitBackslash_def = x3::attr('\\');

BOOST_SPIRIT_DEFINE(skipper, gml, list, listInner, keyValue, key, value, valueInner, listOuter, string, escaped, plain, tab, explicitBackslash, implicitBackslash);

template<typename TextIter>
bool parse(TextIter &textIter, const TextIter &textIterEnd, ast::List &ast, std::ostream &err) {
	using Iter = spirit::classic::position_iterator2<TextIter>;
	Iter iter(textIter, textIterEnd), iterEnd;
	auto doError = [&]() {
		const auto &pos = iter.get_position();
		auto lineNumber = pos.line;
		auto column = pos.column;
		std::string line = iter.get_currentline();
		err << "Parsing failed at " << lineNumber << ":" << column << ":\n";
		err << line << "\n";
		err << std::string(column - 1, '-') << "^\n";
	};
	try {
		bool res = x3::phrase_parse(iter, iterEnd, gml, skipper, ast);
		if(!res || iter != iterEnd) {
			doError();
			err << "End of x3 error.\n";
			return false;
		}
	} catch(const x3::expectation_failure<Iter> &e) {
		iter = e.where();
		doError();
		err << "Expected " << e.which() << ".\n";
		err << "End of x3 error.\n";
		return false;
	}
	return true;
}

bool parse(std::istream &s, ast::List &ast, std::ostream &err) {

	struct FlagsHolder {

		FlagsHolder(std::istream &s) : s(s), flags(s.flags()) { }

		~FlagsHolder() {
			s.flags(flags);
		}
	private:
		std::istream &s;
		std::ios::fmtflags flags;
	} flagsHolder(s);
	s.unsetf(std::ios::skipws);

	using Iter = spirit::multi_pass<std::istream_iterator<char> >;
	Iter iterBegin(s), iterEnd;
	return parse(iterBegin, iterEnd, ast, err);
}

} // namespace parser
} // namespace gml