#ifndef MOD_LIB_IO_RULE_H
#define MOD_LIB_IO_RULE_H

#include <mod/lib/IO/Graph.h> // to make sure the write options are defined
#include <mod/lib/Rule/Real.h>

#include <boost/optional.hpp>

#include <string>

namespace mod {
namespace lib {
namespace IO {
namespace Rule {
namespace Read {

struct Data {
	boost::optional<lib::Rule::LabelledRule> rule;
	boost::optional<std::string> name;
};

Data gml(std::istream &s, std::ostream &err);
} // namespace Read
namespace Write {
typedef IO::Graph::Write::Options Options;
using CoreVertex = lib::Rule::Vertex;
using CoreEdge = lib::Rule::Edge;
// returns the filename _with_ extension
void gml(const lib::Rule::Real &r, bool withCoords, std::ostream &s);
std::string gml(const lib::Rule::Real &r, bool withCoords);
// returns the filename without extension
std::string dotCombined(const lib::Rule::Real &r);
std::string svgCombined(const lib::Rule::Real &r);
std::string pdfCombined(const lib::Rule::Real &r);
// returns the filename _without_ extension
std::string dot(const lib::Rule::Real &r); // does not handle labels correctly, is for coordinate generation
std::string coords(const lib::Rule::Real &r, unsigned int idOffset);
std::pair<std::string, std::string> tikz(const std::string &fileCoordsNoExt, const lib::Rule::Real &r, unsigned int idOffset, const Options &options,
		const std::string &suffixL, const std::string &suffixK, const std::string &suffixR,
		std::function<bool(CoreVertex) > visible, std::function<std::string(CoreVertex) > vColour, std::function<std::string(CoreEdge) > eColour,
		std::function<bool(CoreVertex)> disallowCollapse);
std::pair<std::string, std::string> tikz(const lib::Rule::Real &r, unsigned int idOffset, const Options &options,
		const std::string &suffixL, const std::string &suffixK, const std::string &suffixR,
		std::function<bool(CoreVertex) > visible, std::function<std::string(CoreVertex) > vColour, std::function<std::string(CoreEdge) > eColour,
		std::function<bool(CoreVertex)> disallowCollapse);
std::string pdf(const lib::Rule::Real &r, const Options &options,
		const std::string &suffixL, const std::string &suffixK, const std::string &suffixR,
		std::function<bool(CoreVertex) > visible, std::function<std::string(CoreVertex) > vColour, std::function<std::string(CoreEdge) > eColour);
//std::string pdfCombined(const lib::Rule::Real &r, const Options &options); // TODO
std::pair<std::string, std::string> summary(const lib::Rule::Real &r);
std::pair<std::string, std::string> summary(const lib::Rule::Real &r, const Options &first, const Options &second);
} // namespace Write
} // namespace Rule
} // namespace IO
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_IO_RULE_H */

