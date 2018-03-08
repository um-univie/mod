#ifndef MOD_LIB_DG_DUMP_H
#define MOD_LIB_DG_DUMP_H

#include <mod/graph/ForwardDecl.h>
#include <mod/rule/ForwardDecl.h>

#include <iosfwd>
#include <memory>
#include <vector>

namespace mod {
namespace lib {
namespace DG {
class NonHyper;
namespace Dump {

NonHyper *load(const std::vector<std::shared_ptr<graph::Graph> > &graphs, const std::vector<std::shared_ptr<rule::Rule> > &rules, std::istream &s, std::ostream &err);
void write(const NonHyper &dg, std::ostream &s);

} // namespace Dump
} // namespace DG
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_DG_DUMP_H */