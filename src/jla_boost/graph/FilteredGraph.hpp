#ifndef JLA_BOOST_GRAPH_FILTEREDGRAPH_H
#define	JLA_BOOST_GRAPH_FILTEREDGRAPH_H

#include <jla_boost/graph/morphism/VertexMap.hpp>

#include <boost/graph/filtered_graph.hpp>

namespace jla_boost {
namespace GraphMorphism {

template<typename Graph, typename EdgePredicate, typename VertexPredicate>
struct ReinterpreterTraits<boost::filtered_graph<Graph, EdgePredicate, VertexPredicate> > {
	using type = Graph;

	static const Graph &unwrap(const boost::filtered_graph<Graph, EdgePredicate, VertexPredicate> &g) {
		return g.m_g;
	}
};

} // namespace GraphMorphism
} // namespace jla_boost

#endif	/* JLA_BOOST_GRAPH_FILTEREDGRAPH_H */