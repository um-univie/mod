#ifndef MOD_GRAPHCONCEPTS_HPP
#define MOD_GRAPHCONCEPTS_HPP

#include <mod/Chem.hpp>
#include <mod/graph/Printer.hpp>

#include <boost/concept_check.hpp>

#include <memory>
#include <type_traits>

namespace mod {
namespace detail {

template<typename T, typename = void>
struct GraphHandle {
	using type = T;
};

template<typename T>
struct GraphHandle<T, std::void_t<typename T::Handle>> {
	using type = typename T::Handle;
};

} // namespace

// rst: .. type:: template<typename GraphT> GraphHandle
// rst:
// rst:		Alias for `GraphT::Handle` if it exists, otherwise for `GraphT`.
// rst:
template<typename GraphT>
using GraphHandle = typename detail::GraphHandle<GraphT>::type;

template<typename GraphHandle>
auto getGraphFromHandle(GraphHandle g) {
	return g;
}

template<typename Graph>
auto &getGraphFromHandle(const std::shared_ptr<Graph> &g) {
	return *g;
}

} // namespace mod
namespace mod::concepts {

template<typename GraphT>
struct Graph {
	using Handle = GraphHandle<GraphT>;

	using Vertex = typename GraphT::Vertex;
	BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<Vertex>));
	BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Vertex>));
	BOOST_CONCEPT_ASSERT((boost::LessThanComparable<Vertex>));

	using Edge = typename GraphT::Edge;
	BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<Edge>));
	BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Edge>));
	BOOST_CONCEPT_ASSERT((boost::LessThanComparable<Edge>));

	using VertexRange = typename GraphT::VertexRange;
	using VertexIterator = typename GraphT::VertexIterator;
	BOOST_CONCEPT_ASSERT((boost::ForwardIterator<VertexIterator>));
	static_assert(std::is_convertible_v<typename std::iterator_traits<VertexIterator>::value_type, Vertex>);

	using EdgeRange = typename GraphT::EdgeRange;
	using EdgeIterator = typename GraphT::EdgeIterator;
	BOOST_CONCEPT_ASSERT((boost::ForwardIterator<EdgeIterator>));
	static_assert(std::is_convertible_v<typename std::iterator_traits<EdgeIterator>::value_type, Edge>);

	using IncidentEdgeRange = typename GraphT::IncidentEdgeRange;
	using IncidentEdgeIterator = typename GraphT::IncidentEdgeIterator;
	BOOST_CONCEPT_ASSERT((boost::ForwardIterator<IncidentEdgeIterator>));
	static_assert(std::is_convertible_v<typename std::iterator_traits<IncidentEdgeIterator>::value_type, Edge>);
public:
	BOOST_CONCEPT_USAGE(Graph) {
		// Vertex
		*s << v;
		nSize = v.hash();
		b = bool(v);
		b = v.isNull();
		nSize = v.getId();
		gHandle = v.getGraph();
		nSize = v.getDegree();
		ieRangeMut = v.incidentEdges();

		// Edge
		*s << e;
		b = bool(e);
		b = v.isNull();
		gHandle = e.getGraph();
		vMut = e.source();
		vMut = e.target();

		// VertexRange
		vIterMut = vRange->begin();
		vIterMut = vRange->end();

		// EdgeRange
		eIterMut = eRange->begin();
		eIterMut = eRange->end();

		// IncidentEdgeRange
		ieIterMut = ieRange->begin();
		ieIterMut = ieRange->end();

		// Graph
		nSize = g->numVertices();
		vRangeMut = g->vertices();
		nSize = g->numEdges();
		eRangeMut = g->edges();
	}
private:
	const GraphT *g;
	const Vertex v;
	const Edge e;
	const VertexRange *vRange;
	const EdgeRange *eRange;
	const IncidentEdgeRange *ieRange;
private:
	bool b;
	std::size_t nSize;
	std::ostream *s;
	Handle gHandle;
	Vertex vMut;
	VertexRange vRangeMut;
	VertexIterator vIterMut;
	EdgeRange eRangeMut;
	EdgeIterator eIterMut;
	IncidentEdgeRange ieRangeMut;
	IncidentEdgeIterator ieIterMut;
};


template<typename GraphT>
struct LabelledGraph {
	BOOST_CONCEPT_ASSERT((Graph<GraphT>));
	using Vertex = typename GraphT::Vertex;
	using Edge = typename GraphT::Edge;
public:
	BOOST_CONCEPT_USAGE(LabelledGraph) {
		// Vertex
		str = v.getStringLabel();
		atomId = v.getAtomId();
		isotope = v.getIsotope();
		charge = v.getCharge();
		b = v.getRadical();
		str = v.printStereo();
		str = v.printStereo(printer);

		// Edge
		str = e.getStringLabel();
		bondType = e.getBondType();
	}
private:
	const Vertex v;
	const Edge e;
private:
	std::string str;
	AtomId atomId;
	Isotope isotope;
	Charge charge;
	bool b;
	const graph::Printer printer;
	BondType bondType;
};

} // namespace mod::concepts

#endif // MOD_GRAPHCONCEPTS_HPP