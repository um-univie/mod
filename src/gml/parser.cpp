#include "parser.hpp"

#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_attr.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/qi_difference.hpp>
#include <boost/spirit/include/qi_eol.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/spirit/include/qi_int.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_nonterminal.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_real.hpp>
#include <boost/spirit/include/qi_rule.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp>

namespace gml {
namespace parser {

namespace spirit = boost::spirit;
namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;
namespace classic = boost::spirit::classic;

template<typename Iter, bool Explicit = false >
struct ErrorHandler {

	ErrorHandler(std::ostream &s) : s(s) { }

	template<typename What>
	void operator()(Iter iterStart, Iter iterEnd, Iter iterError, const What &what) const {
		const auto &pos = iterError.get_position();
		s << "Parsing error. ";
		if(Explicit) s << what;
		else s << "Expected " << what;
		s << " at " << pos.line << ":" << pos.column << ". " << std::endl;
		std::size_t column = pos.column;
		const std::string line = iterError.get_currentline();
		for(std::size_t i = 0; i < line.size(); ++i) {
			if(line[i] == '\t') {
				s << std::string(4, ' '); // Spirit hardcodes tabsize to 4, right?
			} else {
				s << line[i];
			}
		}
		assert(column > 0);
		s << std::endl << std::string(column - 1, '-') << "^" << std::endl;
		s << "(please report if the location is inaccurate)" << std::endl;
	}
private:
	std::ostream &s;
};

template<typename Iter>
struct LocationHandler {

	void operator()(ast::LocationInfo &locInfo, Iter iterStart) const {
		const auto &pos = iterStart.get_position();
		locInfo.line = pos.line;
		locInfo.column = pos.column;
	}
};

template<typename Iter, template<typename> class Skipper>
struct Grammar : public qi::grammar<Iter, ast::List(), Skipper<Iter> > {
	using Skip = Skipper<Iter>;

	Grammar(std::ostream &err) : Grammar::base_type(start, "gml"), errorHandler(err) {
		start %= list.alias();
		list %= listInner;
		listInner %= *keyValue;
		keyValue %= key > value;
		keyValue.name("keyValue");
		key %= qi::char_("a-zA-Z") >> *qi::char_("a-zA-Z0-9");
		key.name("key");
		value %= valueInner;
		value.name("value");
		valueInner %= qi::real_parser<double, qi::strict_real_policies<double> >() | qi::int_ | string | listOuter;
		valueInner.name("value");
		listOuter = qi::lit('[') > list > ']';
		listOuter.name("listValue");
		string %= qi::lit('"') > *(escaped | plain) >> qi::lit('"');
		string.name("string");
		escaped %= '\\' >> (qi::char_('"') | tab | explicitBackslash | implicitBackslash);
		plain %= (qi::char_ - qi::char_('"') - qi::char_('\n'));
		tab %= 't' >> qi::attr('\t');
		explicitBackslash %= '\\' >> qi::attr('\\');
		implicitBackslash %= qi::attr('\\');

		qi::on_error<qi::fail>(start, errorHandler(qi::_1, qi::_2, qi::_3, qi::_4));
		// http://stackoverflow.com/questions/19612657/boostspirit-access-position-iterator-from-semantic-actions
		qi::on_success(keyValue, locationHandler(qi::_val, qi::_1));
		qi::on_success(value, locationHandler(qi::_val, qi::_1));
	}
private:
	qi::rule<Iter, ast::List(), Skip > start;
	qi::rule<Iter, ast::List(), Skip> list;
	qi::rule<Iter, std::vector<ast::KeyValue>(), Skip> listInner;
	qi::rule<Iter, ast::KeyValue(), Skip> keyValue;
	qi::rule<Iter, std::string() > key;
	qi::rule<Iter, ast::Value(), Skip> value;
	qi::rule<Iter, ast::ValueStore(), Skip> valueInner;
	qi::rule<Iter, ast::List(), Skip> listOuter;
private:
	qi::rule<Iter, std::string() > string;
	qi::rule<Iter, char() > escaped, plain, tab, explicitBackslash, implicitBackslash;
private:
	phx::function<ErrorHandler<Iter> > errorHandler;
	phx::function<LocationHandler<Iter> > locationHandler;
};

template<typename Iter>
struct Skipper : public qi::grammar<Iter> {

	Skipper() : Skipper::base_type(start) {
		start %= qi::space | (qi::lit('#') >> *(qi::char_ - qi::eol) >> qi::eol);
	}
private:
	qi::rule<Iter> start;
};

template<typename Iter>
bool parse(Iter &iterBegin, const Iter &iterEnd, ast::List &ast, std::ostream &err) {
	Iter start = iterBegin;
	Grammar<Iter, Skipper> grammar(err);
	bool res = qi::phrase_parse(iterBegin, iterEnd, grammar, Skipper<Iter>(), ast);
	if(res && iterBegin == iterEnd) return true;
	if(!res) return false; // most often expectation failure
	(ErrorHandler<Iter, true>(err))(start, iterEnd, iterBegin, "Did not expect any more input.");
	return false;
}

bool parse(std::istream &s, ast::List &ast, std::ostream &err) {
	using ForwardIter = spirit::multi_pass<std::istream_iterator<char> >;
	ForwardIter iterForwardBegin(s), iterForwardEnd;
	using Iter = classic::position_iterator2<ForwardIter>;
	Iter iterBegin(iterForwardBegin, iterForwardEnd), iterEnd;

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
	return parse(iterBegin, iterEnd, ast, err);
}

} // namespace parser
} // namespace gml