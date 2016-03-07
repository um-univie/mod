#ifndef MOD_LIB_IO_PARSERCOMMON_H
#define	MOD_LIB_IO_PARSERCOMMON_H

#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_rule.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

namespace spirit = boost::spirit;
namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;
namespace fusion = boost::fusion;

namespace mod {
namespace lib {
namespace IO {
namespace Parser {
using BaseIteratorType = std::istream_iterator<char>;
using IteratorType = spirit::multi_pass<BaseIteratorType>;

template<typename Attr, typename Iter = IteratorType, typename Skipper = qi::space_type>
struct QiRule {
	using type = qi::rule<Iter, Attr, Skipper>;
};

template<typename Attr, typename Iter = IteratorType>
struct QiRuleLexeme {
	using type = qi::rule<Iter, Attr>;
};

template<char Begin, char End, typename Iter = IteratorType>
struct EscapedString : public qi::grammar<Iter, std::string()> {
	// is explicitly instantiated in the cpp file to reduce compile times
	EscapedString();
private:
	typename QiRuleLexeme<std::string(), Iter>::type escapedString;
	typename QiRuleLexeme<char(), Iter>::type escaped, plain;
	typename QiRuleLexeme<char(), Iter>::type tab, explicitBackslash, implicitBackslash;
};

template<typename Iter, bool Explicit = false >
struct ErrorHandler {

	ErrorHandler(std::ostream &s) : s(s) { }

	template<typename What>
	void operator()(Iter iterStart, Iter iterEnd, Iter iterError, const What &what) const {
		Iter lineStart = iterStart;
		unsigned int lineNum = 1;
		unsigned int colNum = 1;
		for(Iter i = iterStart; i != iterError; ++i) {
			colNum++;
			if(*i == '\n') {
				lineNum++;
				lineStart = i;
				colNum = 1;
			}
		}
		if(lineNum > 1) ++lineStart;
		Iter lineEnd = iterError;
		for(; lineEnd != iterEnd && *lineEnd != '\n'; ++lineEnd);
		s << "Parsing error. ";
		if(Explicit) s << what;
		else s << "Expected " << what;
		s << " at " << lineNum << ":" << colNum << ":" << std::endl;
		std::string line(lineStart, lineEnd);
		unsigned int visualCol = 0;
		for(unsigned int i = 0; i < line.size(); i++) {
			if(line[i] == '\t') {
				unsigned int tabSize = 3;
				s << std::string(tabSize, ' ');
				if(i < colNum - 1) visualCol += tabSize;
			} else {
				s << line[i];
				if(i < colNum - 1) visualCol++;
			}
		}
		s << std::endl;
		s << std::string(visualCol, '-') << "^" << std::endl;
		s << "(please report if the location is inaccurate)" << std::endl;
	}
private:
	std::ostream &s;
};

template <typename Iter, typename Expr, typename Attr>
bool parse(std::ostream &err, Iter &first, Iter last, const Expr &expr, Attr &attr) {
	Iter start = first;
	bool res = qi::parse(first, last, expr, attr);
	if(res && first == last) return true;
	if(!res) return false; // most often expectation failure
	(ErrorHandler<Iter, true>(err))(start, last, first, "Did not expect any more input");
	return false;
}

template <typename Iter, typename Expr, typename Skipper, typename Attr>
bool phrase_parse(std::ostream &err, Iter &first, Iter last, const Expr &expr, const Skipper &skipper, Attr &attr) {
	Iter start = first;
	bool res = qi::phrase_parse(first, last, expr, skipper, attr);
	if(res && first == last) return true;
	if(!res) return false; // most often expectation failure
	(ErrorHandler<Iter, true>(err))(start, last, first, "Did not expect any more input");
	return false;
}

} // namespace Parser
} // namespace IO
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_IO_PARSERCOMMON_H */