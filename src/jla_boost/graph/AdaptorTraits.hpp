#ifndef JLA_BOOST_GRAPH_ADAPTORTRAITS_HPP
#define JLA_BOOST_GRAPH_ADAPTORTRAITS_HPP

// Traits for graphs like boost::filtered_graph that adapts another graph.

#include <boost/graph/filtered_graph.hpp>

namespace jla_boost {

template<typename Graph>
struct GraphAdaptorTraits {
	//	using type = ...; // the type of the adapted graph

	//	static const type &unwrap(const Graph &g) {
	//		return ...;
	//	}
};

template<typename Graph, typename EdgePredicate, typename VertexPredicate>
struct GraphAdaptorTraits<boost::filtered_graph<Graph, EdgePredicate, VertexPredicate> > {
	using type = typename std::remove_const<Graph>::type;

	static const Graph &unwrap(const boost::filtered_graph<Graph, EdgePredicate, VertexPredicate> &g) {
		return g.m_g;
	}
};

} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_ADAPTORTRAITS_HPP */