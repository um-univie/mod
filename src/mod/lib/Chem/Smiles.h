#ifndef MOD_LIB_CHEM_SMILES_H
#define MOD_LIB_CHEM_SMILES_H

#include <mod/lib/Graph/GraphDecl.h>
#include <mod/lib/IO/Graph.h>

#include <string>

namespace mod {
struct AtomId;
namespace lib {
namespace Graph {
struct PropMolecule;
struct PropString;
} // namespace Graph
namespace Chem {

std::string getSmiles(const lib::Graph::GraphType &g, const lib::Graph::PropMolecule &molState);
lib::IO::Graph::Read::Data readSmiles(const std::string &smiles, std::ostream &err);
const std::vector<AtomId> &getSmilesOrganicSubset();
bool isInSmilesOrganicSubset(AtomId atomId);
void addImplicitHydrogens(lib::Graph::GraphType &g, lib::Graph::PropString &pString, lib::Graph::Vertex v, AtomId atomId,
		std::function<void(lib::Graph::GraphType&, lib::Graph::PropString&, lib::Graph::Vertex)> hydrogenAdder);
} // namespace Chem
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_CHEM_SMILES_H */
