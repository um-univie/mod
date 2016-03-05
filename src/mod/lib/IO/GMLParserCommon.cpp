#include "GMLParserCommon.h"

#include <mod/lib/IO/IO.h>

#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_char.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/qi_difference.hpp>
#include <boost/spirit/include/qi_eol.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_int.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_nonterminal.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_real.hpp>
#include <boost/spirit/include/qi_sequence.hpp>

namespace mod {
namespace lib {
namespace IO {
namespace Parser {
namespace GML {

template<typename Iter>
Skipper<Iter>::Skipper() : Skipper::base_type(start) {
	start %= qi::space | (qi::lit('#') >> *(qi::char_ - qi::eol) >> qi::eol);
}

namespace {

void vis2dWarn() {
	lib::IO::log() << "WARNING: 'vis2d' ignored in GML." << std::endl;
}

} // namespace

template<typename Iter, template<typename> class Skipper>
VertexAndEdgeList<Iter, Skipper>::VertexAndEdgeList() : VertexAndEdgeList::base_type(start) {
	start %= qi::lit("[") > vertexOrEdges(qi::_val);
	vertexOrEdges %= qi::lit(']') | (vertexOrEdge(qi::_r1) > vertexOrEdges(qi::_r1));
	vertexOrEdges.name("vertexOrEdge");
	vertexOrEdge %= vertex[phx::bind(&GML::pushVertex, qi::_r1, qi::_1)]
			| edge[phx::bind(&GML::pushEdge, qi::_r1, qi::_1)];
	vertexOrEdge.name("vertexOrEdge");
	vertex %= qi::lit("node") > "[" > "id" > qi::int_ > "label" > escapedString >> -vis2d >> "]";
	vertex.name("nodeStatement");
	vis2d %= qi::lit("vis2d") > '[' > 'x' > qi::double_ > 'y' > qi::double_ > qi::lit(']')[&vis2dWarn];
	vis2d.name("vis2d");
	edge %= qi::lit("edge") > "[" > "source" > qi::int_ > "target" > qi::int_ > "label" > escapedString >> "]";
	edge.name("edgeStatement");
}

template struct Skipper<IteratorType>;
template struct VertexAndEdgeList<IteratorType, Skipper>;

} // namespace GML
} // namespace Parser
} // namespace IO
} // namespace lib
} // namespace mod
