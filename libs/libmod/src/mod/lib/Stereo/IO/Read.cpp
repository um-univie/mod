#include "Read.hpp"

#include <mod/lib/IO/Parsing.hpp>

#include <boost/fusion/adapted/struct.hpp>

#include <boost/spirit/home/x3/auxiliary/attr.hpp>
#include <boost/spirit/home/x3/auxiliary/eoi.hpp>
#include <boost/spirit/home/x3/char/char.hpp>
#include <boost/spirit/home/x3/char/char_class.hpp>
#include <boost/spirit/home/x3/directive/lexeme.hpp>
#include <boost/spirit/home/x3/numeric/int.hpp>
#include <boost/spirit/home/x3/operator/alternative.hpp>
#include <boost/spirit/home/x3/operator/kleene.hpp>
#include <boost/spirit/home/x3/operator/list.hpp>
#include <boost/spirit/home/x3/operator/not_predicate.hpp>
#include <boost/spirit/home/x3/operator/optional.hpp>

namespace mod::lib::Stereo::Read {

namespace {
namespace parser {

const auto fixation = x3::rule<struct fixation, char>{"fixation"}
/*               */ = x3::lit('!') >> x3::attr(true);
const auto virtualEdge = x3::rule<struct virtualEdge, char>{"virtualEdge"}
/*                  */ = x3::char_("a-zA-Z");
const auto neighbour = x3::rule<struct neighbour, ParsedEmbeddingEdge>{"neighbour"}
/*                */ = x3::int_ | virtualEdge;
const auto neighboursInner = x3::rule<struct neighboursInner, std::vector<ParsedEmbeddingEdge>>{
		"comma separated neighbour list"}
/*                      */ = -(neighbour % ',') >> ']';
const auto neighbours = x3::lit('[') > neighboursInner;
const auto geometry = x3::rule<struct geometry, std::string>{"geometry"}
/*               */ = x3::lexeme[x3::char_("a-zA-Z") >> *x3::char_("a-zA-Z0-9")];
const auto start = x3::rule<struct start, ParsedEmbedding>{"stereoEmbedding"}
/*            */ = !x3::eoi >> -geometry >> -neighbours >> -fixation >> x3::eoi;

} // namespace parser
} // namesapce

lib::IO::Result<ParsedEmbedding> parseEmbedding(const std::string &str) {
	try {
		ParsedEmbedding embedding;
		lib::IO::parse(str.begin(), str.end(), parser::start, embedding, false, x3::ascii::space);
		return embedding;
	} catch(const lib::IO::ParsingError &e) {
		return lib::IO::Result<>::Error(e.msg);
	}
}

} // namespace mod::lib::Stereo::Read

BOOST_FUSION_ADAPT_STRUCT(mod::lib::Stereo::Read::ParsedEmbedding,
                          (std::optional<std::string>, geometry)
		                          (std::optional<std::vector<mod::lib::Stereo::Read::ParsedEmbeddingEdge>>, edges)
		                          (std::optional<bool>, fixation)
)