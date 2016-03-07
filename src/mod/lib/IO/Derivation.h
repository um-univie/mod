#ifndef MOD_LIB_IO_DERIVATION_H
#define	MOD_LIB_IO_DERIVATION_H

#include <mod/lib/DG/GraphDecl.h>

namespace mod {
namespace lib {
namespace DG {
class NonHyper;
} // namespace DG
namespace IO {
namespace Graph {
namespace Write {
struct Options;
} // namespace Write
} // namespace Graph
namespace Derivation {
namespace Write {
void summary(const lib::DG::NonHyper &dg, lib::DG::HyperVertex v, const IO::Graph::Write::Options &options, const std::string &matchColour);
} // namespace Write
} // namespace Derivation
} // namespace IO
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_IO_DERIVATION_H */