#ifndef MOD_LIB_GRAPHMORPHISM_DEBUGUTIL_H
#define	MOD_LIB_GRAPHMORPHISM_DEBUGUTIL_H

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

namespace mod {
namespace lib {
namespace GraphMorphism {
namespace Callback {

//------------------------------------------------------------------------------
// Print
//------------------------------------------------------------------------------

template<typename GraphSmall, typename GraphLarge, typename MR = Continue>
struct Print {

	Print(std::ostream &s, const GraphSmall &gSmall, const GraphLarge &gLarge, const MR &mr = Continue()) : s(s), gSmall(gSmall), gLarge(gLarge), mr(mr) { }

	template<typename MapSmallToLarge, typename MapLargeToSmall>
	bool operator()(MapSmallToLarge mSmall2Large, MapLargeToSmall mLarge2Small) const {
		s << "Found match!" << std::endl;
		typedef typename boost::graph_traits<GraphSmall>::vertex_descriptor VertexSmall;
		typedef typename boost::graph_traits<GraphSmall>::edge_descriptor EdgeSmall;
		typedef typename boost::graph_traits<GraphLarge>::vertex_descriptor VertexLarge;
		typedef typename boost::graph_traits<GraphLarge>::edge_descriptor EdgeLarge;

		for(VertexSmall vSmall : asRange(vertices(gSmall))) {
			VertexLarge vLarge = mSmall2Large[vSmall];
			s << "\t" << get(boost::vertex_index_t(), gSmall, vSmall)
					<< "\t" << get(boost::vertex_name_t(), gSmall, vSmall)
					<< "\t=> "
					<< get(boost::vertex_index_t(), gLarge, vLarge)
					<< "\t" << get(boost::vertex_name_t(), gLarge, vLarge)
					<< std::endl;

			for(EdgeSmall eSmall : asRange(out_edges(vSmall, gSmall))) {
				VertexSmall vSmallTarget = target(eSmall, gSmall);
				if(get(boost::vertex_index_t(), gSmall, vSmall) > get(boost::vertex_index_t(), gLarge, vLarge)) continue;

				for(EdgeLarge eLarge : asRange(out_edges(vLarge, gLarge))) {
					VertexLarge vLargeTarget = target(eLarge, gLarge);
					if(mSmall2Large[vSmallTarget] != vLargeTarget) continue;
					s << "\t\t(" << get(boost::vertex_index_t(), gSmall, vSmallTarget)
							<< ", " << get(boost::vertex_name_t(), gSmall, vSmallTarget) << ") "
							<< get(boost::edge_name_t(), gSmall, eSmall)
							<< "\t=> "
							<< get(boost::edge_name_t(), gLarge, eLarge)
							<< "\t(" << get(boost::vertex_index_t(), gLarge, vLargeTarget)
							<< ", " << get(boost::vertex_name_t(), gLarge, vLargeTarget) << ")" << std::endl;
				}
			}
		}
		return mr(mSmall2Large, mLarge2Small);
	}
private:
	std::ostream &s;
	const GraphSmall &gSmall;
	const GraphLarge &gLarge;
	const MR &mr;
};

template<typename GraphSmall, typename GraphLarge, typename MR = Continue>
Print<GraphSmall, GraphLarge, MR> makePrint(std::ostream &s, const GraphSmall &gSmall, const GraphLarge &gLarge, const MR &mr = Continue()) {
	return Print<GraphSmall, GraphLarge, MR > (s, gSmall, gLarge, mr);
}

//------------------------------------------------------------------------------
// PrintShort
//------------------------------------------------------------------------------

template<typename MR = Continue>
struct PrintShort {

	PrintShort(std::ostream &s, const MR &mr = Continue()) : s(s), mr(mr) { }

	template<typename MapSmallToLarge, typename MapLargeToSmall>
	bool operator()(MapSmallToLarge mSmall2Large, MapLargeToSmall mLarge2Small) const {
		s << "Found match!" << std::endl;
		return mr(mSmall2Large, mLarge2Small);
	}
private:
	std::ostream &s;
	const MR &mr;
};

template<typename MR = Continue>
PrintShort<MR> makePrintShort(std::ostream &s, const MR &mr = Continue()) {
	return PrintShort<MR > (s, mr);
}

} // namespace Callback
} // namespace GraphMorphism
} // namespace lib
} // namespace mod

#endif	/* MOD_LIB_GRAPHMORPHISM_DEBUGUTIL_H */