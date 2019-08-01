#ifndef MOD_LIB_LABELLEDGRAPH_H
#define MOD_LIB_LABELLEDGRAPH_H

#include <boost/concept_check.hpp>

namespace mod {
namespace lib {

template<typename G>
struct LabelledGraphTraits {
	using GraphType = typename G::GraphType;
	using PropStringType = typename G::PropStringType;
	using PropTermType = typename G::PropTermType;
};

template<typename G>
struct LabelledGraphConcept {
	using GraphType = typename LabelledGraphTraits<G>::GraphType;
	using PropStringType = typename LabelledGraphTraits<G>::PropStringType;
	using PropTermType = typename LabelledGraphTraits<G>::PropTermType;
public:

	BOOST_CONCEPT_USAGE(LabelledGraphConcept) {
		const G &gOuterConst = gOuter;

		const GraphType &gConst = get_graph(gOuterConst);
		(void) gConst;
		const PropStringType &pString = get_string(gOuterConst);
		(void) pString;
		const PropTermType &pTerm = get_term(gOuterConst);
		(void) pTerm;
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

struct IdentityPropertyHandler {

	template<typename VEProp, typename LabGraphDom, typename LabGraphCodom, typename F, typename ...Args>
	static auto fmap2(const VEProp &l, const VEProp &r, const LabGraphDom &gDom, const LabGraphCodom &gCodom, F &&f, Args&&... args) {
		return f(l, r, gDom, gCodom, std::forward<Args>(args)...);
	}

	template<typename Op, typename Val>
	static auto reduce(Op &&op, Val &&val) {
		return std::forward<Val>(val);
	}
};

} // namespace lib
} // namespace mod

#endif /* MOD_LIB_LABELLEDGRAPH_H */
