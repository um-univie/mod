#ifndef MOD_DG_FORWARDDECL_H
#define MOD_DG_FORWARDDECL_H

namespace mod::dg {
struct Builder;
struct DG;
struct ExecuteResult;
struct PrintData;
struct Printer;
struct Strategy;
} // namespace mod::dg
namespace mod::lib {
namespace DG {
struct Hyper;
struct NonHyper;
struct NonHyperBuilder;
namespace Strategies {
struct Strategy;
} // namespace Strategies
} // namespace DG
namespace IO::DG::Write {
struct Data;
struct Printer;
} // namespace IO::DG::Write
} // namespace mod::lib

#endif /* MOD_DG_FORWARDDECL_H */