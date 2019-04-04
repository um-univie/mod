#include "Molecule.h"

#include <mod/Error.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/Rules/Properties/String.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

namespace mod {
namespace lib {
namespace Rules {

PropMoleculeCore::PropMoleculeCore(const GraphType &g, const PropStringCore &labelState) : Base(g), isReaction(true) {
	vertexState.resize(num_vertices(g));
	for(Vertex v : asRange(vertices(g))) {
		auto decodeLabel = [this](const std::string & label) {
			auto p = Chem::decodeVertexLabel(label);
			if(std::get<0>(p) == AtomIds::Invalid) isReaction = false;
			return AtomData(std::get<0>(p), std::get<1>(p), std::get<2>(p), std::get<3>(p));
		};
		auto vId = get(boost::vertex_index_t(), g, v);
		switch(g[v].membership) {
		case Membership::Left:
			vertexState[vId].left = decodeLabel(labelState.getLeft()[v]);
			break;
		case Membership::Right:
			vertexState[vId].right = decodeLabel(labelState.getRight()[v]);
			break;
		case Membership::Context:
			vertexState[vId].left = decodeLabel(labelState.getLeft()[v]);
			vertexState[vId].right = decodeLabel(labelState.getRight()[v]);
			break;
		}
	}

	edgeState.resize(num_edges(g));
	for(Edge e : asRange(edges(g))) {
		auto decodeLabel = [this](const std::string & label) {
			auto bondType = Chem::decodeEdgeLabel(label);
			if(bondType == BondType::Invalid) isReaction = false;
			return bondType;
		};
		auto eId = get(boost::edge_index_t(), g, e);
		switch(g[e].membership) {
		case Membership::Left:
			edgeState[eId].left = decodeLabel(labelState.getLeft()[e]);
			break;
		case Membership::Right:
			edgeState[eId].right = decodeLabel(labelState.getRight()[e]);
			break;
		case Membership::Context:
			edgeState[eId].left = decodeLabel(labelState.getLeft()[e]);
			edgeState[eId].right = decodeLabel(labelState.getRight()[e]);
			break;
		}
	}
}

} // namespace Rules
} // namespace lib
} // namespace mod