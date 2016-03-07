#ifndef MOD_LIB_RULE_LABELLED_RULE_H
#define MOD_LIB_RULE_LABELLED_RULE_H

#include <mod/lib/GraphMorphism/MatchConstraint.h>
#include <mod/lib/Rule/ConnectedComponent.h>
#include <mod/lib/Rule/GraphDecl.h>
#include <mod/lib/Rule/Properties/String.h>

#include <jla_boost/graph/dpo/FilteredGraphProjection.hpp>

#include <vector>

namespace mod {
namespace lib {
namespace Rule {
namespace detail {

template<typename LabelledRuleU>
struct LabelledSideGraph {
	using LabelledRule = LabelledRuleU;
	using GraphType = jla_boost::GraphDPO::FilteredGraphProjection<LabelledRule>;
	using ComponentFilter = ConnectedComponentFilter<GraphType, std::vector<std::size_t> >;
	using ComponentGraph = boost::filtered_graph<GraphType, ComponentFilter, ComponentFilter>;
public:

	LabelledSideGraph(const LabelledRule &r, jla_boost::GraphDPO::Membership m)
	: r(r), g(get_graph(r), m) { }

	template<typename LabelledRuleT>
	friend const GraphType &get_graph(const LabelledSideGraph<LabelledRuleT> &g) {
		return g.g;
	}
protected:
	const LabelledRule &r;
	const GraphType g;
};

} // namespace detail

template<typename LabelledRule>
struct LabelledLeftGraph : detail::LabelledSideGraph<LabelledRule> {
	using Base = detail::LabelledSideGraph<LabelledRule>;
	using PropStringType = typename LabelledRule::PropStringType::LeftType;
public:

	explicit LabelledLeftGraph(const LabelledRule &r)
	: Base(r, jla_boost::GraphDPO::Membership::Left) { }

	template<typename LabelledRuleT>
	friend PropStringType get_string(const LabelledLeftGraph<LabelledRuleT> &g) {
		return get_string(g.r).getLeft();
	}
public:

	template<typename LabelledRuleT>
	friend const std::vector<std::unique_ptr<typename LabelledRuleT::MatchConstraint> > &
	get_match_constraints(const LabelledLeftGraph<LabelledRuleT> &g) {
		return g.r.leftComponentMatchConstraints;
	}
public:

	template<typename LabelledRuleT>
	friend std::size_t get_num_connected_components(const LabelledLeftGraph<LabelledRuleT> &g) {
		return g.r.numLeftComponents;
	}

	template<typename LabelledRuleT>
	friend typename Base::ComponentGraph
	get_component_graph(std::size_t i, const LabelledLeftGraph<LabelledRuleT> &g) {
		assert(i < get_num_connected_components(g));
		typename Base::ComponentFilter filter(&g.g, &g.r.leftComponents, i);
		return typename Base::ComponentGraph(g.g, filter, filter);
	}
};

template<typename LabelledRule>
struct LabelledRightGraph : detail::LabelledSideGraph<LabelledRule> {
	using Base = detail::LabelledSideGraph<LabelledRule>;
	using PropStringType = typename LabelledRule::PropStringType::RightType;
public:

	explicit LabelledRightGraph(const LabelledRule &r)
	: Base(r, jla_boost::GraphDPO::Membership::Right) { }

	template<typename LabelledRuleT>
	friend PropStringType get_string(const LabelledRightGraph<LabelledRuleT> &g) {
		return get_string(g.r).getRight();
	}
public:

	template<typename LabelledRuleT>
	friend const std::vector<std::unique_ptr<typename LabelledRuleT::MatchConstraint> > &
	get_match_constraints(const LabelledRightGraph<LabelledRuleT> &g) {
		return g.r.rightComponentMatchConstraints;
	}
public:

	template<typename LabelledRuleT>
	friend std::size_t get_num_connected_components(const LabelledRightGraph<LabelledRuleT> &g) {
		return g.r.numRightComponents;
	}

	template<typename LabelledRuleT>
	friend typename Base::ComponentGraph
	get_component_graph(std::size_t i, const LabelledRightGraph<LabelledRuleT> &g) {
		assert(i < get_num_connected_components(g));
		typename Base::ComponentFilter filter(&g.g, &g.r.rightComponents, i);
		return typename Base::ComponentGraph(g.g, filter, filter);
	}
};

struct LabelledRule {
	using GraphType = Rule::GraphType;
	using Vertex = boost::graph_traits<GraphType>::vertex_descriptor;
	using Edge = boost::graph_traits<GraphType>::edge_descriptor;
	using PropStringType = PropStringCore;
	using MatchConstraint = GraphMorphism::MatchConstraint::Constraint<GraphType>;
	using LabelledLeftType = LabelledLeftGraph<LabelledRule>;
	using LabelledRightType = LabelledRightGraph<LabelledRule>;
public:

	LabelledRule() : g(new GraphType()) { }

	friend GraphType &get_graph(LabelledRule &r) {
		return *r.g;
	}

	friend const GraphType &get_graph(const LabelledRule &r) {
		return *r.g;
	}

	friend const PropStringType &get_string(const LabelledRule &r) {
		assert(r.pString);
		return *r.pString;
	}

	friend jla_boost::GraphDPO::Membership membership(const LabelledRule &r, const Vertex &v) {
		return get_graph(r)[v].membership;
	}

	friend jla_boost::GraphDPO::Membership membership(const LabelledRule &r, const Edge &e) {
		return get_graph(r)[e].membership;
	}

	friend void put_membership(LabelledRule &r, const Vertex &v, jla_boost::GraphDPO::Membership m) {
		get_graph(r)[v].membership = m;
	}

	friend void put_membership(LabelledRule &r, const Edge &e, jla_boost::GraphDPO::Membership m) {
		get_graph(r)[e].membership = m;
	}
public:

	friend LabelledLeftType get_labelled_left(const LabelledRule &r) {
		return LabelledLeftType(r);
	}

	friend LabelledRightType get_labelled_right(const LabelledRule &r) {
		return LabelledRightType(r);
	}
public:
	std::unique_ptr<GraphType> g;
	std::unique_ptr<PropStringType> pString;
	std::vector<std::unique_ptr<MatchConstraint> > leftComponentMatchConstraints, rightComponentMatchConstraints;
public:
	std::size_t numLeftComponents = -1, numRightComponents = -1;
	std::vector<std::size_t> leftComponents, rightComponents;
};

} // namespace Rule
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RULE_LABELLED_RULE_H */
