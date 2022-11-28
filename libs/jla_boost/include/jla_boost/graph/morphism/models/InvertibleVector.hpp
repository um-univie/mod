#ifndef JLA_BOOST_GRAPH_MORPHISM_MODELS_INVERTIBLE_VECTOR_HPP
#define JLA_BOOST_GRAPH_MORPHISM_MODELS_INVERTIBLE_VECTOR_HPP

#include <jla_boost/graph/morphism/Concepts.hpp>

#include <vector>

namespace jla_boost::GraphMorphism {

// InvertibleVertexMap
// =============================================================================

template<typename GraphDomT, typename GraphCodomT>
struct InvertibleVectorVertexMap {
	using GraphDom = GraphDomT;
	using GraphCodom = GraphCodomT;
	using Storable = std::true_type;

	template<typename GraphDomU, typename GraphCodomU>
	static auto
	reinterpret(InvertibleVectorVertexMap<GraphDom, GraphCodom> &&m, const GraphDom &gDom, const GraphCodom &gCodom,
	            const GraphDomU &gDomReinterpreted, const GraphCodomU &gCodomReinterpreted) {
		// this of course assumes that the vertex_descriptors represent exactly the same on the reinterpreted graphs
		return InvertibleVectorVertexMap<GraphDomU, GraphCodomU>(std::move(m.forward), std::move(m.backward),
		                                                         gDomReinterpreted, gCodomReinterpreted);
	}

	InvertibleVectorVertexMap(const GraphDom &gDom, const GraphCodom &gCodom)
			: forward(num_vertices(gDom), boost::graph_traits<GraphCodom>::null_vertex()),
			  backward(num_vertices(gCodom), boost::graph_traits<GraphDom>::null_vertex()) {}

	template<typename VertexMap>
	InvertibleVectorVertexMap(const VertexMap &m, const GraphDom &gDom, const GraphCodom &gCodom)
			: InvertibleVectorVertexMap(gDom, gCodom) {
		BOOST_CONCEPT_ASSERT((InvertibleVertexMapConcept<VertexMap>));
		for(auto vDom: asRange(vertices(gDom))) {
			auto vId = get(boost::vertex_index_t(), gDom, vDom);
			forward[vId] = get(m, gDom, gCodom, vDom);
		}
		for(auto vCodom: asRange(vertices(gCodom))) {
			auto vId = get(boost::vertex_index_t(), gCodom, vCodom);
			backward[vId] = get_inverse(m, gDom, gCodom, vCodom);
		}
		{ // just asserts
			for(auto vDom: asRange(vertices(gDom))) {
				auto vCodom = get(*this, gDom, gCodom, vDom);
				if(vCodom != boost::graph_traits<GraphCodom>::null_vertex())
					assert(get_inverse(*this, gDom, gCodom, vCodom) == vDom);
			}
			for(auto vCodom: asRange(vertices(gCodom))) {
				auto vDom = get_inverse(*this, gDom, gCodom, vCodom);
				if(vDom != boost::graph_traits<GraphDom>::null_vertex())
					assert(get(*this, gDom, gCodom, vDom) == vCodom);
			}
		}
	}
private:
	template<typename GraphDomU, typename GraphCodomU>
	friend
	class InvertibleVectorVertexMap;

	InvertibleVectorVertexMap(
			std::vector<typename boost::graph_traits<GraphCodom>::vertex_descriptor> &&forward,
			std::vector<typename boost::graph_traits<GraphDom>::vertex_descriptor> &&backward,
			const GraphDom &gDom, const GraphCodom &gCodom)
			: forward(std::move(forward)), backward(std::move(backward)) {
		assert(this->forward.size() == num_vertices(gDom));
		assert(this->backward.size() == num_vertices(gCodom));
	}
private:
	std::vector<typename boost::graph_traits<GraphCodom>::vertex_descriptor> forward;
	std::vector<typename boost::graph_traits<GraphDom>::vertex_descriptor> backward;
public:
	friend typename boost::graph_traits<GraphCodom>::vertex_descriptor
	get(const InvertibleVectorVertexMap &m, const GraphDom &gDom, const GraphCodom &gCodom,
	    typename boost::graph_traits<GraphDom>::vertex_descriptor v) {
		assert(v != boost::graph_traits<GraphDom>::null_vertex());
		auto vId = get(boost::vertex_index_t(), gDom, v);
		assert(vId < m.forward.size());
		return m.forward[vId];
	}

	friend typename boost::graph_traits<GraphDom>::vertex_descriptor
	get_inverse(const InvertibleVectorVertexMap &m, const GraphDom &gDom, const GraphCodom &gCodom,
	            typename boost::graph_traits<GraphCodom>::vertex_descriptor v) {
		assert(v != boost::graph_traits<GraphCodom>::null_vertex());
		auto vId = get(boost::vertex_index_t(), gCodom, v);
		assert(vId < m.backward.size());
		return m.backward[vId];
	}

	friend void put(InvertibleVectorVertexMap &m, const GraphDom &gDom, const GraphCodom &gCodom,
	                typename boost::graph_traits<GraphDom>::vertex_descriptor vDom,
	                typename boost::graph_traits<GraphCodom>::vertex_descriptor vCodom) {
		assert(vDom != boost::graph_traits<GraphDom>::null_vertex());
		auto vIdDom = get(boost::vertex_index_t(), gDom, vDom);
		if(vCodom == boost::graph_traits<GraphCodom>::null_vertex()) {
			// we need to reset the inverse before we forget it,
			auto vCodomOld = m.forward[vIdDom];
			if(vCodomOld != boost::graph_traits<GraphCodom>::null_vertex()) {
				m.forward[vIdDom] = boost::graph_traits<GraphCodom>::null_vertex();
				auto vIdCodomOld = get(boost::vertex_index_t(), gCodom, vCodomOld);
				m.backward[vIdCodomOld] = boost::graph_traits<GraphDom>::null_vertex();
			}
		} else {
			auto vIdCodom = get(boost::vertex_index_t(), gCodom, vCodom);
			assert(vIdDom < m.forward.size());
			assert(vIdCodom < m.backward.size());
			m.forward[vIdDom] = vCodom;
			m.backward[vIdCodom] = vDom;
		}
	}
public:
	friend void syncSize(InvertibleVectorVertexMap &m, const GraphDom &gDom, const GraphCodom &gCodom) {
		m.forward.resize(num_vertices(gDom), boost::graph_traits<GraphCodom>::null_vertex());
		m.backward.resize(num_vertices(gCodom), boost::graph_traits<GraphDom>::null_vertex());
	}
};

struct ToInvertibleVectorVertexMap {
	template<typename VertexMap>
	auto operator()(VertexMap &&m,
	                const typename VertexMapTraits<VertexMap>::GraphDom &gDom,
	                const typename VertexMapTraits<VertexMap>::GraphCodom &gCodom) const {
		using Map = InvertibleVectorVertexMap<typename VertexMapTraits<VertexMap>::GraphDom, typename VertexMapTraits<VertexMap>::GraphCodom>;
		return Map(std::forward<VertexMap>(m), gDom, gCodom);
	}
};


// InvertibleGraphMap
// =============================================================================

template<typename GraphDomT, typename GraphCodomT>
struct InvertibleVectorGraphMap : InvertibleVectorVertexMap<GraphDomT, GraphCodomT> {
	using Base = InvertibleVectorVertexMap<GraphDomT, GraphCodomT>;
	using GraphDom = GraphDomT;
	using GraphCodom = GraphCodomT;
	using Storable = std::true_type;

//	template<typename GraphDomU, typename GraphCodomU>
//	static auto
//	reinterpret(InvertibleVectorVertexMap<GraphDom, GraphCodom> &&m, const GraphDom &gDom, const GraphCodom &gCodom,
//	            const GraphDomU &gDomReinterpreted, const GraphCodomU &gCodomReinterpreted) {
//		// this of course assumes that the vertex_descriptors represent exactly the same on the reinterpreted graphs
//		return InvertibleVectorVertexMap<GraphDomU, GraphCodomU>(std::move(m.forward), std::move(m.backward),
//		                                                         gDomReinterpreted, gCodomReinterpreted);
//	}

	InvertibleVectorGraphMap(const GraphDom &gDom, const GraphCodom &gCodom)
			: Base(gDom, gCodom), forward(num_edges(gDom)), backward(num_edges(gCodom)) {}

//	template<typename VertexMap>
//	InvertibleVectorVertexMap(const VertexMap &m, const GraphDom &gDom, const GraphCodom &gCodom)
//			: InvertibleVectorVertexMap(gDom, gCodom) {
//		BOOST_CONCEPT_ASSERT((InvertibleVertexMapConcept<VertexMap>));
//		for(auto vDom: asRange(vertices(gDom))) {
//			auto vId = get(boost::vertex_index_t(), gDom, vDom);
//			forward[vId] = get(m, gDom, gCodom, vDom);
//		}
//		for(auto vCodom: asRange(vertices(gCodom))) {
//			auto vId = get(boost::vertex_index_t(), gCodom, vCodom);
//			backward[vId] = get_inverse(m, gDom, gCodom, vCodom);
//		}
//		{ // just asserts
//			for(auto vDom: asRange(vertices(gDom))) {
//				auto vCodom = get(*this, gDom, gCodom, vDom);
//				if(vCodom != boost::graph_traits<GraphCodom>::null_vertex())
//					assert(get_inverse(*this, gDom, gCodom, vCodom) == vDom);
//			}
//			for(auto vCodom: asRange(vertices(gCodom))) {
//				auto vDom = get_inverse(*this, gDom, gCodom, vCodom);
//				if(vDom != boost::graph_traits<GraphDom>::null_vertex())
//					assert(get(*this, gDom, gCodom, vDom) == vCodom);
//			}
//		}
//	}
private:
	template<typename GraphDomU, typename GraphCodomU>
	friend
	class InvertibleVectorGraphMap;

//	InvertibleVectorVertexMap(
//			std::vector<typename boost::graph_traits<GraphCodom>::vertex_descriptor> &&forward,
//			std::vector<typename boost::graph_traits<GraphDom>::vertex_descriptor> &&backward,
//			const GraphDom &gDom, const GraphCodom &gCodom)
//			: forward(std::move(forward)), backward(std::move(backward)) {
//		assert(this->forward.size() == num_vertices(gDom));
//		assert(this->backward.size() == num_vertices(gCodom));
//	}
private:
	std::vector<typename boost::graph_traits<GraphCodom>::edge_descriptor> forward;
	std::vector<typename boost::graph_traits<GraphDom>::edge_descriptor> backward;
public:
	friend typename boost::graph_traits<GraphCodom>::edge_descriptor
	get(const InvertibleVectorGraphMap &m, const GraphDom &gDom, const GraphCodom &gCodom,
	    typename boost::graph_traits<GraphDom>::edge_descriptor e) {
		assert(e != typename boost::graph_traits<GraphDom>::edge_descriptor());
		const auto eId = get(boost::edge_index_t(), gDom, e);
		assert(eId < m.forward.size());
		return m.forward[eId];
	}

	friend typename boost::graph_traits<GraphDom>::edge_descriptor
	get_inverse(const InvertibleVectorGraphMap &m, const GraphDom &gDom, const GraphCodom &gCodom,
	            typename boost::graph_traits<GraphCodom>::edge_descriptor e) {
		assert(e != typename boost::graph_traits<GraphCodom>::edge_descriptor());
		const auto eId = get(boost::edge_index_t(), gCodom, e);
		assert(eId < m.backward.size());
		return m.backward[eId];
	}

	friend void put(InvertibleVectorGraphMap &m, const GraphDom &gDom, const GraphCodom &gCodom,
	                typename boost::graph_traits<GraphDom>::edge_descriptor eDom,
	                typename boost::graph_traits<GraphCodom>::edge_descriptor eCodom) {
		using DomEdge = typename boost::graph_traits<GraphDom>::edge_descriptor;
		using CodomEdge = typename boost::graph_traits<GraphCodom>::edge_descriptor;
		assert(eDom != DomEdge());
		const auto eIdDom = get(boost::edge_index_t(), gDom, eDom);
		if(eCodom == CodomEdge()) {
			// we need to reset the inverse before we forget it,
			const auto eCodomOld = m.forward[eIdDom];
			if(eCodomOld != CodomEdge()) {
				m.forward[eIdDom] = CodomEdge();
				const auto eIdCodomOld = get(boost::edge_index_t(), gCodom, eCodomOld);
				m.backward[eIdCodomOld] = DomEdge();
			}
		} else {
			const auto eIdCodom = get(boost::edge_index_t(), gCodom, eCodom);
			assert(eIdDom < m.forward.size());
			assert(eIdCodom < m.backward.size());
			m.forward[eIdDom] = eCodom;
			m.backward[eIdCodom] = eDom;
		}
	}
public:
	friend void syncSize(InvertibleVectorGraphMap &m, const GraphDom &gDom, const GraphCodom &gCodom) {
		syncSize(static_cast<Base&>(m), gDom, gCodom);
		m.forward.resize(num_edges(gDom));
		m.backward.resize(num_edges(gCodom));
	}
};

struct ToInvertibleVectorGraphMap {
	template<typename VertexMap>
	auto operator()(VertexMap &&m,
	                const typename VertexMapTraits<VertexMap>::GraphDom &gDom,
	                const typename VertexMapTraits<VertexMap>::GraphCodom &gCodom) const {
		using Map = InvertibleVectorGraphMap<typename VertexMapTraits<VertexMap>::GraphDom, typename VertexMapTraits<VertexMap>::GraphCodom>;
		return Map(std::forward<VertexMap>(m), gDom, gCodom);
	}
};

} // namespace jla_boost::GraphMorphism

#endif // JLA_BOOST_GRAPH_MORPHISM_MODELS_INVERTIBLE_VECTOR_HPP