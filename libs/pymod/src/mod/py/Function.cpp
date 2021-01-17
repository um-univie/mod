#include <mod/py/Common.hpp>

#include "Function.hpp"

#include <mod/Derivation.hpp>
#include <mod/graph/Graph.hpp>
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
	exportFunc<std::string(dg::DG::Vertex)>("_Func_StringDGVertex");
	// DG::HyperEdge -> X
	exportFunc<bool(dg::DG::HyperEdge)>("_Func_BoolDGHyperEdge");
	exportFunc<std::string(dg::DG::HyperEdge)>("_Func_StringDGHyperEdge");
	exportFunc<double(dg::DG::HyperEdge)>("_Func_DoubleDGHyperEdge");
	// Graph -> X
	exportFunc<bool(std::shared_ptr<graph::Graph>)>("_Func_BoolGraph");
	exportFunc<int(std::shared_ptr<graph::Graph>)>("_Func_IntGraph");
	exportFunc<std::string(std::shared_ptr<graph::Graph>)>("_Func_StringGraph");
	// Graph x DG -> X
	exportFunc<std::string(std::shared_ptr<graph::Graph>, std::shared_ptr<dg::DG>)>("_Func_StringGraphDG");
	// Graph x DG x bool -> X
	exportFunc<std::string(std::shared_ptr<graph::Graph>, std::shared_ptr<dg::DG>, bool)>("_Func_StringGraphDGBool");
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
}

} // namespace mod::Py
