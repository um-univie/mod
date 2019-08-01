#ifndef JLA_BOOST_GRAPH_MORPHISM_MODELS_REINTERPRETED_HPP
#define JLA_BOOST_GRAPH_MORPHISM_MODELS_REINTERPRETED_HPP

#include <boost/graph/graph_traits.hpp>

namespace jla_boost {
namespace GraphMorphism {

template<typename M>
struct VertexMapTraits;

template<typename VertexMap, typename GraphDomT, typename GraphCodomT>
struct ReinterpretedVertexMap {
	using GraphDom = GraphDomT;
	using GraphCodom = GraphCodomT;
	using Storable = typename VertexMapTraits<VertexMap>::Storable;
public:
	using GraphDomOrig = typename VertexMapTraits<VertexMap>::GraphDom;
	using GraphCodomOrig = typename VertexMapTraits<VertexMap>::GraphCodom;
public:

	ReinterpretedVertexMap(VertexMap m, const GraphDomOrig &gDomm, const GraphCodomOrig &gCodom)
	: m(std::move(m)), gDom(gDomm), gCodom(gCodom) { }

	friend typename boost::graph_traits<GraphCodom>::vertex_descriptor
	get(const ReinterpretedVertexMap &m, const GraphDom&, const GraphCodom&,
			typename boost::graph_traits<GraphDom>::vertex_descriptor vDomU) {
		return get(m.m, m.gDom, m.gCodom, vDomU);
	}

	friend typename boost::graph_traits<GraphDom>::vertex_descriptor
	get_inverse(const ReinterpretedVertexMap &m, const GraphDom&, const GraphCodom&,
			typename boost::graph_traits<GraphCodomOrig>::vertex_descriptor vCodomU) {
		return get_inverse(m.m, m.gDom, m.gCodom, vCodomU);
	}
private:
	VertexMap m;
	const GraphDomOrig &gDom;
	const GraphCodomOrig &gCodom;
};

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_MODELS_REINTERPRETED_HPP */