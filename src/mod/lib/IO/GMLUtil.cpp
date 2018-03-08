#include <mod/lib/IO/GMLUtils.h>

#include <iostream>

namespace mod {
namespace lib {
namespace IO {
namespace GML {

 std::ostream &operator<<(std::ostream &s, const Edge &e) {
	 s << "edge [ source " << e.source << " target " << e.target;
	 if(e.label) s << " label \"" << *e.label << "\"";
	 if(e.stereo) s << " stereo \"" << *e.stereo << "\"";
	 return s << " ]";
 }

} // namespace GML
} // namespace IO
} // namespace lib
} // namespace mod