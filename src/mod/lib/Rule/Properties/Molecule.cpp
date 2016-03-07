#include "Molecule.h"

#include <mod/Error.h>
#include <mod/lib/Chem/MoleculeUtil.h>
#include <mod/lib/IO/IO.h>
#include <mod/lib/Rule/Properties/String.h>

#include <jla_boost/graph/PairToRangeAdaptor.hpp>

namespace mod {
namespace lib {
namespace Rule {

PropMoleculeCore::PropMoleculeCore(const GraphType &g, const PropStringCore &labelState) : Base(g), isReaction(true) {
	vertexState.resize(num_vertices(g));
	for(Vertex v : asRange(vertices(g))) {
		auto decodeLabel = [this](const std::string & label) {
			auto p = Chem::decodeVertexLabel(label);
			if(p.first == AtomIds::Invalid) isReaction = false;
			return AtomData(p.first, p.second);
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
	for(Edge e : asRange(edges(g))) {
		auto decodeLabel = [this](const std::string & label) {
			auto bondType = Chem::decodeEdgeLabel(label);
			if(bondType == BondType::Invalid) isReaction = false;
			return bondType;
		};
		switch(g[e].membership) {
		case Membership::Left:
			edgeState.add(e, EdgeStore(decodeLabel(labelState.getLeft()[e]), BondType()));
			break;
		case Membership::Right:
			edgeState.add(e, EdgeStore(BondType(), decodeLabel(labelState.getRight()[e])));
			break;
		case Membership::Context:
			edgeState.add(e, EdgeStore(
					decodeLabel(labelState.getLeft()[e]),
					decodeLabel(labelState.getRight()[e])
					));
			break;
		}
	}
}

} // namespace Rule
} // namespace lib
} // namespace mod