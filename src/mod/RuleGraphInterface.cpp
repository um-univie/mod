#include "RuleGraphInterface.h"

#include <mod/Chem.h>
#include <mod/lib/GraphPimpl.h>
#include <mod/lib/Rules/Real.h>
#include <mod/lib/Rules/Properties/Molecule.h>
//#include <mod/lib/Graph/Properties/String.h>

namespace mod {

//==============================================================================
// Left
//==============================================================================

std::size_t Rule::LeftGraph::numVertices() const {
	using boost::vertices;
	const auto &graph = get_left(r->getRule().getDPORule());
	const auto &vs = vertices(graph);
	return std::distance(vs.first, vs.second);
}

std::size_t Rule::LeftGraph::numEdges() const {
	using boost::edges;
	const auto &graph = get_left(r->getRule().getDPORule());
	const auto &es = edges(graph);
	return std::distance(es.first, es.second);
}

//------------------------------------------------------------------------------
// Vertex
//------------------------------------------------------------------------------

MOD_GRAPHPIMPL_Define_Vertex(Rule::LeftGraph, rule, get_left(r->getRule().getDPORule()), r, Rule)
MOD_GRAPHPIMPL_Define_Vertex_Undirected(Rule::LeftGraph, rule, get_left(r->getRule().getDPORule()), r)

Rule::Vertex Rule::LeftGraph::Vertex::getCore() const {
	if(isNull()) return Rule::Vertex();
	const auto &graph = get_left(r->getRule().getDPORule());
	using boost::vertices;
	auto v = *std::next(vertices(graph).first, vId);
	auto vCoreId = get(boost::vertex_index_t(), graph, v);
	return Rule::Vertex(r, vCoreId);
}

const std::string &Rule::LeftGraph::Vertex::getStringLabel() const {
	if(!r) throw LogicError("Can not get string label on a null vertex.");
	const auto &graph = get_left(r->getRule().getDPORule());
	using boost::vertices;
	auto v = *std::next(vertices(graph).first, vId);
	return r->getRule().getStringState().getLeft()[v];
}

AtomId Rule::LeftGraph::Vertex::getAtomId() const {
	if(!r) throw LogicError("Can not get atom id on a null vertex.");
	const auto &graph = get_left(r->getRule().getDPORule());
	using boost::vertices;
	auto v = *std::next(vertices(graph).first, vId);
	return r->getRule().getMoleculeState().getLeft()[v].getAtomId();
}

Charge Rule::LeftGraph::Vertex::getCharge() const {
	if(!r) throw LogicError("Can not get charge on a null vertex.");
	const auto &graph = get_left(r->getRule().getDPORule());
	using boost::vertices;
	auto iter = vertices(graph).first;
	std::advance(iter, vId);
	auto v = *iter;
	return r->getRule().getMoleculeState().getLeft()[v].getCharge();
}

bool Rule::LeftGraph::Vertex::getRadical() const {
	if(!r) throw LogicError("Can not get radical status on a null vertex.");
	const auto &graph = get_left(r->getRule().getDPORule());
	using boost::vertices;
	auto v = *std::next(vertices(graph).first, vId);
	return r->getRule().getMoleculeState().getLeft()[v].getRadical();
}

//------------------------------------------------------------------------------
// Edge
//------------------------------------------------------------------------------

MOD_GRAPHPIMPL_Define_Indices(Rule::LeftGraph, rule, get_left(r->getRule().getDPORule()), r, getRule, Rule)

BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Rule::LeftGraph::VertexIterator>));
BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Rule::LeftGraph::EdgeIterator>));
BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Rule::LeftGraph::IncidentEdgeIterator>));

Rule::Edge Rule::LeftGraph::Edge::getCore() const {
	if(isNull()) return Rule::Edge();
	const auto &graph = get_left(r->getRule().getDPORule());
	using boost::vertices;
	auto v = *std::next(vertices(graph).first, vId);
	auto vCoreId = get(boost::vertex_index_t(), graph, v);
	auto e = *std::next(out_edges(v, graph).first, eId);
	const auto &es = out_edges(v, get_graph(r->getRule().getDPORule()));
	auto eCoreId = std::distance(es.first, std::find(es.first, es.second, e));
	return Rule::Edge(r, vCoreId, eCoreId);
}

const std::string &Rule::LeftGraph::Edge::getStringLabel() const {
	if(!r) throw LogicError("Can not get string label on a null edge.");
	const auto &graph = get_left(r->getRule().getDPORule());
	using boost::vertices;
	auto v = *std::next(vertices(graph).first, vId);
	auto e = *std::next(out_edges(v, graph).first, eId);
	return r->getRule().getStringState().getLeft()[e];
}

BondType Rule::LeftGraph::Edge::getBondType() const {
	if(!r) throw LogicError("Can not get bond type on a null edge.");
	const auto &graph = get_left(r->getRule().getDPORule());
	using boost::vertices;
	auto v = *std::next(vertices(graph).first, vId);
	auto e = *std::next(out_edges(v, graph).first, eId);
	return r->getRule().getMoleculeState().getLeft()[e];
}


//==============================================================================
// Context
//==============================================================================

std::size_t Rule::ContextGraph::numVertices() const {
	using boost::vertices;
	const auto &graph = get_context(r->getRule().getDPORule());
	const auto &vs = vertices(graph);
	return std::distance(vs.first, vs.second);
}

std::size_t Rule::ContextGraph::numEdges() const {
	using boost::edges;
	const auto &graph = get_context(r->getRule().getDPORule());
	const auto &es = edges(graph);
	return std::distance(es.first, es.second);
}

//------------------------------------------------------------------------------
// Vertex
//------------------------------------------------------------------------------

MOD_GRAPHPIMPL_Define_Vertex(Rule::ContextGraph, rule, get_context(r->getRule().getDPORule()), r, Rule)
MOD_GRAPHPIMPL_Define_Vertex_Undirected(Rule::ContextGraph, rule, get_context(r->getRule().getDPORule()), r)

Rule::Vertex Rule::ContextGraph::Vertex::getCore() const {
	if(isNull()) return Rule::Vertex();
	const auto &graph = get_context(r->getRule().getDPORule());
	using boost::vertices;
	auto v = *std::next(vertices(graph).first, vId);
	auto vCoreId = get(boost::vertex_index_t(), graph, v);
	return Rule::Vertex(r, vCoreId);
}

//------------------------------------------------------------------------------
// Edge
//------------------------------------------------------------------------------

MOD_GRAPHPIMPL_Define_Indices(Rule::ContextGraph, rule, get_context(r->getRule().getDPORule()), r, getRule, Rule)

BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Rule::ContextGraph::VertexIterator>));
BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Rule::ContextGraph::EdgeIterator>));
BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Rule::ContextGraph::IncidentEdgeIterator>));

Rule::Edge Rule::ContextGraph::Edge::getCore() const {
	if(isNull()) return Rule::Edge();
	const auto &graph = get_context(r->getRule().getDPORule());
	using boost::vertices;
	auto v = *std::next(vertices(graph).first, vId);
	auto vCoreId = get(boost::vertex_index_t(), graph, v);
	auto e = *std::next(out_edges(v, graph).first, eId);
	const auto &es = out_edges(v, get_graph(r->getRule().getDPORule()));
	auto eCoreId = std::distance(es.first, std::find(es.first, es.second, e));
	return Rule::Edge(r, vCoreId, eCoreId);
}

//==============================================================================
// Right
//==============================================================================

std::size_t Rule::RightGraph::numVertices() const {
	using boost::vertices;
	const auto &graph = get_right(r->getRule().getDPORule());
	const auto &vs = vertices(graph);
	return std::distance(vs.first, vs.second);
}

std::size_t Rule::RightGraph::numEdges() const {
	using boost::edges;
	const auto &graph = get_right(r->getRule().getDPORule());
	const auto &es = edges(graph);
	return std::distance(es.first, es.second);
}

//------------------------------------------------------------------------------
// Vertex
//------------------------------------------------------------------------------

MOD_GRAPHPIMPL_Define_Vertex(Rule::RightGraph, rule, get_right(r->getRule().getDPORule()), r, Rule)
MOD_GRAPHPIMPL_Define_Vertex_Undirected(Rule::RightGraph, rule, get_right(r->getRule().getDPORule()), r)

Rule::Vertex Rule::RightGraph::Vertex::getCore() const {
	if(isNull()) return Rule::Vertex();
		const auto &graph = get_right(r->getRule().getDPORule());
	using boost::vertices;
	auto v = *std::next(vertices(graph).first, vId);
	auto vCoreId = get(boost::vertex_index_t(), graph, v);
	return Rule::Vertex(r, vCoreId);
}

const std::string &Rule::RightGraph::Vertex::getStringLabel() const {
	if(!r) throw LogicError("Can not get string label on a null vertex.");
	const auto &graph = get_right(r->getRule().getDPORule());
	using boost::vertices;
	auto v = *std::next(vertices(graph).first, vId);
	return r->getRule().getStringState().getRight()[v];
}

AtomId Rule::RightGraph::Vertex::getAtomId() const {
	if(!r) throw LogicError("Can not get atom id on a null vertex.");
	const auto &graph = get_right(r->getRule().getDPORule());
	using boost::vertices;
	auto v = *std::next(vertices(graph).first, vId);
	return r->getRule().getMoleculeState().getRight()[v].getAtomId();
}

Charge Rule::RightGraph::Vertex::getCharge() const {
	if(!r) throw LogicError("Can not get charge on a null vertex.");
	const auto &graph = get_right(r->getRule().getDPORule());
	using boost::vertices;
	auto v = *std::next(vertices(graph).first, vId);
	return r->getRule().getMoleculeState().getRight()[v].getCharge();
}

bool Rule::RightGraph::Vertex::getRadical() const {
	if(!r) throw LogicError("Can not get radical status on a null vertex.");
	const auto &graph = get_right(r->getRule().getDPORule());
	using boost::vertices;
	auto v = *std::next(vertices(graph).first, vId);
	return r->getRule().getMoleculeState().getRight()[v].getRadical();
}

//------------------------------------------------------------------------------
// Edge
//------------------------------------------------------------------------------

MOD_GRAPHPIMPL_Define_Indices(Rule::RightGraph, rule, get_right(r->getRule().getDPORule()), r, getRule, Rule)

BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Rule::RightGraph::VertexIterator>));
BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Rule::RightGraph::EdgeIterator>));
BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Rule::RightGraph::IncidentEdgeIterator>));

Rule::Edge Rule::RightGraph::Edge::getCore() const {
	if(isNull()) return Rule::Edge();
	const auto &graph = get_right(r->getRule().getDPORule());
	using boost::vertices;
	auto v = *std::next(vertices(graph).first, vId);
	auto vCoreId = get(boost::vertex_index_t(), graph, v);
	auto e = *std::next(out_edges(v, graph).first, eId);
	const auto &es = out_edges(v, get_graph(r->getRule().getDPORule()));
	auto eCoreId = std::distance(es.first, std::find(es.first, es.second, e));
	return Rule::Edge(r, vCoreId, eCoreId);
}

const std::string &Rule::RightGraph::Edge::getStringLabel() const {
	if(!r) throw LogicError("Can not get string label on a null edge.");
	const auto &graph = get_right(r->getRule().getDPORule());
	using boost::vertices;
	auto iter = vertices(graph).first;
	std::advance(iter, vId);
	auto v = *iter;
	auto e = *std::next(out_edges(v, graph).first, eId);
	return r->getRule().getStringState().getRight()[e];
}

BondType Rule::RightGraph::Edge::getBondType() const {
	if(!r) throw LogicError("Can not get bond type on a null edge.");
	const auto &graph = get_right(r->getRule().getDPORule());
	using boost::vertices;
	auto iter = vertices(graph).first;
	std::advance(iter, vId);
	auto v = *iter;
	auto e = *std::next(out_edges(v, graph).first, eId);
	return r->getRule().getMoleculeState().getRight()[e];
}


//==============================================================================
// Core
//==============================================================================

//------------------------------------------------------------------------------
// Vertex
//------------------------------------------------------------------------------

MOD_GRAPHPIMPL_Define_Vertex(Rule, rule, r->getRule().getGraph(), r, Rule)
MOD_GRAPHPIMPL_Define_Vertex_Undirected(Rule, rule, r->getRule().getGraph(), r)

Rule::LeftGraph::Vertex Rule::Vertex::getLeft() const {
	if(isNull()) return LeftGraph::Vertex();
	const auto &dpoRule = r->getRule().getDPORule();
	const auto v = vertex(vId, get_graph(dpoRule));
	if(membership(dpoRule, v) != lib::Rules::Membership::Right) {
		using boost::vertices;
		const auto &vs = vertices(get_left(r->getRule().getDPORule()));
		auto iter = std::find(vs.first, vs.second, v);
		auto vSubId = std::distance(vs.first, iter);
		return LeftGraph::Vertex(r, vSubId);
	} else return LeftGraph::Vertex();
}

Rule::ContextGraph::Vertex Rule::Vertex::getContext() const {
	if(isNull()) return ContextGraph::Vertex();
	const auto &dpoRule = r->getRule().getDPORule();
	const auto v = vertex(vId, get_graph(dpoRule));
	if(membership(dpoRule, v) == lib::Rules::Membership::Context) {
		using boost::vertices;
		const auto &vs = vertices(get_context(r->getRule().getDPORule()));
		auto iter = std::find(vs.first, vs.second, v);
		auto vSubId = std::distance(vs.first, iter);
		return ContextGraph::Vertex(r, vSubId);
	} else return ContextGraph::Vertex();
}

Rule::RightGraph::Vertex Rule::Vertex::getRight() const {
	if(isNull()) return RightGraph::Vertex();
	const auto &dpoRule = r->getRule().getDPORule();
	const auto v = vertex(vId, get_graph(dpoRule));
	if(membership(dpoRule, v) != lib::Rules::Membership::Left) {
		using boost::vertices;
		const auto &vs = vertices(get_right(r->getRule().getDPORule()));
		auto iter = std::find(vs.first, vs.second, v);
		auto vSubId = std::distance(vs.first, iter);
		return RightGraph::Vertex(r, vSubId);
	} else return RightGraph::Vertex();
}

//------------------------------------------------------------------------------
// Edge
//------------------------------------------------------------------------------

MOD_GRAPHPIMPL_Define_Indices(Rule, rule, r->getRule().getGraph(), r, getRule, Rule)

BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Rule::VertexIterator>));
BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Rule::EdgeIterator>));
BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Rule::IncidentEdgeIterator>));

Rule::LeftGraph::Edge Rule::Edge::getLeft() const {
	if(isNull()) return LeftGraph::Edge();
	const auto &dpoRule = r->getRule().getDPORule();
	const auto &graph = get_graph(dpoRule);
	using boost::vertices;
	const auto v = *(vertices(graph).first + vId);
	const auto e = *(out_edges(v, graph).first + eId);
	if(membership(dpoRule, e) != lib::Rules::Membership::Right) {
		const auto &vs = vertices(get_left(r->getRule().getDPORule()));
		auto iter = std::find(vs.first, vs.second, v);
		auto vSubId = std::distance(vs.first, iter);
		const auto &es = out_edges(v, get_left(r->getRule().getDPORule()));
		auto eIter = std::find(es.first, es.second, e);
		auto eSubId = std::distance(es.first, eIter);
		return LeftGraph::Edge(r, vSubId, eSubId);
	} else return LeftGraph::Edge();
}

Rule::ContextGraph::Edge Rule::Edge::getContext() const {
	if(isNull()) return ContextGraph::Edge();
	const auto &dpoRule = r->getRule().getDPORule();
	const auto &graph = get_graph(dpoRule);
	using boost::vertices;
	const auto v = *(vertices(graph).first + vId);
	const auto e = *(out_edges(v, graph).first + eId);
	if(membership(dpoRule, e) == lib::Rules::Membership::Context) {
		const auto &vs = vertices(get_context(r->getRule().getDPORule()));
		auto iter = std::find(vs.first, vs.second, v);
		auto vSubId = std::distance(vs.first, iter);
		const auto &es = out_edges(v, get_context(r->getRule().getDPORule()));
		auto eIter = std::find(es.first, es.second, e);
		auto eSubId = std::distance(es.first, eIter);
		return ContextGraph::Edge(r, vSubId, eSubId);
	} else return ContextGraph::Edge();
}

Rule::RightGraph::Edge Rule::Edge::getRight() const {
	if(isNull()) return RightGraph::Edge();
	const auto &dpoRule = r->getRule().getDPORule();
	const auto &graph = get_graph(dpoRule);
	using boost::vertices;
	const auto v = *(vertices(graph).first + vId);
	const auto e = *(out_edges(v, graph).first + eId);
	if(membership(dpoRule, e) != lib::Rules::Membership::Left) {
		const auto &vs = vertices(get_right(r->getRule().getDPORule()));
		auto iter = std::find(vs.first, vs.second, v);
		auto vSubId = std::distance(vs.first, iter);
		const auto &es = out_edges(v, get_right(r->getRule().getDPORule()));
		auto eIter = std::find(es.first, es.second, e);
		auto eSubId = std::distance(es.first, eIter);
		return RightGraph::Edge(r, vSubId, eSubId);
	} else return RightGraph::Edge();
}

} // namespace mod