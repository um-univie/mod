#ifndef MOD_LIB_IO_PARSINGUTIL_H
#define MOD_LIB_IO_PARSINGUTIL_H

#include <boost/spirit/home/x3/support/traits/tuple_traits.hpp> // because it's missing somewhere inside operator/sequence.hpp

#include <boost/spirit/home/x3/core/parse.hpp>
#include <boost/spirit/home/x3/nonterminal.hpp>
#include <boost/spirit/home/x3/operator/sequence.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>

namespace spirit = boost::spirit;
namespace x3 = boost::spirit::x3;

namespace mod {
namespace lib {
namespace IO {
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
void doParserError(const TextIter &textIter, const PosIter &iter, const PosIter &iterEnd, std::ostream &err) {
	const auto lineNumber = iter.position();
	const auto lineRange = get_current_line(PosIter(textIter), iter, iterEnd);
	const auto column = get_column(lineRange.begin(), iter);
	err << "Parsing failed at " << lineNumber << ":" << column << ":\n";
	err << lineRange << "\n";
	err << std::string(column - 1, '-') << "^\n";
}

template<typename TextIter, typename PosIter>
void doParserExpectationError(const x3::expectation_failure<PosIter> &e, const TextIter &textIter, const PosIter &iterEnd, std::ostream &err) {
	const PosIter &iter = e.where();
	detail::doParserError(textIter, iter, iterEnd, err);
	err << "Expected " << e.which() << ".";
}

} // namespace detail

template<typename Iter>
using PositionIter = spirit::line_pos_iterator<Iter>;

template<typename TextIter, typename Parser, typename Attr, typename ...Skipper>
bool parse(const TextIter &textFirst, PositionIter<TextIter> &first, const PositionIter<TextIter> &last, const Parser &p, Attr &attr, std::ostream &err, const Skipper& ...skipper) {
	try {
		bool res = detail::ParseDispatch < Skipper...>::parse(first, last, p, attr, skipper...);
		if(!res || first != last) {
			detail::doParserError(textFirst, first, last, err);
			return false;
		}
	} catch(const x3::expectation_failure<PositionIter<TextIter> > &e) {
		detail::doParserExpectationError(e, textFirst, last, err);
		return false;
	}
	return true;
}

template<typename TextIter, typename Parser, typename Attr, typename ...Skipper>
bool parse(const TextIter &textFirst, const TextIter &textLast, const Parser &p, Attr &attr, std::ostream &err, const Skipper& ...skipper) {
	PositionIter<TextIter> first(textFirst);
	return parse(textFirst, first, PositionIter<TextIter>(textLast), p, attr, err, skipper...);
}

} // namespace IO
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_IO_PARSINGUTIL_H */