#ifndef MOD_LIB_IO_PARSINGUTIL_HPP
#define MOD_LIB_IO_PARSINGUTIL_HPP

#include <mod/lib/IO/ParsingError.hpp>

#include <boost/lexical_cast.hpp>

#include <boost/spirit/home/x3/support/traits/tuple_traits.hpp> // because it's missing somewhere inside operator/sequence.hpp

#include <boost/spirit/home/x3/core/parse.hpp>
#include <boost/spirit/home/x3/nonterminal.hpp>
#include <boost/spirit/home/x3/operator/sequence.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>

namespace spirit = boost::spirit;
namespace x3 = boost::spirit::x3;

namespace mod::lib::IO {
namespace detail {

template<typename Skipper = void>
struct ParseDispatch {
	template<typename Iter, typename Parser, typename Attr>
	static bool parse(Iter &iter, Iter iterEnd, const Parser &p, Attr &attr, const Skipper &s) {
		return x3::phrase_parse(iter, iterEnd, p, s, attr);
	}
};

template<>
struct ParseDispatch<void> {
	template<typename Iter, typename Parser, typename Attr>
	static bool parse(Iter &iter, Iter iterEnd, const Parser &p, Attr &attr) {
		return x3::parse(iter, iterEnd, p, attr);
	}
};

template<typename TextIter, typename PosIter>
std::string makeParserError(const TextIter &textIter, const PosIter &iter, const PosIter &iterEnd) {
	const auto lineNumber = iter.position();
	const auto lineRange = get_current_line(PosIter(textIter), iter, iterEnd);
	const auto column = get_column(lineRange.begin(), iter);
	return "Parsing failed at " + std::to_string(lineNumber)
	       + ":" + std::to_string(column) + ":\n"
	       + boost::lexical_cast<std::string>(lineRange) + "\n" + std::string(column - 1, '-') + "^";
}

template<typename TextIter, typename PosIter>
std::string makeParserExpectationError(const x3::expectation_failure<PosIter> &e, const TextIter &textIter,
                                       const PosIter &iterEnd) {
	return detail::makeParserError(textIter, e.where(), iterEnd)
	       + "\nExpected " + e.which() + ".";
}

} // namespace detail

template<typename Iter>
using PositionIter = spirit::line_pos_iterator<Iter>;

template<typename TextIter, typename Parser, typename Attr, typename ...Skipper>
void parse(const TextIter &textFirst, PositionIter<TextIter> &first, const PositionIter<TextIter> &last,
           const Parser &p, Attr &attr, const Skipper &...skipper) {
	bool res;
	try {
		res = detail::ParseDispatch<Skipper...>::parse(first, last, p, attr, skipper...);
	} catch(const x3::expectation_failure<PositionIter<TextIter> > &e) {
		throw ParsingError{detail::makeParserExpectationError(e, textFirst, last)};
	}
	if(!res || first != last)
		throw ParsingError{detail::makeParserError(textFirst, first, last)};
}

template<typename TextIter, typename Parser, typename Attr, typename ...Skipper>
void parse(const TextIter &textFirst, const TextIter &textLast,
           const Parser &p, Attr &attr, const Skipper &...skipper) {
	PositionIter<TextIter> first(textFirst);
	return parse(textFirst, first, PositionIter<TextIter>(textLast), p, attr, skipper...);
}

} // namespace mod::lib::IO

#endif // MOD_LIB_IO_PARSINGUTIL_HPP