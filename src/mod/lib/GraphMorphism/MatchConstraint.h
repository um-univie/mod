#ifndef MOD_LIB_GRAPHMORPHISM_MATCHCONSTRAINT_H
#define	MOD_LIB_GRAPHMORPHISM_MATCHCONSTRAINT_H

#include <jla_boost/graph/morphism/VertexMap.hpp>

#include <boost/graph/graph_traits.hpp>

#include <string>
#include <unordered_set>

namespace mod {
namespace lib {
namespace GraphMorphism {
namespace MatchConstraint {
template<typename Graph> struct VertexAdjacency;

template<typename Graph, typename Ret>
struct Visitor {
	virtual Ret operator()(const VertexAdjacency<Graph> &c) const = 0;
};

template<typename Graph>
struct Constraint {
	virtual ~Constraint() = default;
	virtual void visit(Visitor<Graph, void> &visitor) const = 0;
	virtual bool visit(Visitor<Graph, bool> &visitor) const = 0;
};

enum class Operator {
	EQ, LT, GT, LEQ, GEQ
};

// A converted version of MC_NodeAdjacency from GGL

template<typename Graph>
struct VertexAdjacency : Constraint<Graph> {

	VertexAdjacency(typename boost::graph_traits<Graph>::vertex_descriptor vConstrained, Operator op, std::size_t count)
	: vConstrained(vConstrained), op(op), count(count), vertexLabels(1), edgeLabels(1) { }

	virtual void visit(Visitor<Graph, void> &visitor) const override {
		visitor(*this);
	}

	virtual bool visit(Visitor<Graph, bool> &visitor) const override {
		return visitor(*this);
	}
public:
	typename boost::graph_traits<Graph>::vertex_descriptor vConstrained;
	Operator op;
	std::size_t count;
	std::unordered_set<std::string> vertexLabels, edgeLabels;
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
				if(c.edgeLabels.find(labelRight[eRightOut]) == c.edgeLabels.end()) continue;
			}
			if(!c.vertexLabels.empty()) {
				if(c.vertexLabels.find(labelRight[target(eRightOut, gRight)]) == c.vertexLabels.end()) continue;
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
	const LabelRight &labelRight;
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

#endif	/* MOD_LIB_GRAPHMORPHISM_MATCHCONSTRAINT_H */