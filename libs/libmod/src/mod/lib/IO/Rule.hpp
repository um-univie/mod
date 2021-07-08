#ifndef MOD_LIB_IO_RULE_HPP
#define MOD_LIB_IO_RULE_HPP

#include <mod/Config.hpp>
#include <mod/lib/IO/Graph.hpp> // to make sure the write options are defined
#include <mod/lib/IO/Result.hpp>
#include <mod/lib/Rules/LabelledRule.hpp>

#include <optional>
#include <ostream>
#include <string>
#include <string_view>

namespace mod::lib::Rules {
struct Real;
} // namespace mod::lib::Rules
namespace mod::lib::IO::Rules {
namespace Read {

struct Data {
	std::optional<lib::Rules::LabelledRule> rule;
	std::optional<std::string> name;
	std::optional<LabelType> labelType;
	std::map<int, std::size_t> externalToInternalIds;
};

Result<Data> gml(lib::IO::Warnings &warnings, std::string_view input);

} // namespace Read
namespace Write {

using Options = IO::Graph::Write::Options;
using CoreVertex = lib::Rules::Vertex;
using CoreEdge = lib::Rules::Edge;

struct BaseArgs {
	std::function<bool(CoreVertex)> visible;
	std::function<std::string(CoreVertex)> vColour;
	std::function<std::string(CoreEdge)> eColour;
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
std::string coords(const lib::Rules::Real &r, unsigned int idOffset, const Options &options,
                   std::function<bool(CoreVertex)> disallowCollapse_);
std::pair<std::string, std::string>
tikz(const std::string &fileCoordsNoExt, const lib::Rules::Real &r, unsigned int idOffset, const Options &options,
     const std::string &suffixL, const std::string &suffixK, const std::string &suffixR, const BaseArgs &args,
     std::function<bool(CoreVertex)> disallowCollapse);
std::pair<std::string, std::string> tikz(const lib::Rules::Real &r, unsigned int idOffset, const Options &options,
                                         const std::string &suffixL, const std::string &suffixK,
                                         const std::string &suffixR, const BaseArgs &args,
                                         std::function<bool(CoreVertex)> disallowCollapse);
std::string pdf(const lib::Rules::Real &r, const Options &options,
                const std::string &suffixL, const std::string &suffixK, const std::string &suffixR,
                const BaseArgs &args);
std::pair<std::string, std::string>
tikzTransitionState(const std::string &fileCoordsNoExt, const lib::Rules::Real &r, unsigned int idOffset,
                    const Options &options,
                    const std::string &suffix, const BaseArgs &args);
std::pair<std::string, std::string>
tikzTransitionState(const lib::Rules::Real &r, unsigned int idOffset, const Options &options,
                    const std::string &suffix, const BaseArgs &args);
std::string pdfTransitionState(const lib::Rules::Real &r, const Options &options,
                               const std::string &suffix, const BaseArgs &args);
//std::string pdfCombined(const lib::Rules::Real &r, const Options &options); // TODO
std::pair<std::string, std::string> summary(const lib::Rules::Real &r, bool printCombined);
std::pair<std::string, std::string>
summary(const lib::Rules::Real &r, const Options &first, const Options &second, bool printCombined);
void termState(const lib::Rules::Real &r);
} // namespace Write
} // namespace mod::lib::IO::Rules

#endif // MOD_LIB_IO_RULE_HPP