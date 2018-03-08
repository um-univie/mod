#ifndef MOD_LIB_GRAPHMORPHISM_SHORTESTPATH_H
#define MOD_LIB_GRAPHMORPHISM_SHORTESTPATH_H

#include <mod/Config.h>
#include <mod/lib/LabelledGraph.h>
#include <mod/lib/GraphMorphism/Constraints/Constraint.h>

#include <jla_boost/graph/morphism/VertexMap.hpp>

#include <boost/graph/dijkstra_shortest_paths.hpp>

namespace mod {
namespace lib {
namespace GraphMorphism {
namespace Constraints {

template<typename Graph>
struct ShortestPath : Constraint<Graph> {
	MOD_VISITABLE();
	using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;
public:

	ShortestPath(Vertex vSrc, Vertex vTar, Operator op, int length)
	: vSrc(vSrc), vTar(vTar), op(op), length(length) { }

	virtual std::unique_ptr<Constraint<Graph> > clone() const override {
		return std::make_unique<ShortestPath>(vSrc, vTar, op, length);
	}

	virtual std::string name() const override {
		return "ShortestPath";
	}

	virtual bool supportsTerm() const override {
		return true;
	}

	template<typename Visitor, typename LabelledGraphCodom, typename VertexMap>
	bool matches(Visitor &vis, const Graph &gDom, const LabelledGraphCodom &lgCodom, const VertexMap &m, const LabelSettings ls) const {
		using GraphCodom = typename LabelledGraphTraits<LabelledGraphCodom>::GraphType;
		static_assert(std::is_same<Graph, typename jla_boost::GraphMorphism::VertexMapTraits<VertexMap>::GraphDom>::value, "");
		static_assert(std::is_same<GraphCodom, typename jla_boost::GraphMorphism::VertexMapTraits<VertexMap>::GraphCodom>::value, "");
		const GraphCodom &gCodom = get_graph(lgCodom);
		{ // verify
#ifndef NDEBUG
			const auto vs = vertices(gDom);
			assert(std::find(vs.first, vs.second, vSrc) != vs.second);
			assert(std::find(vs.first, vs.second, vTar) != vs.second);
#endif
		}
		const auto check = [this](int length) -> bool {
			switch(op) {
			case Operator::EQ: return length == this->length;
			case Operator::LT: return length < this->length;
			case Operator::GT: return length > this->length;
			case Operator::LEQ: return length <= this->length;
			case Operator::GEQ: return length >= this->length;
			}
			assert(false);
			std::abort();
		};
		const auto vSrcCodom = get(m, gDom, gCodom, vSrc);
		const auto vTarCodom = get(m, gDom, gCodom, vTar);
		const auto vRightNull = boost::graph_traits<GraphCodom>::null_vertex();
		if(vSrcCodom == vRightNull && vTarCodom == vRightNull) return true;
		if(vSrcCodom == vRightNull || vTarCodom == vRightNull) {
			return check(std::numeric_limits<int>::max());
		}
		using VertexCodom = typename boost::graph_traits<GraphCodom>::vertex_descriptor;
		using EdgeCodom = typename boost::graph_traits<GraphCodom>::edge_descriptor;
		std::vector<int> distance(num_vertices(gCodom));
		std::vector<VertexCodom> predesc(num_vertices(gCodom));
		auto distanceMap = boost::make_iterator_property_map(distance.begin(), get(boost::vertex_index_t(), gCodom));
		auto predescMap = boost::make_iterator_property_map(predesc.begin(), get(boost::vertex_index_t(), gCodom));

		boost::dijkstra_shortest_paths(gCodom, vSrcCodom, distance_map(distanceMap).predecessor_map(predescMap)
				.weight_map(boost::make_constant_property<EdgeCodom>(1)));
		const auto length = distance[get(boost::vertex_index_t(), gCodom, vTarCodom)];
		return check(length);
	}
public:
	Vertex vSrc, vTar;
	Operator op;
	int length;
};

} // namespace Constraints
} // namespace GraphMorphism
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPHMORPHISM_SHORTESTPATH_H */