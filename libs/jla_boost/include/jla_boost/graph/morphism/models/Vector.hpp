#ifndef JLA_BOOST_GRAPH_MORPHISM_MODELS_VECTOR_HPP
#define JLA_BOOST_GRAPH_MORPHISM_MODELS_VECTOR_HPP

#include <jla_boost/graph/morphism/Concepts.hpp>

#include <vector>

namespace jla_boost::GraphMorphism {

// VertexMap
// =============================================================================

template<typename GraphDomT, typename GraphCodomT>
struct VectorVertexMap {
	using GraphDom = GraphDomT;
	using GraphCodom = GraphCodomT;
	using Storable = std::true_type;

	template<typename GraphDomU, typename GraphCodomU>
	static auto reinterpret(VectorVertexMap &&m, const GraphDom &gDom, const GraphCodom &gCodom,
	                        const GraphDomU &gDomReinterpreted, const GraphCodomU &gCodomReinterpreted) {
		// this of course assumes that the vertex_descriptors represent exactly the same on the reinterpreted graphs
		return VectorVertexMap<GraphDomU, GraphCodomU>(std::move(m.data), gDomReinterpreted, gCodomReinterpreted);
	}
public:
	VectorVertexMap(const GraphDom &gDom, const GraphCodom &gCodom)
			: data(num_vertices(gDom), boost::graph_traits<GraphCodom>::null_vertex()) {}

	template<typename VertexMap>
	VectorVertexMap(VertexMap &&m, const GraphDom &gDom, const GraphCodom &gCodom)
			: VectorVertexMap(gDom, gCodom) {
		BOOST_CONCEPT_ASSERT((VertexMapConcept<VertexMap>));
		for(auto v: asRange(vertices(gDom))) {
			auto vId = get(boost::vertex_index_t(), gDom, v);
			data[vId] = get(m, gDom, gCodom, v);
		}
	}

	std::size_t size() const {
		return data.size();
	}
private:
	template<typename GraphDomU, typename GraphCodomU>
	friend
	class VectorVertexMap;
	template<typename M>
	friend
	class VertexMapTraits;

	VectorVertexMap(std::vector<typename boost::graph_traits<GraphCodom>::vertex_descriptor> &&data,
	                const GraphDom &gDom, const GraphCodom &gCodom)
			: data(std::move(data)) {
		assert(this->data.size() == num_vertices(gDom));
	}
private:
	std::vector<typename boost::graph_traits<GraphCodom>::vertex_descriptor> data;
public:
	friend typename boost::graph_traits<GraphCodom>::vertex_descriptor
	get(const VectorVertexMap &m, const GraphDom &gDom, const GraphCodom &gCodom,
	    typename boost::graph_traits<GraphDom>::vertex_descriptor v) {
		auto vId = get(boost::vertex_index_t(), gDom, v);
		assert(vId < m.data.size());
		return m.data[vId];
	}

	friend void put(VectorVertexMap &m, const GraphDom &gDom, const GraphCodom &gCodom,
	                typename boost::graph_traits<GraphDom>::vertex_descriptor vDom,
	                typename boost::graph_traits<GraphCodom>::vertex_descriptor vCodom) {
		auto vId = get(boost::vertex_index_t(), gDom, vDom);
		assert(vId < m.data.size());
		m.data[vId] = vCodom;
	}

	friend void syncSize(VectorVertexMap &m, const GraphDom &gDom, const GraphCodom &gCodom) {
		m.data.resize(num_vertices(gDom), boost::graph_traits<GraphCodom>::null_vertex());
	}
};

struct ToVectorVertexMap {
	template<typename VertexMap>
	auto operator()(VertexMap &&m,
	                const typename VertexMapTraits<VertexMap>::GraphDom &gDom,
	                const typename VertexMapTraits<VertexMap>::GraphCodom &gCodom) const {
		using Map = VectorVertexMap<typename VertexMapTraits<VertexMap>::GraphDom, typename VertexMapTraits<VertexMap>::GraphCodom>;
		return Map(std::forward<VertexMap>(m), gDom, gCodom);
	}
};

// GraphMap
// =============================================================================

template<typename GraphDomT, typename GraphCodomT>
struct VectorGraphMap : VectorVertexMap<GraphDomT, GraphCodomT> {
	using Base = VectorVertexMap<GraphDomT, GraphCodomT>;
	using GraphDom = GraphDomT;
	using GraphCodom = GraphCodomT;
	using Storable = std::true_type;

//	template<typename GraphDomU, typename GraphCodomU>
//	static auto reinterpret(VectorGraphMap &&m, const GraphDom &gDom, const GraphCodom &gCodom,
//	                        const GraphDomU &gDomReinterpreted, const GraphCodomU &gCodomReinterpreted) {
//		// this of course assumes that the descriptors represent exactly the same on the reinterpreted graphs
//		return VectorGraphMap<GraphDomU, GraphCodomU>(std::move(m.data), gDomReinterpreted, gCodomReinterpreted);
//	}
public:
	VectorGraphMap(const GraphDom &gDom, const GraphCodom &gCodom)
			: Base(gDom, gCodom), edgeData(num_edges(gDom)) {}

//	template<typename VertexMap>
//	VectorGraphMap(VertexMap &&m, const GraphDom &gDom, const GraphCodom &gCodom)
//			: VectorGraphMap(gDom, gCodom) {
//		BOOST_CONCEPT_ASSERT((VertexMapConcept<VertexMap>));
//		for(const auto e: asRange(edges(gDom))) {
//			const auto eId = get(boost::edge_index_t(), gDom, e);
//			edgeData[eId] = get(m, gDom, gCodom, e);
//		}
//	}

	std::size_t size() const {
		return edgeData.size();
	}
private:
	template<typename GraphDomU, typename GraphCodomU>
	friend
	class VectorGraphMap;
	template<typename M>
	friend
	class VertexMapTraits;

//	VectorGraphMap(std::vector<typename boost::graph_traits<GraphCodom>::edge_descriptor> &&edgeData,
//	               const GraphDom &gDom, const GraphCodom &gCodom)
//			: edgeData(std::move(edgeData)) {
//		assert(this->data.size() == num_vertices(gDom));
//	}
private:
	std::vector<typename boost::graph_traits<GraphCodom>::edge_descriptor> edgeData;
public:
	friend typename boost::graph_traits<GraphCodom>::edge_descriptor
	get(const VectorGraphMap &m, const GraphDom &gDom, const GraphCodom &gCodom,
	    typename boost::graph_traits<GraphDom>::edge_descriptor e) {
		const auto eId = get(boost::edge_index_t(), gDom, e);
		assert(eId < m.edgeData.size());
		return m.edgeData[eId];
	}

	friend void put(VectorGraphMap &m, const GraphDom &gDom, const GraphCodom &gCodom,
	                typename boost::graph_traits<GraphDom>::edge_descriptor eDom,
	                typename boost::graph_traits<GraphCodom>::edge_descriptor eCodom) {
		const auto eId = get(boost::edge_index_t(), gDom, eDom);
		assert(eId < m.edgeData.size());
		m.edgeData[eId] = eCodom;
	}

	friend void syncSize(VectorGraphMap &m, const GraphDom &gDom, const GraphCodom &gCodom) {
		syncSize(static_cast<Base&>(m), gDom, gCodom);
		m.edgeData.resize(num_edges(gDom));
	}
};

struct ToVectorGraphMap {
	template<typename GraphMap>
	auto operator()(GraphMap &&m,
	                const typename VertexMapTraits<GraphMap>::GraphDom &gDom,
	                const typename VertexMapTraits<GraphMap>::GraphCodom &gCodom) const {
		using Map = VectorGraphMap<typename VertexMapTraits<GraphMap>::GraphDom, typename VertexMapTraits<GraphMap>::GraphCodom>;
		return Map(std::forward<GraphMap>(m), gDom, gCodom);
	}
};

} // namespace jla_boost::GraphMorphism

#endif // JLA_BOOST_GRAPH_MORPHISM_MODELS_VECTOR_HPP