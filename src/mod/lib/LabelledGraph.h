#ifndef MOD_LIB_LABELLEDGRAPH_H
#define MOD_LIB_LABELLEDGRAPH_H

#include <boost/concept_check.hpp>

namespace mod {
namespace lib {

template<typename G>
struct LabelledGraphTraits {
	using GraphType = typename G::GraphType;
	using PropStringType = typename G::PropStringType;
};

template<typename G>
struct LabelledGraphConcept {
	using GraphType = typename LabelledGraphTraits<G>::GraphType;
	using PropStringType = typename LabelledGraphTraits<G>::PropStringType;
public:

	BOOST_CONCEPT_USAGE(LabelledGraphConcept) {
		const G &gOuterConst = gOuter;

		const GraphType &gConst = get_graph(gOuterConst);
		(void) gConst;
		const PropStringType &pString = get_string(gOuterConst);
		(void) pString;
	}
private:
	G gOuter;
};

//template<typename G>
//struct MutableLabelledGraphConcept : LabelledGraphConcept<G>  {
//	using GraphType = typename G::GraphType;
//public:
//
//	BOOST_CONCEPT_USAGE(MutableLabelledGraphConcept) {		
//		GraphType &g = get_graph(gOuter);
//	}
//private:
//	G gOuter;
//};

} // namespace lib
} // namespace mod

#endif /* MOD_LIB_LABELLEDGRAPH_H */
