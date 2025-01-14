#include <mod/py/Function.hpp>

#include <mod/Derivation.hpp>
#include <mod/VertexMap.hpp>
#include <mod/graph/Graph.hpp>
#include <mod/graph/GraphInterface.hpp>
#include <mod/dg/DG.hpp>
#include <mod/dg/GraphInterface.hpp>
#include <mod/dg/Strategies.hpp>

namespace mod::Py {

void Function_doExport() {
	// () -> X
	exportFunc<std::vector<std::shared_ptr<graph::Graph> >()>("_Func_VecGraph");
	exportFunc<std::string()>("_Func_String");
	// Derivation -> X
	exportFunc<bool(const Derivation &)>("_Func_BoolDerivation");
	exportFunc<std::string(const Derivation &)>("_Func_StringDerivation");
	// DG::Vertex -> X
	exportFunc<bool(dg::DG::Vertex)>("_Func_BoolDGVertex");
	exportFunc<double(dg::DG::Vertex)>("_Func_DoubleDGVertex");
	exportFunc<std::string(dg::DG::Vertex)>("_Func_StringDGVertex");
	// DG::Vertex x int -> X
	exportFunc<std::pair<std::string, std::string>(dg::DG::Vertex, int)>(
			"_Func_PairStringStringDGVertexInt");
	// DG::HyperEdge -> X
	exportFunc<bool(dg::DG::HyperEdge)>("_Func_BoolDGHyperEdge");
	exportFunc<std::string(dg::DG::HyperEdge)>("_Func_StringDGHyperEdge");
	exportFunc<double(dg::DG::HyperEdge)>("_Func_DoubleDGHyperEdge");
	// Graph -> X
	exportFunc<bool(std::shared_ptr<graph::Graph>)>("_Func_BoolGraph");
	exportFunc<int(std::shared_ptr<graph::Graph>)>("_Func_IntGraph");
	exportFunc<std::string(std::shared_ptr<graph::Graph>)>("_Func_StringGraph");
	// Graph x Strategy::GraphState -> X
	exportFunc<bool(std::shared_ptr<graph::Graph>, const dg::Strategy::GraphState &)>(
			"_Func_BoolGraphDGStratGraphState");
	// Graph x Strategy::GraphState x bool -> X
	exportFunc<bool(std::shared_ptr<graph::Graph>, const dg::Strategy::GraphState &, bool)>(
			"_Func_BoolGraphDGStratGraphStateBool");
	// Graph x Graph x Strategy::GraphState -> X
	exportFunc<bool(std::shared_ptr<graph::Graph>, std::shared_ptr<graph::Graph>, const dg::Strategy::GraphState &)>(
			"_Func_BoolGraphGraphDGStratGraphState");
	// Strategy::GraphState -> X
	exportFunc<void(const dg::Strategy::GraphState &)>("_Func_VoidDGStratGraphState");

	exportFunc<bool(VertexMap<graph::Graph, graph::Graph>)>("_Func_BoolVertexMapGraphGraph");
}

} // namespace mod::Py
