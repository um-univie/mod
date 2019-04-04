#ifndef MOD_LIB_IO_DERIVATION_H
#define MOD_LIB_IO_DERIVATION_H

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
std::vector<std::pair<std::string, std::string> >
summary(const lib::DG::NonHyper &dg, lib::DG::HyperVertex v, const IO::Graph::Write::Options &options, const std::string &nomatchColour, const std::string &matchColour);
void summaryTransitionState(const lib::DG::NonHyper &dg, lib::DG::HyperVertex v, const IO::Graph::Write::Options &options);
} // namespace Write
} // namespace Derivation
} // namespace IO
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_IO_DERIVATION_H */