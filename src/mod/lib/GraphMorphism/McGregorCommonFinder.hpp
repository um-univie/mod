#ifndef JLA_BOOST_GRAPH_MORPHISM_MCGREGORCOMMONFINDER_HPP
#define JLA_BOOST_GRAPH_MORPHISM_MCGREGORCOMMONFINDER_HPP

#include <jla_boost/graph/morphism/models/PropertyMap.hpp>

#include <boost/graph/mcgregor_common_subgraphs.hpp>

namespace mod {
namespace lib {
namespace GraphMorphism {
namespace detail {

template<typename Next>
struct CommonSubgraphWrapper {

	CommonSubgraphWrapper(Next next) : next(next) { }

	template<typename MapLR, typename MapRL>
	bool operator()(MapLR mLeftRight, MapRL mRightLeft, std::size_t) const {
		return next(mLeftRight, mRightLeft);
	}
private:
	Next next;
};

template<typename Next>
CommonSubgraphWrapper<Next> makeCommonSubgraphWrapper(Next &&next) {
	return CommonSubgraphWrapper<Next>(std::forward<Next>(next));
}

} // namespace detail

struct McGregorCommonFinder {

	McGregorCommonFinder(bool maximum, bool connected) : maximum(maximum), connected(connected) { }

	template<typename GraphDomain, typename GraphCodomain, typename MR, typename EdgePredicate, typename VertexPredicate,
	typename ArgsProviderDomain, typename ArgsProviderCodomain>
	bool operator()(const GraphDomain &gDomain, const GraphCodomain &gCodomain, MR mr, EdgePredicate edgePred, VertexPredicate vertexPred,
			ArgsProviderDomain argsDomain, ArgsProviderCodomain argsCodomain) {
		auto boostMr = detail::makeCommonSubgraphWrapper(jla_boost::GraphMorphism::makeMatchAsVertexMapWrapper(gDomain, gCodomain, mr));
		auto idxDomain = get(boost::vertex_index_t(), gDomain);
		auto idxCodomain = get(boost::vertex_index_t(), gCodomain);
		if(maximum) {
			// TOOD: this does actually not really work when part of the morphism definition is implemented in the MR.
			// e.g., see the rc test 'A -> AAA' with 'ABA -> A' in term mode.
			// Make a test with terms that include variables that actually doesn't unify, so we get a wrong maximum.
			boost::mcgregor_common_subgraphs_maximum(
					gDomain, gCodomain, idxDomain, idxCodomain,
					edgePred, vertexPred, connected, boostMr);
		} else {
			boost::mcgregor_common_subgraphs(
					gDomain, gCodomain, idxDomain, idxCodomain,
					edgePred, vertexPred, connected, boostMr);
		}
		return true;
	}

	template<typename GraphDomain, typename GraphCodomain, typename MR, typename EdgePredicate, typename VertexPredicate>
	bool operator()(const GraphDomain &gDomain, const GraphCodomain &gCodomain, MR mr, EdgePredicate edgePred, VertexPredicate vertexPred) {
		return (*this)(gDomain, gCodomain, mr, edgePred, vertexPred, DefaultFinderArgsProvider(), DefaultFinderArgsProvider());
	}
private:
	const bool maximum, connected;
};

} // namespace GraphMorphism
} // namespace lib
} // namespace mod

#endif /* JLA_BOOST_GRAPH_MORPHISM_MCGREGORCOMMONFINDER_HPP */