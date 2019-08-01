#ifndef MOD_LIB_CHEM_MOLECULEUTIL_H
#define MOD_LIB_CHEM_MOLECULEUTIL_H

#include <mod/Chem.hpp>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>

#include <string>
#include <vector>

namespace mod {

inline bool isCleanHydrogen(const AtomData &ad) {
	return ad == AtomData(AtomIds::H);
}

namespace lib {
namespace Chem {

std::tuple<std::string, Isotope, Charge, bool> extractIsotopeChargeRadical(const std::string &label);
std::tuple<std::string::const_iterator, std::string::const_iterator, Isotope, Charge, bool>
/*   */ extractIsotopeChargeRadicalLen(const std::string &label);
AtomId atomIdFromSymbol(const std::string &label);
AtomId atomIdFromSymbol(const std::string::const_iterator first, const std::string::const_iterator last);
std::tuple<AtomId, Isotope, Charge, bool> decodeVertexLabel(const std::string &label); // we don't return an AtomData, because it could potentially have extra stuff that we don't decode
BondType decodeEdgeLabel(const std::string &label);
void markSpecialAtomsUsed(std::vector<bool> &used);
std::string symbolFromAtomId(AtomId atomId);
void appendSymbolFromAtomId(std::string &s, AtomId atomId);
char bondToChar(BondType bt);
double exactMass(AtomId a, Isotope i); // implemented in Mass.cpp
constexpr double electronMass = 0.000548580;

template<typename Graph, typename AtomDataT, typename EdgeDataT, typename HasImportantStereo>
bool isCollapsible(const typename boost::graph_traits<Graph>::vertex_descriptor v,
						 const Graph &g,
						 const AtomDataT &atomData,
						 const EdgeDataT &edgeData,
						 const HasImportantStereo &hasImportantStereo) {
	if(!isCleanHydrogen(atomData(v))) return false;
	if(out_degree(v, g) != 1) return false;
	const auto e = *out_edges(v, g).first;
	const auto eType = edgeData(e);
	if(eType != BondType::Single) return false;
	const auto vAdj = target(e, g);
	if(hasImportantStereo(vAdj)) return false;
	if(!isCleanHydrogen(atomData(vAdj))) return true;
	// collapse the highest id vertex in H_2
	return get(boost::vertex_index_t(), g, v) > get(boost::vertex_index_t(), g, vAdj);
}

} // namespace Chem
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_CHEM_MOLECULEUTIL_H */
