#ifndef MOD_LIB_CHEM_MOLECULEUTIL_H
#define	MOD_LIB_CHEM_MOLECULEUTIL_H

#include <mod/Chem.h>

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

std::tuple<std::string, Charge, bool> extractChargeRadical(std::string label);
std::tuple<std::size_t, Charge, bool> extractChargeRadicalLen(const std::string &label);
AtomId atomIdFromSymbol(const std::string &label);
AtomId atomIdFromSymbol(const std::string &label, std::size_t len);
std::tuple<AtomId, Charge, bool> decodeVertexLabel(const std::string &label); // we don't return an AtomData, because a label explicitly can only hold atomId and charge
BondType decodeEdgeLabel(const std::string &label);
void markSpecialAtomsUsed(std::vector<bool> &used);
std::string symbolFromAtomId(AtomId atomId);
void appendSymbolFromAtomId(std::string &s, AtomId atomId);
char bondToChar(BondType bt);

template<typename Graph, typename AtomDataT, typename EdgeDataT>
bool isCollapsible(typename boost::graph_traits<Graph>::vertex_descriptor v, const Graph &g, const AtomDataT &atomData, const EdgeDataT &edgeData) {
	typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
	typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
	if(!isCleanHydrogen(atomData(v))) return false;
	if(out_degree(v, g) != 1) return false;
	Edge e = *out_edges(v, g).first;
	BondType eType = edgeData(e);
	if(eType != BondType::Single) return false;
	Vertex vAdj = target(e, g);
	if(!isCleanHydrogen(atomData(vAdj))) return true;
	// collapse the highest id vertex in H_2
	return get(boost::vertex_index_t(), g, v) > get(boost::vertex_index_t(), g, vAdj);
}

} // namespace Chem
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_CHEM_MOLECULEUTIL_H */
