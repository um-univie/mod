#include "GML.hpp"

#include <ostream>

namespace mod::lib::IO::GML {

std::ostream &operator<<(std::ostream &s, const Edge &e) {
	s << "edge [ source " << e.source << " target " << e.target;
	if(e.label) s << " label \"" << *e.label << "\"";
	if(e.stereo) s << " stereo \"" << *e.stereo << "\"";
	return s << " ]";
}

} // namespace mod::lib::IO::GML