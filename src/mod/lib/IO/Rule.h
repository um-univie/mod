#ifndef MOD_LIB_IO_RULE_H
#define MOD_LIB_IO_RULE_H

#include <mod/lib/IO/Graph.h> // to make sure the write options are defined
//#include <mod/lib/Graph/GraphDecl.h>
#include <mod/lib/Rules/LabelledRule.h>

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
};

Data gml(std::istream &s, std::ostream &err);
} // namespace Read
namespace Write {
using Options = IO::Graph::Write::Options;
using CoreVertex = lib::Rules::Vertex;
using CoreEdge = lib::Rules::Edge;
// returns the filename _with_ extension
void gml(const lib::Rules::Real &r, bool withCoords, std::ostream &s);
std::string gml(const lib::Rules::Real &r, bool withCoords);
// returns the filename without extension
std::string dotCombined(const lib::Rules::Real &r);
std::string svgCombined(const lib::Rules::Real &r);
std::string pdfCombined(const lib::Rules::Real &r);
// returns the filename _without_ extension
std::string dot(const lib::Rules::Real &r); // does not handle labels correctly, is for coordinate generation
std::string coords(const lib::Rules::Real &r, unsigned int idOffset, int rotation);
std::pair<std::string, std::string> tikz(const std::string &fileCoordsNoExt, const lib::Rules::Real &r, unsigned int idOffset, const Options &options,
		const std::string &suffixL, const std::string &suffixK, const std::string &suffixR,
		std::function<bool(CoreVertex) > visible, std::function<std::string(CoreVertex) > vColour, std::function<std::string(CoreEdge) > eColour,
		std::function<bool(CoreVertex) > disallowCollapse);
std::pair<std::string, std::string> tikz(const lib::Rules::Real &r, unsigned int idOffset, const Options &options,
		const std::string &suffixL, const std::string &suffixK, const std::string &suffixR,
		std::function<bool(CoreVertex) > visible, std::function<std::string(CoreVertex) > vColour, std::function<std::string(CoreEdge) > eColour,
		std::function<bool(CoreVertex) > disallowCollapse);
std::string pdf(const lib::Rules::Real &r, const Options &options,
		const std::string &suffixL, const std::string &suffixK, const std::string &suffixR,
		std::function<bool(CoreVertex) > visible, std::function<std::string(CoreVertex) > vColour, std::function<std::string(CoreEdge) > eColour);
std::pair<std::string, std::string> summary(const lib::Rules::Real &r);
std::pair<std::string, std::string> summary(const lib::Rules::Real &r, const Options &first, const Options &second);
} // namespace Write
} // namespace Rules
} // namespace IO
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_IO_RULE_H */

