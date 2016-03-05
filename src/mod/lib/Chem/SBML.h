#ifndef MOD_LIB_CHEM_SBML_H
#define	MOD_LIB_CHEM_SBML_H

#include <string>

namespace mod {
namespace lib {
namespace DG {
class Matrix;
} // namespace DG

namespace Chem {
DG::Matrix *loadNetworkFromSBML(const std::string &fileName);
} // namespace Chem
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_CHEM_SBML_H */
