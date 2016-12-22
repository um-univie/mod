#ifndef MOD_LIB_GRAPH_LABELLED_GRAPH_H
#define MOD_LIB_GRAPH_LABELLED_GRAPH_H

#include <mod/lib/Graph/GraphDecl.h>

namespace mod {
namespace lib {
namespace Graph {
struct PropMolecule;
struct PropString;

struct LabelledGraph { // models a mod::lib::LabelledGraphConcept
	using GraphType = lib::Graph::GraphType;
	using PropStringType = PropString;
public:
	using PropMoleculeType = PropMolecule;
public:
	LabelledGraph(std::unique_ptr<GraphType> g, std::unique_ptr<PropStringType> pString);
	LabelledGraph(const LabelledGraph &other);
	~LabelledGraph();
	friend GraphType &get_graph(LabelledGraph &g);
	friend const GraphType &get_graph(const LabelledGraph &g);
	friend PropStringType &get_string(LabelledGraph &g);
	friend const PropStringType &get_string(const LabelledGraph &g);
	friend const PropMoleculeType &get_molecule(const LabelledGraph &g);
public:
	friend const std::vector<typename boost::graph_traits<GraphType>::vertex_descriptor>&
	get_vertex_order(const LabelledGraph &g);
private: // intrinsic data
	std::unique_ptr<GraphType> g;
	mutable std::unique_ptr<PropStringType> pString;
private: // views
	mutable std::unique_ptr<PropMoleculeType> pMolecule;
private: // optimisation
	mutable std::vector<typename boost::graph_traits<GraphType>::vertex_descriptor> vertex_order;
};

} // namespace Graph
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_GRAPH_LABELLED_GRAPH_H */
