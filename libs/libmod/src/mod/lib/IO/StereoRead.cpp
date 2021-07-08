#include "Stereo.hpp"

#include <mod/lib/IO/ParsingUtil.hpp>

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
#include <boost/spirit/home/x3/operator/optional.hpp>

namespace mod::lib::IO::Stereo::Read {
namespace {
namespace parser {

const auto fixation = x3::rule<struct fixation, char>{"fixation"}
/*               */ = x3::lit('!') >> x3::attr(true);
const auto virtualEdge = x3::rule<struct virtualEdge, char>{"virtualEdge"}
/*                  */ = x3::char_("er"); // lone-pair or radical
const auto edge = x3::rule<struct edge, ParsedEmbeddingEdge>{"edge"}
/*           */ = x3::int_ | virtualEdge;
const auto edges = x3::rule<struct edges, std::vector<ParsedEmbeddingEdge> >{"edges"}
/*            */ = x3::lit('[') > -(edge % ',') >> ']';
const auto geometry = x3::rule<struct geometry, std::string>{"geometry"}
/*               */ = x3::lexeme[x3::char_("a-zA-Z") >> *x3::char_("a-zA-Z0-9")];
const auto start = x3::rule<struct start, ParsedEmbedding>{"stereoEmbedding"}
/*            */ = -geometry >> -edges >> -fixation >> x3::eoi;

} // namespace parser
} // namesapce

lib::IO::Result<ParsedEmbedding> parseEmbedding(const std::string &str) {
	using Iter = std::string::const_iterator;
	Iter start = str.begin();
	ParsedEmbedding embedding;
	bool res = x3::phrase_parse(start, str.end(), parser::start, x3::space, embedding);
	if(!res || start != str.end()) {
		std::string msg = "Error in stereo embedding at column ";
		msg += std::to_string(start - str.begin());
		msg += ".";
		if(start != str.end()) msg += " Expected end of input.";
		return lib::IO::Result<ParsedEmbedding>::Error(std::move(msg));
	}
	return embedding;
}

} // namespace mod::lib::IO::Stereo::Read

BOOST_FUSION_ADAPT_STRUCT(mod::lib::IO::Stereo::Read::ParsedEmbedding,
                          (std::optional<std::string>, geometry)
		                          (std::optional<std::vector<mod::lib::IO::Stereo::Read::ParsedEmbeddingEdge> >, edges)
		                          (std::optional<bool>, fixation)
)