#ifndef JLA_BOOST_GRAPH_DPO_RULE_HPP
#define JLA_BOOST_GRAPH_DPO_RULE_HPP

#include <boost/graph/graph_concepts.hpp>

namespace jla_boost {
namespace GraphDPO {

enum class Membership {
	Left, Right, Context
};

template<typename R>
struct PushoutRuleTraits;

template<typename R>
struct PushoutRuleConcept {
	using Traits = PushoutRuleTraits<R>;
	using GraphType = typename Traits::GraphType;
	using LeftGraphType = typename Traits::LeftGraphType;
	using ContextGraphType = typename Traits::ContextGraphType;
	using RightGraphType = typename Traits::RightGraphType;

	using Vertex = typename boost::graph_traits<GraphType>::vertex_descriptor;
	using Edge = typename boost::graph_traits<GraphType>::edge_descriptor;

	BOOST_CONCEPT_ASSERT((boost::GraphConcept<GraphType>));

	BOOST_CONCEPT_USAGE(PushoutRuleConcept) {
		const R &rConst = r;
		const GraphType &gConst = get_graph(rConst);
		(void) gConst;
		const LeftGraphType &gLeft = get_left(rConst);
		const ContextGraphType &gContext = get_context(rConst);
		const RightGraphType &gRight = get_right(rConst);
		(void) gLeft;
		(void) gContext;
		(void) gRight;
		m = membership(r, v);
		m = membership(r, e);
	}
private:
	R r;
	Vertex v;
	Edge e;
private:
	Membership m;
};

template<typename R>
struct WritablePushoutRuleConcept : PushoutRuleConcept<R> {
	using Traits = PushoutRuleTraits<R>;
	using GraphType = typename Traits::GraphType;
	using LeftGraphType = typename Traits::LeftGraphType;
	using ContextGraphType = typename Traits::ContextGraphType;
	using RightGraphType = typename Traits::RightGraphType;

	using Vertex = typename boost::graph_traits<GraphType>::vertex_descriptor;
	using Edge = typename boost::graph_traits<GraphType>::edge_descriptor;

	BOOST_CONCEPT_USAGE(WritablePushoutRuleConcept) {
		GraphType &g = get_graph(r);
		(void) g;
		put_membership(r, v, m);
		put_membership(r, e, m);
	}
private:
	R r;
	Vertex v;
	Edge e;
private:
	Membership m;
};

template<typename R>
struct PushoutRuleTraits {
	using GraphType = typename R::GraphType;
	using LeftGraphType = typename R::LeftGraphType;
	using ContextGraphType = typename R::ContextGraphType;
	using RightGraphType = typename R::RightGraphType;
};

} // namespace GraphDPO
} // namespace jla_boost

#endif /* JLA_BOOST_GRAPH_DPO_RULE_HPP */