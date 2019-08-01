#ifndef MOD_LIB_IO_RULE_H
#define MOD_LIB_IO_RULE_H

#include <mod/Config.hpp>
#include <mod/lib/IO/Graph.hpp> // to make sure the write options are defined
#include <mod/lib/Rules/LabelledRule.hpp>

#include <boost/optional.hpp>

#include <string>

namespace mod {
namespace lib {
namespace Rules {
struct Real;
} // namespace Rules
namespace IO {
namespace Rules {
namespace Read {

struct Data {
	boost::optional<lib::Rules::LabelledRule> rule;
	boost::optional<std::string> name;
	boost::optional<LabelType> labelType;
	std::map<int, std::size_t> externalToInternalIds;
};

Data gml(std::istream &s, std::ostream &err);
} // namespace Read
namespace Write {
using Options = IO::Graph::Write::Options;
using CoreVertex = lib::Rules::Vertex;
using CoreEdge = lib::Rules::Edge;

struct BaseArgs {
	std::function<bool(CoreVertex) > visible;
	std::function<std::string(CoreVertex) > vColour;
	std::function<std::string(CoreEdge) > eColour;
};

// returns the filename _with_ extension
void gml(const lib::Rules::Real &r, bool withCoords, std::ostream &s);
std::string gml(const lib::Rules::Real &r, bool withCoords);
// returns the filename without extension
std::string dotCombined(const lib::Rules::Real &r);
std::string svgCombined(const lib::Rules::Real &r);
std::string pdfCombined(const lib::Rules::Real &r);
// returns the filename _without_ extension
std::string dot(const lib::Rules::Real &r); // does not handle labels correctly, is for coordinate generation
std::string coords(const lib::Rules::Real &r, unsigned int idOffset, const Options &options, std::function<bool(CoreVertex) > disallowCollapse_);
std::pair<std::string, std::string> tikz(const std::string &fileCoordsNoExt, const lib::Rules::Real &r, unsigned int idOffset, const Options &options,
		const std::string &suffixL, const std::string &suffixK, const std::string &suffixR, const BaseArgs &args,
		std::function<bool(CoreVertex) > disallowCollapse);
std::pair<std::string, std::string> tikz(const lib::Rules::Real &r, unsigned int idOffset, const Options &options,
		const std::string &suffixL, const std::string &suffixK, const std::string &suffixR, const BaseArgs &args,
		std::function<bool(CoreVertex) > disallowCollapse);
std::string pdf(const lib::Rules::Real &r, const Options &options,
		const std::string &suffixL, const std::string &suffixK, const std::string &suffixR, const BaseArgs &args);
std::pair<std::string, std::string> tikzTransitionState(const std::string &fileCoordsNoExt, const lib::Rules::Real &r, unsigned int idOffset, const Options &options,
		const std::string &suffix, const BaseArgs &args);
std::pair<std::string, std::string> tikzTransitionState(const lib::Rules::Real &r, unsigned int idOffset, const Options &options,
		const std::string &suffix, const BaseArgs &args);
std::string pdfTransitionState(const lib::Rules::Real &r, const Options &options,
		const std::string &suffix, const BaseArgs &args);
//std::string pdfCombined(const lib::Rules::Real &r, const Options &options); // TODO
std::pair<std::string, std::string> summary(const lib::Rules::Real &r);
std::pair<std::string, std::string> summary(const lib::Rules::Real &r, const Options &first, const Options &second);
void termState(const lib::Rules::Real &r);
} // namespace Write
} // namespace Rules
} // namespace IO
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_IO_RULE_H */

