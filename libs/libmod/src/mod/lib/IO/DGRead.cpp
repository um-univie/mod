#include "DG.hpp"

#include <mod/graph/Graph.hpp>
#include <mod/rule/Rule.hpp>
#include <mod/lib/DG/Dump.hpp>
#include <mod/lib/DG/NonHyper.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/Graph/Properties/Stereo.hpp>
#include <mod/lib/Graph/Properties/String.hpp>
#include <mod/lib/IO/IO.hpp>
#include <mod/lib/IO/ParsingUtil.hpp>

#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/spirit/home/x3/auxiliary/attr.hpp>
#include <boost/spirit/home/x3/auxiliary/eps.hpp>
#include <boost/spirit/home/x3/char/char.hpp>
#include <boost/spirit/home/x3/char/char_class.hpp>
#include <boost/spirit/home/x3/directive/lexeme.hpp>
#include <boost/spirit/home/x3/numeric/uint.hpp>
#include <boost/spirit/home/x3/operator/alternative.hpp>
#include <boost/spirit/home/x3/operator/and_predicate.hpp>
#include <boost/spirit/home/x3/operator/difference.hpp>
#include <boost/spirit/home/x3/operator/kleene.hpp>
#include <boost/spirit/home/x3/operator/list.hpp>
#include <boost/spirit/home/x3/operator/plus.hpp>
#include <boost/spirit/home/x3/string/literal_string.hpp>

#include <string>
#include <unordered_map>

BOOST_FUSION_ADAPT_STRUCT(mod::lib::IO::DG::Read::AbstractDerivation,
                          (mod::lib::IO::DG::Read::AbstractDerivation::List, left)
		                          (bool, reversible)
		                          (mod::lib::IO::DG::Read::AbstractDerivation::List, right)
);

namespace mod {
namespace lib {
namespace IO {
namespace DG {
namespace Read {
namespace {
namespace parser {

const auto identifier = x3::lexeme[*(x3::char_ - x3::space)];
const auto coef = x3::lexeme[x3::uint_ >> &x3::space] | (x3::eps >> x3::attr(1u));
const auto element = x3::rule<struct element, std::pair<unsigned int, std::string> >("element")
/*              */ = coef >> identifier;
const auto side = element % '+';
const auto arrow = ("->" >> x3::attr(false)) | ("<=>" >> x3::attr(true));
const auto derivation = x3::rule<struct arrow, AbstractDerivation>("derivation")
/*                 */ = side >> arrow >> side;
const auto derivations = +derivation;

} // namespace parser
} // namespace

std::unique_ptr<lib::DG::NonHyper> dump(const std::vector<std::shared_ptr<graph::Graph> > &graphs,
                                        const std::vector<std::shared_ptr<rule::Rule> > &rules,
                                        const std::string &file,
                                        std::ostream &err) {
	return lib::DG::Dump::load(graphs, rules, file, err);
}

boost::optional<std::vector<AbstractDerivation>> abstract(const std::string &s, std::ostream &err) {
	auto iterStart = s.begin(), iterEnd = s.end();
	std::vector<AbstractDerivation> derivations;
	const bool res = lib::IO::parse(iterStart, iterEnd, parser::derivations, derivations, err, x3::space);
	if(!res) return {};
	else return derivations;
}

} // namespace Read
} // namespace DG
} // namespace IO
} // namespace lib
} // namespace mod
