#ifndef JLA_BOOST_GRAPH_MORPHISM_PRINTCALLBACK_HPP
#define JLA_BOOST_GRAPH_MORPHISM_PRINTCALLBACK_HPP

#include <jla_boost/Functional.hpp>
#include <jla_boost/graph/PairToRangeAdaptor.hpp>

namespace jla_boost {
namespace GraphMorphism {

template<typename Next>
struct PrintCallback {

	PrintCallback(Next next) : next(next) { }

	template<typename VertexMap, typename GraphDom, typename GraphCodom>
	bool operator()(VertexMap &&m, const GraphDom &gDom, const GraphCodom &gCodom) const {
		for(auto vDom : asRange(vertices(gDom))) {
			std::cout << '(' << get(boost::vertex_index_t(), gDom, vDom) << ", ";
			auto vCodom = get(m, gDom, gCodom, vDom);
			if(vCodom == boost::graph_traits<GraphCodom>::null_vertex()) std::cout << "nil";
			else std::cout << get(boost::vertex_index_t(), gCodom, vCodom);
			std::cout << ') ';
		}
		std::cout << '\n';
		return next(std::forward<VertexMap>(m), gDom, gCodom);
	}
private:
	Next next;
};

template<typename Next = AlwaysTrue>
PrintCallback<Next> makePrintCallback(Next &&next = AlwaysTrue()) {
	return PrintCallback<Next>(std::forward<Next>(next));
}

} // namespace GraphMorphism
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_MORPHISM_PRINTCALLBACK_HPP */