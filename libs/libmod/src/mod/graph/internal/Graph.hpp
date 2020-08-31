#ifndef MOD_GRAPH_INTERNAL_GRAPH_HPP
#define MOD_GRAPH_INTERNAL_GRAPH_HPP

#include <mod/BuildConfig.hpp>
#include <mod/graph/internal/ForwardDecl.hpp>
#include <mod/lib/LabelledUnionGraph.hpp>
#include <mod/lib/Graph/LabelledGraph.hpp>

namespace mod {
namespace graph {
namespace internal {

// Graph
MOD_DECL const lib::Graph::LabelledGraph &getLabelledGraph(const lib::Graph::Single &g);

// LabelledUnionGraph<LablledGraph>
MOD_DECL void push_back(lib::LabelledUnionGraph<lib::Graph::LabelledGraph> &ug, const lib::Graph::LabelledGraph *g);

MOD_DECL const lib::LabelledUnionGraph<lib::Graph::LabelledGraph>::GraphType &
getGraph(const lib::LabelledUnionGraph<lib::Graph::LabelledGraph> &g);

MOD_DECL const std::string &getString(
		boost::graph_traits<lib::LabelledUnionGraph<lib::Graph::LabelledGraph>::GraphType>::vertex_descriptor v,
		const lib::LabelledUnionGraph<lib::Graph::LabelledGraph> &g);
MOD_DECL const std::string &getString(
		boost::graph_traits<lib::LabelledUnionGraph<lib::Graph::LabelledGraph>::GraphType>::edge_descriptor e,
		const lib::LabelledUnionGraph<lib::Graph::LabelledGraph> &g);

MOD_DECL const AtomData &getMolecule(
		boost::graph_traits<lib::LabelledUnionGraph<lib::Graph::LabelledGraph>::GraphType>::vertex_descriptor v,
		const lib::LabelledUnionGraph<lib::Graph::LabelledGraph> &g);
MOD_DECL BondType getMolecule(
		boost::graph_traits<lib::LabelledUnionGraph<lib::Graph::LabelledGraph>::GraphType>::edge_descriptor e,
		const lib::LabelledUnionGraph<lib::Graph::LabelledGraph> &g);

} // namespace internal
} // namespace graph
} // namespace mod

#endif // MOD_GRAPH_INTERNAL_GRAPH_HPP