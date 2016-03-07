#include "ParserCommon.h"

#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_attr.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_difference.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_sequence.hpp>

namespace mod {
namespace lib {
namespace IO {
namespace Parser {

template<char Begin, char End, typename Iter>
EscapedString<Begin, End, Iter>::EscapedString()
: EscapedString::base_type(escapedString) {
	escapedString %= qi::lit(Begin) > *(escaped | plain) >> qi::lit(End);
	escapedString.name("escapedString");
	escaped %= '\\' >> (qi::char_(End) | tab | explicitBackslash | implicitBackslash);
	escaped.name("escaped");
	plain %= (qi::char_ - qi::char_(End) - qi::char_('\n'));
	plain.name("plain");
	tab %= 't' >> qi::attr('\t');
	tab.name("tab");
	explicitBackslash %= '\\' >> qi::attr('\\');
	explicitBackslash.name("explicitBackslash");
	implicitBackslash %= qi::attr('\\');
	implicitBackslash.name("implicitBackslash");
}

template struct EscapedString<'[', ']', std::string::const_iterator>;
template struct EscapedString<'{', '}', std::string::const_iterator>;
template struct EscapedString<'"', '"', IteratorType>;
} // namespace Parser
} // namespace IO
} // namespace lib
} // namespace mod