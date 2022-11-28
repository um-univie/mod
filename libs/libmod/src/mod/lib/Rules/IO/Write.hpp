#ifndef MOD_LIB_RULE_IO_WRITE_HPP
#define MOD_LIB_RULE_IO_WRITE_HPP

#include <mod/lib/IO/GraphWrite.hpp>
#include <mod/lib/Rules/LabelledRule.hpp>

namespace mod::lib::Rules {
struct Real;
} // namespace mod::lib::Rules
namespace mod::lib::Rules::Write {

using Options = IO::Graph::Write::Options;
using CombinedVertex = lib::DPO::CombinedRule::CombinedVertex;
using CombinedEdge = lib::DPO::CombinedRule::CombinedEdge;

struct BaseArgs {
	std::function<bool(CombinedVertex)> visible;
	std::function<std::string(CombinedVertex)> vColour;
	std::function<std::string(CombinedEdge)> eColour;
};

// returns the filename _with_ extension
void gml(const Real &r, bool withCoords, std::ostream &s);
std::string gml(const Real &r, bool withCoords);
// returns the filename without extension
std::string dotCombined(const Real &r);
std::string svgCombined(const Real &r);
std::string pdfCombined(const Real &r);
// returns the filename _without_ extension
std::string
dot(const Real &r, const Options &options); // does not handle labels correctly, is for coordinate generation
std::string coords(const Real &r, int idOffset, const Options &options,
                   std::function<bool(CombinedVertex)> disallowCollapse_);
std::pair<std::string, std::string>
tikz(const std::string &fileCoordsNoExt, const Real &r, unsigned int idOffset, const Options &options,
     const std::string &suffixL, const std::string &suffixK, const std::string &suffixR, const BaseArgs &args,
     std::function<bool(CombinedVertex)> disallowCollapse);
std::pair<std::string, std::string> tikz(const Real &r, unsigned int idOffset, const Options &options,
                                         const std::string &suffixL, const std::string &suffixK,
                                         const std::string &suffixR, const BaseArgs &args,
                                         std::function<bool(CombinedVertex)> disallowCollapse);
std::string pdf(const Real &r, const Options &options,
                const std::string &suffixL, const std::string &suffixK, const std::string &suffixR,
                const BaseArgs &args);
std::pair<std::string, std::string>
tikzTransitionState(const std::string &fileCoordsNoExt, const Real &r, unsigned int idOffset,
                    const Options &options,
                    const std::string &suffix, const BaseArgs &args);
std::pair<std::string, std::string>
tikzTransitionState(const Real &r, unsigned int idOffset, const Options &options,
                    const std::string &suffix, const BaseArgs &args);
std::string pdfTransitionState(const Real &r, const Options &options,
                               const std::string &suffix, const BaseArgs &args);
//std::string pdfCombined(const Real &r, const Options &options); // TODO
std::pair<std::string, std::string> summary(const Real &r, bool printCombined);
std::pair<std::string, std::string>
summary(const Real &r, const Options &first, const Options &second, bool printCombined);
void termState(const Real &r);

std::string stereoSummary(const Real &r, lib::DPO::CombinedRule::CombinedVertex v, Membership m,
                          const IO::Graph::Write::Options &options);

} // namespace mod::lib::Rules::Write

#endif // MOD_LIB_RULE_IO_WRITE_HPP