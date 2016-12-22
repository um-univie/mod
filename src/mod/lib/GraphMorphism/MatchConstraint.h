#ifndef MOD_LIB_GRAPHMORPHISM_MATCHCONSTRAINT_H
#define MOD_LIB_GRAPHMORPHISM_MATCHCONSTRAINT_H

#include <jla_boost/graph/morphism/VertexMap.hpp>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <string>
#include <unordered_set>

namespace mod {
namespace lib {
namespace GraphMorphism {
namespace MatchConstraint {
template<typename Graph> struct VertexAdjacency;
template<typename Graph> struct ShortestPath;

template<typename Graph, typename Ret>
struct Visitor {
	virtual Ret operator()(const VertexAdjacency<Graph> &c) const = 0;
	virtual Ret operator()(const ShortestPath<Graph> &c) const = 0;
};

template<typename Graph>
struct Constraint {
	virtual ~Constraint() = default;
	virtual std::unique_ptr<Constraint<Graph> > clone() const = 0;
	virtual void visit(Visitor<Graph, void> &visitor) const = 0;
	virtual bool visit(Visitor<Graph, bool> &visitor) const = 0;
	virtual std::string name() const = 0;
	virtual bool supportsTerm() const = 0;
};

enum class Operator {
	EQ, LT, GT, LEQ, GEQ
};

template<typename Graph>
struct VertexAdjacency : Constraint<Graph> {

	VertexAdjacency(typename boost::graph_traits<Graph>::vertex_descriptor vConstrained, Operator op, std::size_t count)
	: vConstrained(vConstrained), op(op), count(count), vertexLabels(1), edgeLabels(1) { }

	virtual std::unique_ptr<Constraint<Graph> > clone() const override {
		auto c = std::make_unique<VertexAdjacency>(vConstrained, op, count);
		c->vertexLabels = vertexLabels;
		c->edgeLabels = edgeLabels;
		return std::move(c);
	}

	virtual void visit(Visitor<Graph, void> &visitor) const override {
		visitor(*this);
	}

	virtual bool visit(Visitor<Graph, bool> &visitor) const override {
		return visitor(*this);
	}

	virtual std::string name() const override {
		return "VertexAdjacency";
	}

	virtual bool supportsTerm() const override {
		return false;
	}
public:
	typename boost::graph_traits<Graph>::vertex_descriptor vConstrained;
	Operator op;
	std::size_t count;
	std::unordered_set<std::string> vertexLabels, edgeLabels;
};

template<typename Graph>
struct ShortestPath : Constraint<Graph> {
	using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;
public:

	ShortestPath(Vertex vSrc, Vertex vTar, Operator op, std::size_t length)
	: vSrc(vSrc), vTar(vTar), op(op), length(length) { }

	virtual std::unique_ptr<Constraint<Graph> > clone() const override {
		return std::make_unique<ShortestPath>(vSrc, vTar, op, length);
	}

	virtual void visit(Visitor<Graph, void> &visitor) const override {
		visitor(*this);
	}

	virtual bool visit(Visitor<Graph, bool> &visitor) const override {
		return visitor(*this);
	}

	virtual std::string name() const override {
		return "ShortestPath";
	}

	virtual bool supportsTerm() const override {
		return true;
	}
public:
	Vertex vSrc, vTar;
	Operator op;
	std::size_t length;
};

template<typename GraphLeft, typename GraphRight, typename MapLeftRight, typename LabelRight>
struct MatchesVisitor : Visitor<GraphLeft, bool> {

	MatchesVisitor(const GraphLeft &gLeft, const GraphRight &gRight, const MapLeftRight &mLeftRight, LabelRight labelRight)
	: gLeft(gLeft), gRight(gRight), mLeftRight(mLeftRight), labelRight(labelRight) { }

	bool operator()(const VertexAdjacency<GraphLeft> &c) const {
		{ // verify
#ifndef NDEBUG
			auto vs = vertices(gLeft);
			assert(std::find(vs.first, vs.second, c.vConstrained) != vs.second);
#endif
		}
		auto vRight = get(mLeftRight, gLeft, gRight, c.vConstrained);
		if(vRight == boost::graph_traits<GraphRight>::null_vertex())
			return true; // vertex is not even mapped by the vertex map
		std::size_t count = 0;
		for(auto eRightOut : asRange(out_edges(vRight, gRight))) {
			if(!c.edgeLabels.empty()) {
				auto iter = c.edgeLabels.find(labelRight[eRightOut]);
				if(iter == c.edgeLabels.end()) continue;
			}
			if(!c.vertexLabels.empty()) {
				auto iter = c.vertexLabels.find(labelRight[target(eRightOut, gRight)]);
				if(iter == c.vertexLabels.end()) continue;
			}
			count++;
		}
		switch(c.op) {
		case Operator::EQ: return count == c.count;
		case Operator::LT: return count < c.count;
		case Operator::GT: return count > c.count;
		case Operator::LEQ: return count <= c.count;
		case Operator::GEQ: return count >= c.count;
		}
		assert(false);
		std::abort();
	}

	bool operator()(const ShortestPath<GraphLeft> &c) const {
		{ // verify
#ifndef NDEBUG
			auto vs = vertices(gLeft);
			assert(std::find(vs.first, vs.second, c.vSrc) != vs.second);
			assert(std::find(vs.first, vs.second, c.vTar) != vs.second);
#endif
		}
		auto check = [&c](std::size_t length) -> bool {
			switch(c.op) {
			case Operator::EQ: return length == c.length;
			case Operator::LT: return length < c.length;
			case Operator::GT: return length > c.length;
			case Operator::LEQ: return length <= c.length;
			case Operator::GEQ: return length >= c.length;
			}
			assert(false);
			std::abort();
		};
		auto vRightSrc = get(mLeftRight, gLeft, gRight, c.vSrc);
		auto vRightTar = get(mLeftRight, gLeft, gRight, c.vTar);
		auto vRightNull = boost::graph_traits<GraphRight>::null_vertex();
		if(vRightSrc == vRightNull && vRightTar == vRightNull) return true;
		if(vRightSrc == vRightNull || vRightTar == vRightNull) {
			return check(std::numeric_limits<std::size_t>::max());
		}
		using VertexRight = typename boost::graph_traits<GraphRight>::vertex_descriptor;
		using EdgeRight = typename boost::graph_traits<GraphRight>::edge_descriptor;
		std::vector<std::size_t> distance(num_vertices(gRight));
		std::vector<VertexRight> predesc(num_vertices(gRight));
		auto distanceMap = boost::make_iterator_property_map(distance.begin(), get(boost::vertex_index_t(), gRight));
		auto predescMap = boost::make_iterator_property_map(predesc.begin(), get(boost::vertex_index_t(), gRight));

		boost::dijkstra_shortest_paths(gRight, vRightSrc, distance_map(distanceMap).predecessor_map(predescMap)
				.weight_map(boost::make_constant_property<EdgeRight>(1)));
		auto length = distance[get(boost::vertex_index_t(), gRight, vRightTar)];
		return check(length);
	}
public:
	const GraphLeft &gLeft;
	const GraphRight &gRight;
	const MapLeftRight &mLeftRight;
	const LabelRight labelRight;
};


} // namespace MatchConstraint

template<typename ConstraintRange, typename LabelRight, typename Next>
struct CheckConstraints {

	CheckConstraints(ConstraintRange constraints, LabelRight labelRight, Next next)
	: constraints(constraints), labelRight(labelRight), next(next) { }

	template<typename Morphism, typename GraphLeft, typename GraphRight>
	bool operator()(Morphism &&m, const GraphLeft &gLeft, const GraphRight &gRight) const {
		MatchConstraint::MatchesVisitor<GraphLeft, GraphRight, Morphism, LabelRight>
				isMatching(gLeft, gRight, m, labelRight);
		for(const auto &c : constraints)
			if(!c->visit(isMatching))
				return true;
		return next(std::forward<Morphism>(m), gLeft, gRight);
	}
private:
	ConstraintRange constraints;
	const LabelRight labelRight;
	Next next;
};

template<typename ConstraintRange, typename LabelRight, typename Next = jla_boost::AlwaysTrue>
CheckConstraints<ConstraintRange, LabelRight, Next>
makeCheckConstraints(ConstraintRange constraints, LabelRight labelRight, Next &&next = jla_boost::AlwaysTrue()) {
	return CheckConstraints<ConstraintRange, LabelRight, Next>(constraints, labelRight, std::forward<Next>(next));
}

} // namespace GraphMorphism
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPHMORPHISM_MATCHCONSTRAINT_H */
