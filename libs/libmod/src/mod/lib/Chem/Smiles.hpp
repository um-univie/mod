#ifndef MOD_LIB_CHEM_SMILES_HPP
#define MOD_LIB_CHEM_SMILES_HPP

#include <mod/lib/IO/Graph.hpp>

#include <string>

namespace mod {
struct AtomId;
} // namespace mod
namespace mod::lib::Graph {
struct PropMolecule;
struct PropString;
} // namespace mod::lib::Graph
namespace mod::lib::Chem {

std::string getSmiles(const lib::Graph::GraphType &g, const lib::Graph::PropMolecule &molState,
                      const std::vector<int> *ranks, bool withIds);
lib::IO::Result<std::vector<lib::IO::Graph::Read::Data>>
readSmiles(lib::IO::Warnings &warnings, const std::string &smiles, bool allowAbstract, SmilesClassPolicy classPolicy);
const std::vector<AtomId> &getSmilesOrganicSubset();
bool isInSmilesOrganicSubset(AtomId atomId);
void addImplicitHydrogens(lib::Graph::GraphType &g, lib::Graph::PropString &pString, lib::Graph::Vertex v,
                          AtomId atomId, std::function<void(lib::Graph::GraphType &, lib::Graph::PropString &,
                                                            lib::Graph::Vertex)> hydrogenAdder);

} // namespace mod::lib::Chem

#endif // MOD_LIB_CHEM_SMILES_HPP