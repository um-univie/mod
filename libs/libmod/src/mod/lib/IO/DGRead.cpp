#include "DG.hpp"

#include <iomanip>
//#define BOOST_SPIRIT_X3_DEBUG

#include <mod/graph/Graph.hpp>
#include <mod/rule/Rule.hpp>
#include <mod/lib/DG/Dump.hpp>
#include <mod/lib/DG/NonHyperBuilder.hpp>
#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/Graph/Properties/Stereo.hpp>
#include <mod/lib/Graph/Properties/String.hpp>
#include <mod/lib/IO/Config.hpp>
#include <mod/lib/IO/JsonUtils.hpp>
#include <mod/lib/IO/ParsingUtil.hpp>

#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/spirit/home/x3/auxiliary/attr.hpp>
#include <boost/spirit/home/x3/auxiliary/eps.hpp>
#include <boost/spirit/home/x3/char/char.hpp>
#include <boost/spirit/home/x3/char/char_class.hpp>
#include <boost/spirit/home/x3/directive/lexeme.hpp>
#include <boost/spirit/home/x3/numeric/uint.hpp>
#include <boost/spirit/home/x3/operator/alternative.hpp>
#include <boost/spirit/home/x3/operator/and_predicate.hpp>
#include <boost/spirit/home/x3/operator/difference.hpp>
#include <boost/spirit/home/x3/operator/list.hpp>
#include <boost/spirit/home/x3/operator/plus.hpp>
#include <boost/spirit/home/x3/string/literal_string.hpp>

#include <string>

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

const auto identifier = x3::rule<struct identifier, std::string>("identifier")
		                        = x3::lexeme[+(x3::ascii::char_ - x3::ascii::space)];
const auto coef = x3::rule<struct coef, unsigned int>("coefficient")
		                  = x3::lexeme[x3::uint_ >> &x3::ascii::space] | (x3::eps >> x3::attr(1u));
const auto element = x3::rule<struct element, std::pair<unsigned int, std::string> >("element")
		                     = coef >> identifier;
const auto side = x3::rule<struct side, std::vector<std::pair<unsigned int, std::string>>>("side")
		                  = element % '+';
const auto arrow = x3::rule<struct arrow, bool>("-> or <=>")
		                   = ("->" >> x3::attr(false)) | ("<=>" >> x3::attr(true));
const auto derivation = x3::rule<struct arrow, AbstractDerivation>("derivation")
		                        = side > arrow > side;
const auto derivations = +derivation;

} // namespace parser
} // namespace

boost::optional<nlohmann::json> loadDump(const std::string &file, std::ostream &err) {
	boost::iostreams::mapped_file_source ifs(file);
	std::vector<std::uint8_t> data(ifs.begin(), ifs.end());
	auto jOpt = IO::readJson(data, err);
	if(!jOpt) return {};
	static const nlohmann::json schema = R"({
		"$schema": "http://json-schema.org/draft-07/schema#",
		"type": "object",
		"properties": {
			"version": {"type": "integer"},
			"labelSettings": {"type": "object"},
			"vertices": {"type": "array", "items": {
				"type": "array",
				"items": [
					{"type": "integer", "descirption": "id"},
					{"type": "string",  "description": "graphName"},
					{"type": "string",  "description": "graphGML"}
				]
			}},
			"rules": {"type": "array", "items": {"type": "string"}},
			"edges": {"type": "array", "items": {
				"type": "array",
				"items": [
					{"type": "integer",                             "description": "id"},
					{"type": "array", "items": {"type": "integer"}, "description": "sources"},
					{"type": "array", "items": {"type": "integer"}, "description": "targets"}
				]
			}}
		},
		"required": ["version", "labelSettings", "vertices"]
	})"_json;
	// TODO: the labelSettings part should be validated as well
	static const nlohmann::json_schema::json_validator validator(schema);
	auto &j = *jOpt;
	if(!IO::validateJson(j, validator, err, "Data does not conform to schema:"))
		return {};

	if(j["version"].get<int>() != 3) {
		err << "Unknown DG dump version.";
		return {};
	}

	return jOpt;
}

std::unique_ptr<lib::DG::NonHyper> dump(const std::vector<std::shared_ptr<graph::Graph>> &graphDatabase,
                                        const std::vector<std::shared_ptr<rule::Rule>> &ruleDatabase,
                                        const std::string &file,
                                        IsomorphismPolicy graphPolicy,
                                        std::ostream &err, int verbosity) {
	boost::iostreams::mapped_file_source ifs(file);
	if(ifs.size() > 8 && std::string(ifs.begin(), ifs.begin() + 8) == "version:") {
		ifs.close();
		return lib::DG::Dump::load(graphDatabase, ruleDatabase, file, err);
	}
	ifs.close();
	auto jOpt = loadDump(file, err);
	if(!jOpt) return {};
	auto &j = *jOpt;

	LabelSettings labelSettings = from_json(j["labelSettings"]);
	auto dgInternal = std::make_unique<lib::DG::NonHyperBuilder>(labelSettings, graphDatabase, graphPolicy);
	{ // construction
		auto b = dgInternal->build();
		auto res = b.trustLoadDump(std::move(j), ruleDatabase, err, verbosity);
		if(!res) return {};
	}
	return std::unique_ptr<lib::DG::NonHyper>(dgInternal.release());
}

boost::optional<std::vector<AbstractDerivation>> abstract(const std::string &s, std::ostream &err) {
	auto iterStart = s.begin(), iterEnd = s.end();
	std::vector<AbstractDerivation> derivations;
	const bool res = lib::IO::parse(iterStart, iterEnd, parser::derivations, derivations, err, x3::ascii::space);
	if(!res) return {};
	else return derivations;
}

} // namespace Read
} // namespace DG
} // namespace IO
} // namespace lib
} // namespace mod
