#include "Graph.hpp"

#include <mod/lib/Graph/Single.hpp>
#include <mod/lib/Graph/Properties/Molecule.hpp>
#include <mod/lib/Graph/Properties/String.hpp>

namespace mod {
namespace graph {
namespace internal {

const lib::Graph::LabelledGraph &getLabelledGraph(const lib::Graph::Single &g) {
	return g.getLabelledGraph();
}

void push_back(lib::LabelledUnionGraph<lib::Graph::LabelledGraph> &ug, const lib::Graph::LabelledGraph *g) {
	ug.push_back(g);
}

const lib::LabelledUnionGraph<lib::Graph::LabelledGraph>::GraphType &
getGraph(const lib::LabelledUnionGraph<lib::Graph::LabelledGraph> &g) {
	return get_graph(g);
}

const std::string &getString(
		boost::graph_traits<lib::LabelledUnionGraph<lib::Graph::LabelledGraph>::GraphType>::vertex_descriptor v,
		const lib::LabelledUnionGraph<lib::Graph::LabelledGraph> &g) {
	return get_string(g)[v];
}

const std::string &getString(
		boost::graph_traits<lib::LabelledUnionGraph<lib::Graph::LabelledGraph>::GraphType>::edge_descriptor e,
		const lib::LabelledUnionGraph<lib::Graph::LabelledGraph> &g) {
	return get_string(g)[e];
}

const AtomData &getMolecule(
		boost::graph_traits<lib::LabelledUnionGraph<lib::Graph::LabelledGraph>::GraphType>::vertex_descriptor v,
		const lib::LabelledUnionGraph<lib::Graph::LabelledGraph> &g) {
	return get_molecule(g)[v];
}

BondType getMolecule(
		boost::graph_traits<lib::LabelledUnionGraph<lib::Graph::LabelledGraph>::GraphType>::edge_descriptor e,
		const lib::LabelledUnionGraph<lib::Graph::LabelledGraph> &g) {
	return get_molecule(g)[e];
}

} // namespace internal
} // namespace graph
} // namespace mod