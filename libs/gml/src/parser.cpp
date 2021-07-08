#include <gml/parser.hpp>

#include <boost/spirit/home/x3/auxiliary/attr.hpp>
#include <boost/spirit/home/x3/auxiliary/eol.hpp>
#include <boost/spirit/home/x3/auxiliary/eps.hpp>
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
#include <boost/spirit/include/support_line_pos_iterator.hpp>

namespace spirit = boost::spirit;
namespace x3 = boost::spirit::x3;

namespace gml::parser {
namespace {
constexpr int SpacesPerTabs = 4;
} // namespace

struct LocationHandler {
	template<typename Iterator, typename Context>
	void on_success(const Iterator &first, const Iterator &last, ast::LocationInfo &locInfo, const Context &) {
		locInfo.line = get_line(first);
		locInfo.column = get_column(first, last, SpacesPerTabs);
	}
};

struct keyValue_class : LocationHandler {
};

struct value_class : LocationHandler {
};

const x3::rule<struct skipper> skipper = "skipper";
const x3::rule<struct gml, ast::KeyValue> gml = "gml";
const x3::rule<struct list, ast::List> list = "list";
const x3::rule<struct listInner, std::vector<ast::KeyValue>> listInner = "key or ']'";
const x3::rule<struct keyValue_class, ast::KeyValue> keyValue = "keyValue";
const x3::rule<struct key, std::string> key = "key";
const x3::rule<struct value_class, ast::Value> value = "value";
const x3::rule<struct valueInner, ast::Value> valueInner = "valueInner";
const x3::rule<struct string, std::string> string = "string";
#define GML_PARSER_CHAR_DEF(name) const x3::rule<struct name, char> name = #name;
GML_PARSER_CHAR_DEF(escaped)
GML_PARSER_CHAR_DEF(plain)
GML_PARSER_CHAR_DEF(tab)
GML_PARSER_CHAR_DEF(explicitBackslash)
GML_PARSER_CHAR_DEF(implicitBackslash)
#undef GML_PARSER_CHAR_DEF

const auto skipper_def = x3::space | (x3::lit('#') >> *(x3::char_ - x3::eol) >> x3::eol);
const auto gml_def = keyValue;
const auto list_def = x3::lit('[') > listInner;
const auto listInner_def = (keyValue > listInner) | x3::lit(']');
const auto keyValue_def = key > value;
const auto key_def = x3::lexeme[x3::char_("a-zA-Z") >> *x3::char_("a-zA-Z0-9")];
const auto value_def = valueInner;
const auto valueInner_def = x3::real_parser<double, x3::strict_real_policies<double> >() | x3::int_ | string | list;
const auto string_def = x3::lexeme[x3::lit('"') > *(escaped | plain) > x3::lit('"')];
const auto escaped_def = '\\' >> (x3::char_('"') | tab | explicitBackslash | implicitBackslash);
const auto plain_def = (x3::char_ - x3::char_('"') - x3::char_('\n'));
const auto tab_def = 't' >> x3::attr('\t');
const auto explicitBackslash_def = '\\' >> x3::attr('\\');
const auto implicitBackslash_def = x3::attr('\\');

BOOST_SPIRIT_DEFINE(skipper, gml, list, listInner, keyValue, key, value, valueInner, string, escaped, plain, tab,
                    explicitBackslash, implicitBackslash)

ast::KeyValue parse(std::string_view src) {
	using PosIter = spirit::line_pos_iterator<std::string_view::const_iterator>;
	PosIter iter(src.begin()); // referenced in doError
	const auto makeError = [&]() {
		const auto lineNumber = iter.position();
		const auto lineRange = get_current_line(PosIter(src.begin()), iter, PosIter(src.end()));
		const auto column = get_column(lineRange.begin(), iter, SpacesPerTabs);
		std::string msg = "Parsing failed at " + std::to_string(lineNumber) + ":" + std::to_string(column) + ":\n";
		for(const char c : lineRange) {
			if(c == '\t') msg += std::string(SpacesPerTabs, ' ');
			else msg += c;
		}
		msg += "\n";
		msg += std::string(column - 1, '-');
		msg += "^";
		return msg;
	};
	try {
		ast::KeyValue ast;
		bool res = x3::phrase_parse(iter, PosIter(src.end()), gml, skipper, ast);
		if(!res || iter != PosIter(src.end()))
			throw error(makeError() + "\nEnd of x3 error.");
		return ast;
	} catch(const x3::expectation_failure<PosIter> &e) {
		iter = e.where();
		throw error(makeError()
		            + "\nExpected " + e.which() + ".\n"
		            + "End of x3 error.");
	}
}

} // namespace gml::parser