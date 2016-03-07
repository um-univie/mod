#ifndef JLA_BOOST_GRAPH_MORPHISM_VECTOR_VERTEX_MAP_H
#define JLA_BOOST_GRAPH_MORPHISM_VECTOR_VERTEX_MAP_H

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

template<typename GraphLeftT, typename GraphRightT>
struct VectorVertexMap {
	using GraphLeft = GraphLeftT;
	using GraphRight = GraphRightT;
	using Storable = std::true_type;

	template<typename GraphLeftU, typename GraphRightU>
	struct Reinterpret {
		using type = VectorVertexMap<GraphLeftU, GraphRightU>;
	};

	template<typename GraphLeftU, typename GraphRightU>
	static VectorVertexMap<GraphLeftU, GraphRightU>
	reinterpret(VectorVertexMap<GraphLeft, GraphRight> &&m, const GraphLeft &gLeft, const GraphRight &gRight,
			const GraphLeftU &gLeftReinterpreted, const GraphRightU &gRightReinterpreted) {
		// this of course assumes that the vertex_descriptors represent exactly the same on the reinterpreted graphs
		return VectorVertexMap<GraphLeftU, GraphRightU>(std::move(m.data), gLeftReinterpreted, gRightReinterpreted);
	}

	VectorVertexMap(const GraphLeft &gLeft, const GraphRight &gRight)
	: data(num_vertices(gLeft), boost::graph_traits<GraphRight>::null_vertex()) { }

	template<typename VertexMap>
	VectorVertexMap(VertexMap &&m, const GraphLeft &gLeft, const GraphRight &gRight)
	: VectorVertexMap(gLeft, gRight) {
		BOOST_CONCEPT_ASSERT((VertexMapConcept<VertexMap>));
		for(auto v : asRange(vertices(gLeft))) {
			auto vId = get(boost::vertex_index_t(), gLeft, v);
			data[vId] = get(m, gLeft, gRight, v);
		}
	}

	std::size_t size() const {
		return data.size();
	}
private:
	template<typename GraphLeftU, typename GraphRightU>
	friend class VectorVertexMap;

	VectorVertexMap(std::vector<typename boost::graph_traits<GraphRight>::vertex_descriptor> &&data, const GraphLeft &gLeft, const GraphRight &gRight)
	: data(std::move(data)) {
		assert(this->data.size() == num_vertices(gLeft));
	}
private:
	std::vector<typename boost::graph_traits<GraphRight>::vertex_descriptor> data;
public:

	friend typename boost::graph_traits<GraphRight>::vertex_descriptor
	get(const VectorVertexMap<GraphLeft, GraphRight> &m, const GraphLeft &gLeft, const GraphRight &gRight,
			typename boost::graph_traits<GraphLeft>::vertex_descriptor v) {
		auto vId = get(boost::vertex_index_t(), gLeft, v);
		assert(vId < m.data.size());
		return m.data[vId];
	}

	friend void put(VectorVertexMap<GraphLeft, GraphRight> &m, const GraphLeft &gLeft, const GraphRight &gRight,
			typename boost::graph_traits<GraphLeft>::vertex_descriptor vLeft,
			typename boost::graph_traits<GraphRight>::vertex_descriptor vRight) {
		auto vId = get(boost::vertex_index_t(), gLeft, vLeft);
		assert(vId < m.data.size());
		m.data[vId] = vRight;
	}
};

// ToVectorVertexMap
//------------------------------------------------------------------------------

template<typename Next>
struct ToVectorVertexMap {

	ToVectorVertexMap(Next next) : next(next) { }

	template<typename VertexMap>
	bool operator()(VertexMap &&m,
			const typename VertexMapTraits<VertexMap>::GraphLeft &gLeft,
			const typename VertexMapTraits<VertexMap>::GraphRight &gRight) const {
		VectorVertexMap<typename VertexMapTraits<VertexMap>::GraphLeft, typename VertexMapTraits<VertexMap>::GraphRight >
				mNew(std::forward<VertexMap>(m), gLeft, gRight);
		return next(std::move(mNew), gLeft, gRight);
	}
private:
	Next next;
};

template<typename Next>
ToVectorVertexMap<Next> makeToVectorVertexMap(Next &&next) {
	return ToVectorVertexMap<Next>(std::forward<Next>(next));
}

// InvertibleVectorVertexMap
//------------------------------------------------------------------------------

template<typename GraphLeftT, typename GraphRightT>
struct InvertibleVectorVertexMap {
	using GraphLeft = GraphLeftT;
	using GraphRight = GraphRightT;
	using Storable = std::true_type;

	template<typename GraphLeftU, typename GraphRightU>
	struct Reinterpret {
		using type = InvertibleVectorVertexMap<GraphLeftU, GraphRightU>;
	};

	template<typename GraphLeftU, typename GraphRightU>
	static InvertibleVectorVertexMap<GraphLeftU, GraphRightU>
	reinterpret(InvertibleVectorVertexMap<GraphLeft, GraphRight> &&m, const GraphLeft &gLeft, const GraphRight &gRight,
			const GraphLeftU &gLeftReinterpreted, const GraphRightU &gRightReinterpreted) {
		// this of course assumes that the vertex_descriptors represent exactly the same on the reinterpreted graphs
		return InvertibleVectorVertexMap<GraphLeftU, GraphRightU>(std::move(m.forward), std::move(m.backward), gLeftReinterpreted, gRightReinterpreted);
	}

	InvertibleVectorVertexMap(const GraphLeft &gLeft, const GraphRight &gRight)
	: forward(num_vertices(gLeft), boost::graph_traits<GraphRight>::null_vertex()),
	backward(num_vertices(gRight), boost::graph_traits<GraphLeft>::null_vertex()) { }

	template<typename VertexMap>
	InvertibleVectorVertexMap(VertexMap &&m, const GraphLeft &gLeft, const GraphRight &gRight)
	: InvertibleVectorVertexMap(gLeft, gRight) {
		BOOST_CONCEPT_ASSERT((InvertibleVertexMapConcept<typename std::remove_reference<VertexMap>::type>));
		for(auto vLeft : asRange(vertices(gLeft))) {
			auto vId = get(boost::vertex_index_t(), gLeft, vLeft);
			forward[vId] = get(m, gLeft, gRight, vLeft);
		}
		for(auto vRight : asRange(vertices(gRight))) {
			auto vId = get(boost::vertex_index_t(), gRight, vRight);
			backward[vId] = get_inverse(m, gLeft, gRight, vRight);
		}
		{ // just asserts
			for(auto vLeft : asRange(vertices(gLeft))) {
				auto vRight = get(*this, gLeft, gRight, vLeft);
				if(vRight != boost::graph_traits<GraphRight>::null_vertex())
					assert(get_inverse(*this, gLeft, gRight, vRight) == vLeft);
			}
			for(auto vRight : asRange(vertices(gRight))) {
				auto vLeft = get(*this, gLeft, gRight, vRight);
				if(vLeft != boost::graph_traits<GraphLeft>::null_vertex())
					assert(get(*this, gLeft, gRight, vLeft) == vRight);
			}
		}
	}

	std::size_t size() const {
		return forward.size();
	}

	std::size_t sizeInverse() const {
		return backward.size();
	}
private:
	template<typename GraphLeftU, typename GraphRightU>
	friend class InvertibleVectorVertexMap;

	InvertibleVectorVertexMap(
			std::vector<typename boost::graph_traits<GraphRight>::vertex_descriptor> &&forward,
			std::vector<typename boost::graph_traits<GraphLeft>::vertex_descriptor> &&backward,
			const GraphLeft &gLeft, const GraphRight &gRight)
	: forward(std::move(forward)), backward(std::move(backward)) {
		assert(this->forward.size() == num_vertices(gLeft));
		assert(this->backward.size() == num_vertices(gRight));
	}
private:
	std::vector<typename boost::graph_traits<GraphRight>::vertex_descriptor> forward;
	std::vector<typename boost::graph_traits<GraphLeft>::vertex_descriptor> backward;
public:

	friend typename boost::graph_traits<GraphRight>::vertex_descriptor
	get(const InvertibleVectorVertexMap<GraphLeft, GraphRight> &m, const GraphLeft &gLeft, const GraphRight &gRight,
			typename boost::graph_traits<GraphLeft>::vertex_descriptor v) {
		assert(v != boost::graph_traits<GraphLeft>::null_vertex());
		auto vId = get(boost::vertex_index_t(), gLeft, v);
		assert(vId < m.forward.size());
		return m.forward[vId];
	}

	friend typename boost::graph_traits<GraphLeft>::vertex_descriptor
	get_inverse(const InvertibleVectorVertexMap<GraphLeft, GraphRight> &m, const GraphLeft &gLeft, const GraphRight &gRight,
			typename boost::graph_traits<GraphRight>::vertex_descriptor v) {
		assert(v != boost::graph_traits<GraphRight>::null_vertex());
		auto vId = get(boost::vertex_index_t(), gRight, v);
		assert(vId < m.backward.size());
		return m.backward[vId];
	}

	friend void put(InvertibleVectorVertexMap<GraphLeft, GraphRight> &m, const GraphLeft &gLeft, const GraphRight &gRight,
			typename boost::graph_traits<GraphLeft>::vertex_descriptor vLeft,
			typename boost::graph_traits<GraphRight>::vertex_descriptor vRight) {
		assert(vLeft != boost::graph_traits<GraphLeft>::null_vertex());
		auto vIdLeft = get(boost::vertex_index_t(), gLeft, vLeft);
		if(vRight == boost::graph_traits<GraphRight>::null_vertex()) {
			// we need to reset the inverse before we forget it,
			auto vRightOld = m.forward[vIdLeft];
			if(vRightOld != boost::graph_traits<GraphRight>::null_vertex()) {
				m.forward[vIdLeft] = boost::graph_traits<GraphRight>::null_vertex();
				auto vIdRightOld = get(boost::vertex_index_t(), gRight, vRightOld);
				m.backward[vIdRightOld] = boost::graph_traits<GraphLeft>::null_vertex();
			}
		} else {
			auto vIdRight = get(boost::vertex_index_t(), gRight, vRight);
			assert(vIdLeft < m.forward.size());
			assert(vIdRight < m.backward.size());
			m.forward[vIdLeft] = vRight;
			m.backward[vIdRight] = vLeft;
		}
	}
};

// ToInvertibleVectorVertexMap
//------------------------------------------------------------------------------

template<typename Next>
struct ToInvertibleVectorVertexMap {

	ToInvertibleVectorVertexMap(Next next) : next(next) { }

	template<typename VertexMap>
	bool operator()(VertexMap &&m,
			const typename VertexMapTraits<VertexMap>::GraphLeft &gLeft,
			const typename VertexMapTraits<VertexMap>::GraphRight &gRight) const {
		InvertibleVectorVertexMap<typename VertexMapTraits<VertexMap>::GraphLeft, typename VertexMapTraits<VertexMap>::GraphRight >
				mNew(std::forward<VertexMap>(m), gLeft, gRight);
		return next(std::move(mNew), gLeft, gRight);
	}
private:
	Next next;
};

template<typename Next>
ToInvertibleVectorVertexMap<Next> makeToInvertibleVectorVertexMap(Next &&next) {
	return ToInvertibleVectorVertexMap<Next>(std::forward<Next>(next));
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_VECTOR_VERTEX_MAP_H */