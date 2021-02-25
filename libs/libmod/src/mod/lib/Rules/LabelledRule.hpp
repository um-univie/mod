#ifndef MOD_LIB_RULES_LABELLED_RULE_HPP
#define MOD_LIB_RULES_LABELLED_RULE_HPP

#include <mod/lib/GraphMorphism/Constraints/Constraint.hpp>
#include <mod/lib/Rules/ConnectedComponent.hpp>
#include <mod/lib/Rules/GraphDecl.hpp>
#include <mod/lib/Rules/Properties/Molecule.hpp>
#include <mod/lib/Rules/Properties/Stereo.hpp>
#include <mod/lib/Rules/Properties/String.hpp>
#include <mod/lib/Rules/Properties/Term.hpp>
#include <mod/lib/Graph/LabelledGraph.hpp>

#include <vector>

namespace mod::lib::Rules {
struct LabelledLeftGraph;
struct LabelledRightGraph;

class LabelledRule {
public: // LabelledGraphConcept, PushoutRuleConcept
	using GraphType = lib::Rules::GraphType;
public: // PushoutRuleConcept
	using LeftGraphType = lib::Rules::SideGraphType;
	using ContextGraphType = lib::Rules::SideGraphType;
	using RightGraphType = lib::Rules::SideGraphType;
public: // LabelledGraphConcept
	using PropStringType = PropStringCore;
	using PropTermType = PropTermCore;
	using PropStereoType = PropStereoCore;
	using PropMoleculeType = PropMoleculeCore;
public: // Other
	using Vertex = boost::graph_traits<GraphType>::vertex_descriptor;
	using Edge = boost::graph_traits<GraphType>::edge_descriptor;
	using LeftMatchConstraint = GraphMorphism::Constraints::Constraint<LeftGraphType>;
	using RightMatchConstraint = GraphMorphism::Constraints::Constraint<RightGraphType>;
	using LabelledLeftType = LabelledLeftGraph;
	using LabelledRightType = LabelledRightGraph;

	using ComponentGraph_v2 = Graph::LabelledGraph;
public:
	LabelledRule();
	LabelledRule(const LabelledRule &other, bool withConstraints);
	void initComponents(); // TODO: this is a huge hax
	void invert();
public: // LabelledGraphConcept, PushoutRuleConcept
	friend GraphType &get_graph(LabelledRule &r);
	friend const GraphType &get_graph(const LabelledRule &r);
public: // LabelledGraphConcept
	friend const PropStringType &get_string(const LabelledRule &r);
	friend const PropTermType &get_term(const LabelledRule &r);
	friend bool has_stereo(const LabelledRule &r);
	friend const PropStereoType &get_stereo(const LabelledRule &r);
public:
	friend const PropMoleculeType &get_molecule(const LabelledRule &r);
public: // PushoutRuleConcept
	friend const LeftGraphType &get_left(const LabelledRule &r);
	friend const ContextGraphType &get_context(const LabelledRule &r);
	friend const RightGraphType &get_right(const LabelledRule &r);
	friend jla_boost::GraphDPO::Membership membership(const LabelledRule &r, const Vertex &v);
	friend jla_boost::GraphDPO::Membership membership(const LabelledRule &r, const Edge &e);
	friend void put_membership(LabelledRule &r, const Vertex &v, jla_boost::GraphDPO::Membership m);
	friend void put_membership(LabelledRule &r, const Edge &e, jla_boost::GraphDPO::Membership m);
public:
	friend LabelledLeftType get_labelled_left(const LabelledRule &r);
	friend LabelledRightType get_labelled_right(const LabelledRule &r);
private:
	struct Projections {
		Projections(const LabelledRule &r);
	public:
		LeftGraphType left;
		ContextGraphType context;
		RightGraphType right;
	};
	std::unique_ptr<GraphType> g;
	mutable std::unique_ptr<Projections> projs;
public:
	mutable std::unique_ptr<PropStringType> pString;
	mutable std::unique_ptr<PropTermType> pTerm;
	mutable std::unique_ptr<PropStereoType> pStereo;
	std::vector<std::unique_ptr<LeftMatchConstraint> > leftMatchConstraints;
	std::vector<std::unique_ptr<RightMatchConstraint> > rightMatchConstraints;
private:
	mutable std::unique_ptr<PropMoleculeType> pMolecule;
public:
	std::size_t numLeftComponents = -1, numRightComponents = -1;
	std::vector<std::size_t> leftComponents, rightComponents;

public: // experimental new component graphs that does not use the filtered wrapper
	std::vector<ComponentGraph_v2> leftComponentGraphs, rightComponentGraphs;
	std::vector<std::vector<std::size_t>> leftComponentVertexToCoreVertex, rightComponentVertexToCoreVertex;
	std::vector<std::pair<std::size_t, std::size_t>> coreVertexToLeftComponentVertex, coreVertexToRightComponentVertex;
};

namespace detail {

struct LabelledSideGraph {
	using LabelledRule = lib::Rules::LabelledRule;
	using GraphType = lib::Rules::SideGraphType;
	using ComponentFilter = ConnectedComponentFilter<GraphType, std::vector<std::size_t> >;
	using ComponentGraph = boost::filtered_graph<GraphType, ComponentFilter, ComponentFilter>;
public:
	LabelledSideGraph(const LabelledRule &r, jla_boost::GraphDPO::Membership m);
public:
	const LabelledRule &r;
	const jla_boost::GraphDPO::Membership m;
protected:
	mutable std::vector<std::vector<boost::graph_traits<GraphType>::vertex_descriptor> > vertex_orders;

};

} // namespace detail

struct LabelledLeftGraph : detail::LabelledSideGraph {
	using Base = detail::LabelledSideGraph;
	using PropStringType = LabelledRule::PropStringType::LeftType;
	using PropTermType = LabelledRule::PropTermType::LeftType;
	using PropStereoType = LabelledRule::PropStereoType::LeftType;
public:
	using PropMoleculeType = typename LabelledRule::PropMoleculeType::LeftType;
public:
	explicit LabelledLeftGraph(const LabelledRule &r);
	friend const Base::GraphType &get_graph(const LabelledLeftGraph &g);
	friend PropStringType get_string(const LabelledLeftGraph &g);
	friend PropTermType get_term(const LabelledLeftGraph &g);
	friend bool has_stereo(const LabelledLeftGraph &g);
	friend PropStereoType get_stereo(const LabelledLeftGraph &g);
public:
	friend const std::vector<std::unique_ptr<LabelledRule::LeftMatchConstraint> > &
	get_match_constraints(const LabelledLeftGraph &g);
public:
	friend std::size_t get_num_connected_components(const LabelledLeftGraph &g);
	friend const std::vector<std::size_t>& get_connected_component_map(const LabelledLeftGraph& g);
	friend Base::ComponentGraph get_component_graph(std::size_t i, const LabelledLeftGraph &g);
	friend const LabelledRule::ComponentGraph_v2& get_component_graph_v2(std::size_t i, const LabelledLeftGraph &g);
	friend boost::graph_traits<SideGraphType>::vertex_descriptor get_component_core_vertex(std::size_t i, std::size_t v, const LabelledLeftGraph &g);
public:
	friend PropMoleculeType get_molecule(const LabelledLeftGraph &g);
public:
	friend const std::vector<boost::graph_traits<GraphType>::vertex_descriptor> &
	get_vertex_order_component(std::size_t i, const LabelledLeftGraph &g);
};

struct LabelledRightGraph : detail::LabelledSideGraph {
	using Base = detail::LabelledSideGraph;
	using PropStringType = LabelledRule::PropStringType::RightType;
	using PropTermType = LabelledRule::PropTermType::RightType;
	using PropStereoType = LabelledRule::PropStereoType::RightType;
public:
	using PropMoleculeType = typename LabelledRule::PropMoleculeType::RightType;
public:
	explicit LabelledRightGraph(const LabelledRule &r);
	friend const Base::GraphType &get_graph(const LabelledRightGraph &g);
	friend PropStringType get_string(const LabelledRightGraph &g);
	friend PropTermType get_term(const LabelledRightGraph &g);
	friend bool has_stereo(const LabelledRightGraph &g);
	friend PropStereoType get_stereo(const LabelledRightGraph &g);
public:
	friend const std::vector<std::unique_ptr<LabelledRule::RightMatchConstraint> > &
	get_match_constraints(const LabelledRightGraph &g);
public:
	friend std::size_t get_num_connected_components(const LabelledRightGraph &g);
	friend const std::vector<std::size_t>& get_connected_component_map(const LabelledRightGraph& g);
	friend Base::ComponentGraph get_component_graph(std::size_t i, const LabelledRightGraph &g);
	friend const LabelledRule::ComponentGraph_v2& get_component_graph_v2(std::size_t i, const LabelledRightGraph &g);
	friend boost::graph_traits<SideGraphType>::vertex_descriptor get_component_core_vertex(std::size_t i, std::size_t v, const LabelledRightGraph &g);
public:
	friend PropMoleculeType get_molecule(const LabelledRightGraph &g);
public:
	friend const std::vector<boost::graph_traits<GraphType>::vertex_descriptor> &
	get_vertex_order_component(std::size_t i, const LabelledRightGraph &g);
};

} // namespace mod::lib::Rules

#endif // MOD_LIB_RULES_LABELLED_RULE_HPP
