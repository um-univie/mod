#ifndef JLA_BOOST_GRAPH_MORPHISM_MODELS_VECTOR_H
#define JLA_BOOST_GRAPH_MORPHISM_MODELS_VECTOR_H

#include <jla_boost/graph/morphism/VertexMap.hpp>

#include <vector>

// - VectorVertexMap
// - ToVectorVertexMap
// - InvertibleVectorVertexMap
// - ToInvertibleVectorVertexMap

namespace jla_boost {
namespace GraphMorphism {

// VectorVertexMap
//------------------------------------------------------------------------------

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
	: data(num_vertices(gDom), boost::graph_traits<GraphCodom>::null_vertex()) { }

	template<typename VertexMap>
	VectorVertexMap(VertexMap &&m, const GraphDom &gDom, const GraphCodom &gCodom)
	: VectorVertexMap(gDom, gCodom) {
		BOOST_CONCEPT_ASSERT((VertexMapConcept<VertexMap>));
		for(auto v : asRange(vertices(gDom))) {
			auto vId = get(boost::vertex_index_t(), gDom, v);
			data[vId] = get(m, gDom, gCodom, v);
		}
	}

	std::size_t size() const {
		return data.size();
	}
private:
	template<typename GraphDomU, typename GraphCodomU>
	friend class VectorVertexMap;
	template<typename M>
	friend class VertexMapTraits;

	VectorVertexMap(std::vector<typename boost::graph_traits<GraphCodom>::vertex_descriptor> &&data, const GraphDom &gDom, const GraphCodom &gCodom)
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
};

// ToVectorVertexMap
//------------------------------------------------------------------------------

struct ToVectorVertexMap {

	template<typename VertexMap>
	auto operator()(VertexMap &&m,
			const typename VertexMapTraits<VertexMap>::GraphDom &gDom,
			const typename VertexMapTraits<VertexMap>::GraphCodom &gCodom) const {
		using Map = VectorVertexMap<typename VertexMapTraits<VertexMap>::GraphDom, typename VertexMapTraits<VertexMap>::GraphCodom >;
		return Map(std::forward<VertexMap>(m), gDom, gCodom);
	}
};

// InvertibleVectorVertexMap
//------------------------------------------------------------------------------

template<typename GraphDomT, typename GraphCodomT>
struct InvertibleVectorVertexMap {
	using GraphDom = GraphDomT;
	using GraphCodom = GraphCodomT;
	using Storable = std::true_type;

	template<typename GraphDomU, typename GraphCodomU>
	static auto reinterpret(InvertibleVectorVertexMap<GraphDom, GraphCodom> &&m, const GraphDom &gDom, const GraphCodom &gCodom,
			const GraphDomU &gDomReinterpreted, const GraphCodomU &gCodomReinterpreted) {
		// this of course assumes that the vertex_descriptors represent exactly the same on the reinterpreted graphs
		return InvertibleVectorVertexMap<GraphDomU, GraphCodomU>(std::move(m.forward), std::move(m.backward), gDomReinterpreted, gCodomReinterpreted);
	}

	InvertibleVectorVertexMap(const GraphDom &gDom, const GraphCodom &gCodom)
	: forward(num_vertices(gDom), boost::graph_traits<GraphCodom>::null_vertex()),
	backward(num_vertices(gCodom), boost::graph_traits<GraphDom>::null_vertex()) { }

	template<typename VertexMap>
	InvertibleVectorVertexMap(const VertexMap &m, const GraphDom &gDom, const GraphCodom &gCodom)
	: InvertibleVectorVertexMap(gDom, gCodom) {
		BOOST_CONCEPT_ASSERT((InvertibleVertexMapConcept<VertexMap>));
		for(auto vDom : asRange(vertices(gDom))) {
			auto vId = get(boost::vertex_index_t(), gDom, vDom);
			forward[vId] = get(m, gDom, gCodom, vDom);
		}
		for(auto vCodom : asRange(vertices(gCodom))) {
			auto vId = get(boost::vertex_index_t(), gCodom, vCodom);
			backward[vId] = get_inverse(m, gDom, gCodom, vCodom);
		}
		{ // just asserts
			for(auto vDom : asRange(vertices(gDom))) {
				auto vCodom = get(*this, gDom, gCodom, vDom);
				if(vCodom != boost::graph_traits<GraphCodom>::null_vertex())
					assert(get_inverse(*this, gDom, gCodom, vCodom) == vDom);
			}
			for(auto vCodom : asRange(vertices(gCodom))) {
				auto vDom = get_inverse(*this, gDom, gCodom, vCodom);
				if(vDom != boost::graph_traits<GraphDom>::null_vertex())
					assert(get(*this, gDom, gCodom, vDom) == vCodom);
			}
		}
	}
private:
	template<typename GraphDomU, typename GraphCodomU>
	friend class InvertibleVectorVertexMap;

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

	void resizeRight(const GraphDom &gDom, const GraphCodom &gCodom) {
		backward.resize(num_vertices(gCodom), boost::graph_traits<GraphDom>::null_vertex());
	}
};

// ToInvertibleVectorVertexMap
//------------------------------------------------------------------------------

struct ToInvertibleVectorVertexMap {

	template<typename VertexMap>
	auto operator()(VertexMap &&m,
			const typename VertexMapTraits<VertexMap>::GraphDom &gDom,
			const typename VertexMapTraits<VertexMap>::GraphCodom &gCodom) const {
		using Map = InvertibleVectorVertexMap<typename VertexMapTraits<VertexMap>::GraphDom, typename VertexMapTraits<VertexMap>::GraphCodom >;
		return Map(std::forward<VertexMap>(m), gDom, gCodom);
	}
};

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_MODELS_VECTOR_H */