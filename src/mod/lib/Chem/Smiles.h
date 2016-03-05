#ifndef MOD_LIB_CHEM_SMILES_H
#define	MOD_LIB_CHEM_SMILES_H

#include <mod/lib/Graph/Base.h>
#include <mod/lib/Graph/Properties/Molecule.h>

#include <string>

namespace mod {
struct AtomId;
namespace lib {
namespace Chem {

std::string getSmiles(const lib::Graph::GraphType &g, const lib::Graph::PropMolecule &molState);
std::pair<std::unique_ptr<lib::Graph::GraphType>, std::unique_ptr<lib::Graph::PropStringType> > readSmiles(const std::string &smiles, std::ostream &err);
const std::vector<AtomId> &getSmilesOrganicSubset();
bool isInSmilesOrganicSubset(AtomId atomId);
void addImplicitHydrogens(lib::Graph::GraphType &g, lib::Graph::PropStringType &pString, lib::Graph::Vertex v, AtomId atomId,
		std::function<void(lib::Graph::GraphType&, lib::Graph::PropStringType&, lib::Graph::Vertex)> hydrogenAdder);
} // namespace Chem
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_CHEM_SMILES_H */
