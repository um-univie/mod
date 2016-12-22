#include "GraphGraphInterface.h"

#include <mod/lib/GraphPimpl.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/Graph/Properties/Molecule.h>
#include <mod/lib/Graph/Properties/String.h>

namespace mod {

//------------------------------------------------------------------------------
// Vertex
//------------------------------------------------------------------------------

MOD_GRAPHPIMPL_Define_Vertex(Graph, graph, g->getGraph().getGraph(), g, Graph)
MOD_GRAPHPIMPL_Define_Vertex_Undirected(Graph, graph, g->getGraph().getGraph(), g)

const std::string &Graph::Vertex::getStringLabel() const {
	if(!g) throw LogicError("Can not get string label on a null vertex.");
	const auto &graph = g->getGraph().getGraph();
	using boost::vertices;
	auto v = *(vertices(graph).first + vId);
	return g->getGraph().getStringState()[v];
}

AtomId Graph::Vertex::getAtomId() const {
	if(!g) throw LogicError("Can not get atom id on a null vertex.");
	const auto &graph = g->getGraph().getGraph();
	using boost::vertices;
	auto v = *(vertices(graph).first + vId);
	return g->getGraph().getMoleculeState()[v].getAtomId();
}

Charge Graph::Vertex::getCharge() const {
	if(!g) throw LogicError("Can not get charge on a null vertex.");
	const auto &graph = g->getGraph().getGraph();
	using boost::vertices;
	auto v = *(vertices(graph).first + vId);
	return g->getGraph().getMoleculeState()[v].getCharge();
}

bool Graph::Vertex::getRadical() const {
	if(!g) throw LogicError("Can not get radical status on a null vertex.");
	const auto &graph = g->getGraph().getGraph();
	using boost::vertices;
	auto v = *(vertices(graph).first + vId);
	return g->getGraph().getMoleculeState()[v].getRadical();
}

//------------------------------------------------------------------------------
// Edge
//------------------------------------------------------------------------------

MOD_GRAPHPIMPL_Define_Indices(Graph, graph, g->getGraph().getGraph(), g, getGraph, Graph)

BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Graph::VertexIterator>));
BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Graph::EdgeIterator>));
BOOST_CONCEPT_ASSERT((boost::ForwardIterator<Graph::IncidentEdgeIterator>));

const std::string &Graph::Edge::getStringLabel() const {
	if(!g) throw LogicError("Can not get string label on a null edge.");
	const auto &graph = g->getGraph().getGraph();
	using boost::vertices;
	auto v = *(vertices(graph).first + vId);
	auto e = *(out_edges(v, graph).first + eId);
	return g->getGraph().getStringState()[e];
}

BondType Graph::Edge::getBondType() const {
	if(!g) throw LogicError("Can not get bond type on a null edge.");
	const auto &graph = g->getGraph().getGraph();
	using boost::vertices;
	auto v = *(vertices(graph).first + vId);
	auto e = *(out_edges(v, graph).first + eId);
	return g->getGraph().getMoleculeState()[e];
}

} // namespace mod