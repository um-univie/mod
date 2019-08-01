#ifndef MOD_LIB_DG_DUMP_H
#define MOD_LIB_DG_DUMP_H

#include <mod/graph/ForwardDecl.hpp>
#include <mod/rule/ForwardDecl.hpp>

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace mod {
namespace lib {
namespace DG {
class NonHyper;
namespace Dump {

std::unique_ptr<NonHyper> load(const std::vector<std::shared_ptr<graph::Graph> > &graphs,
										 const std::vector<std::shared_ptr<rule::Rule> > &rules,
										 const std::string &file,
										 std::ostream &err);
void write(const NonHyper &dg, std::ostream &s);

} // namespace Dump
} // namespace DG
} // namespace lib
} // namespace mod

#endif   /* MOD_LIB_DG_DUMP_H */