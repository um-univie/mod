#include "LabelledRule.h"

#include <mod/lib/GraphMorphism/Finder.hpp>

#include <boost/graph/connected_components.hpp>

namespace mod {
namespace lib {
namespace Rules {

// LabelledRule
//------------------------------------------------------------------------------

LabelledRule::LabelledRule() : g(new GraphType()) { }

LabelledRule::LabelledRule(const LabelledRule &other, bool withConstraints) : LabelledRule() {
	auto &g = *this->g;
	const auto &gOther = get_graph(other);
	this->pString = std::make_unique<PropStringType>(g);
	auto &pString = *this->pString;
	auto &pStringOther = get_string(other);
	for(Vertex vOther : asRange(vertices(gOther))) {
		Vertex v = add_vertex(g);
		g[v].membership = membership(other, vOther);
		switch(g[v].membership) {
		case Membership::Left:
			pString.add(v, pStringOther.getLeft()[vOther], "");
			break;
		case Membership::Right:
			pString.add(v, "", pStringOther.getRight()[vOther]);
			break;
		case Membership::Context:
			pString.add(v, pStringOther.getRight()[vOther], pStringOther.getLeft()[vOther]);
			break;
		}
	}
	for(Edge eOther : asRange(edges(gOther))) {
		Edge e = add_edge(source(eOther, gOther), target(eOther, gOther), g).first;
		g[e].membership = membership(other, eOther);
		switch(g[e].membership) {
		case Membership::Left:
			pString.add(e, pStringOther.getLeft()[eOther], "");
			break;
		case Membership::Right:
			pString.add(e, "", pStringOther.getRight()[eOther]);
			break;
		case Membership::Context:
			pString.add(e, pStringOther.getLeft()[eOther], pStringOther.getRight()[eOther]);
			break;
		}
	}
	if(withConstraints) {
		leftComponentMatchConstraints.reserve(other.leftComponentMatchConstraints.size());
		rightComponentMatchConstraints.reserve(other.rightComponentMatchConstraints.size());
		for(auto &&c : other.leftComponentMatchConstraints)
			leftComponentMatchConstraints.push_back(c->clone());
		for(auto &&c : other.rightComponentMatchConstraints)
			rightComponentMatchConstraints.push_back(c->clone());
	}
}

void LabelledRule::initComponents() { // TODO: structure this better
	if(numLeftComponents != std::numeric_limits<std::size_t>::max()) MOD_ABORT;
	leftComponents.resize(num_vertices(get_graph(*this)), -1);
	rightComponents.resize(num_vertices(get_graph(*this)), -1);
	numLeftComponents = boost::connected_components(get_graph(get_labelled_left(*this)), leftComponents.data());
	numRightComponents = boost::connected_components(get_graph(get_labelled_right(*this)), rightComponents.data());
}

void LabelledRule::invert() {
	// clear cached stuff
	this->projs.reset();
	// and not the actual inversion
	auto &g = *this->g;
	auto &pString = *this->pString;
	for(Vertex v : asRange(vertices(g))) {
		auto membership = g[v].membership;
		switch(membership) {
		case Membership::Left:
		{
			auto label = pString.getLeft()[v];
			g[v].membership = Membership::Right;
			pString.setRight(v, label);
		}
			break;
		case Membership::Right:
		{
			auto label = pString.getRight()[v];
			g[v].membership = Membership::Left;
			pString.setLeft(v, label);
		}
			break;
		case Membership::Context:
		{
			auto left = pString.getLeft()[v];
			auto right = pString.getRight()[v];
			pString.setLeft(v, right);
			pString.setRight(v, left);
		}
			break;
		}
	}

	for(Edge e : asRange(edges(g))) {
		auto membership = g[e].membership;
		switch(membership) {
		case Membership::Left:
		{
			auto label = pString.getLeft()[e];
			g[e].membership = Membership::Right;
			pString.setRight(e, label);
		}
			break;
		case Membership::Right:
		{
			auto label = pString.getRight()[e];
			g[e].membership = Membership::Left;
			pString.setLeft(e, label);
		}
			break;
		case Membership::Context:
		{
			auto left = pString.getLeft()[e];
			auto right = pString.getRight()[e];
			pString.setLeft(e, right);
			pString.setRight(e, left);
		}
			break;
		}
	}
	// also invert the component stuff
	using std::swap;
	swap(this->numLeftComponents, this->numRightComponents);
	swap(this->leftComponents, this->rightComponents);
	swap(this->leftComponentMatchConstraints, this->rightComponentMatchConstraints);
}

GraphType &get_graph(LabelledRule &r) {
	return *r.g;
}

const GraphType &get_graph(const LabelledRule &r) {
	return *r.g;
}

const LabelledRule::PropStringType &get_string(const LabelledRule &r) {
	assert(r.pString);
	return *r.pString;
}

const LabelledRule::LeftGraphType &get_left(const LabelledRule &r) {
	if(!r.projs) r.projs.reset(new LabelledRule::Projections(r));
	return r.projs->left;
}

const LabelledRule::ContextGraphType &get_context(const LabelledRule &r) {
	if(!r.projs) r.projs.reset(new LabelledRule::Projections(r));
	return r.projs->context;
}

const LabelledRule::RightGraphType &get_right(const LabelledRule &r) {
	if(!r.projs) r.projs.reset(new LabelledRule::Projections(r));
	return r.projs->right;
}

jla_boost::GraphDPO::Membership membership(const LabelledRule &r, const Vertex &v) {
	return get_graph(r)[v].membership;
}

jla_boost::GraphDPO::Membership membership(const LabelledRule &r, const Edge &e) {
	return get_graph(r)[e].membership;
}

void put_membership(LabelledRule &r, const Vertex &v, jla_boost::GraphDPO::Membership m) {
	get_graph(r)[v].membership = m;
}

void put_membership(LabelledRule &r, const Edge &e, jla_boost::GraphDPO::Membership m) {
	get_graph(r)[e].membership = m;
}

LabelledRule::LabelledLeftType get_labelled_left(const LabelledRule &r) {
	return LabelledRule::LabelledLeftType(r);
}

LabelledRule::LabelledRightType get_labelled_right(const LabelledRule &r) {
	return LabelledRule::LabelledRightType(r);
}

LabelledRule::Projections::Projections(const LabelledRule &r)
: left(get_graph(r), Membership::Left),
context(get_graph(r), Membership::Context),
right(get_graph(r), Membership::Right) { }

// LabelledSideGraph
//------------------------------------------------------------------------------

namespace detail {

LabelledSideGraph::LabelledSideGraph(const LabelledRule &r, jla_boost::GraphDPO::Membership m)
: r(r), m(m) { }

} // namespace detail

// LabelledLeftGraph
//------------------------------------------------------------------------------

LabelledLeftGraph::LabelledLeftGraph(const LabelledRule &r)
: Base(r, jla_boost::GraphDPO::Membership::Left) { }

const LabelledLeftGraph::GraphType &get_graph(const LabelledLeftGraph &g) {
	return get_left(g.r);
}

LabelledLeftGraph::PropStringType get_string(const LabelledLeftGraph &g) {
	return get_string(g.r).getLeft();
}

const std::vector<std::unique_ptr<LabelledRule::LeftMatchConstraint> > &
get_match_constraints(const LabelledLeftGraph &g) {
	return g.r.leftComponentMatchConstraints;
}

std::size_t get_num_connected_components(const LabelledLeftGraph &g) {
	return g.r.numLeftComponents;
}

LabelledLeftGraph::Base::ComponentGraph
get_component_graph(std::size_t i, const LabelledLeftGraph &g) {
	assert(i < get_num_connected_components(g));
	LabelledLeftGraph::Base::ComponentFilter filter(&get_graph(g), &g.r.leftComponents, i);
	return LabelledLeftGraph::Base::ComponentGraph(get_graph(g), filter, filter);
}

const std::vector<boost::graph_traits<GraphType>::vertex_descriptor>&
get_vertex_order_component(std::size_t i, const LabelledLeftGraph &g) {
	assert(i < get_num_connected_components(g));
	// the number of connected components is initialized externally after construction, so we have this annoying hax
	if(g.vertex_orders.empty()) g.vertex_orders.resize(get_num_connected_components(g));
	if(g.vertex_orders[i].empty()) {
		g.vertex_orders[i] = get_vertex_order(lib::GraphMorphism::DefaultFinderArgsProvider(), get_component_graph(i, g));
	}
	return g.vertex_orders[i];
}

// LabelledRightGraph
//------------------------------------------------------------------------------

LabelledRightGraph::LabelledRightGraph(const LabelledRule &r)
: Base(r, jla_boost::GraphDPO::Membership::Right) { }

const LabelledRightGraph::GraphType &get_graph(const LabelledRightGraph &g) {
	return get_right(g.r);
}

LabelledRightGraph::PropStringType get_string(const LabelledRightGraph &g) {
	return get_string(g.r).getRight();
}

const std::vector<std::unique_ptr<LabelledRule::RightMatchConstraint> > &
get_match_constraints(const LabelledRightGraph &g) {
	return g.r.rightComponentMatchConstraints;
}

std::size_t get_num_connected_components(const LabelledRightGraph &g) {
	return g.r.numRightComponents;
}

LabelledRightGraph::Base::ComponentGraph
get_component_graph(std::size_t i, const LabelledRightGraph &g) {
	assert(i < get_num_connected_components(g));
	LabelledRightGraph::Base::ComponentFilter filter(&get_graph(g), &g.r.rightComponents, i);
	return LabelledRightGraph::Base::ComponentGraph(get_graph(g), filter, filter);
}

const std::vector<boost::graph_traits<GraphType>::vertex_descriptor>&
get_vertex_order_component(std::size_t i, const LabelledRightGraph &g) {
	assert(i < get_num_connected_components(g));
	// the number of connected components is initialized externally after construction, so we have this annoying hax
	if(g.vertex_orders.empty()) g.vertex_orders.resize(get_num_connected_components(g));
	if(g.vertex_orders[i].empty()) {
		g.vertex_orders[i] = get_vertex_order(lib::GraphMorphism::DefaultFinderArgsProvider(), get_component_graph(i, g));
	}
	return g.vertex_orders[i];
}

} // namespace Rules
} // namespace lib
} // namespace mod
