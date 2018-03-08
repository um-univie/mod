#include <mod/Py/Common.h>

#include "Function.h"

#include <mod/Derivation.h>
#include <mod/graph/Graph.h>
#include <mod/dg/DG.h>
#include <mod/dg/GraphInterface.h>
#include <mod/dg/Strategies.h>

namespace mod {
namespace Py {

void Function_doExport() {
	// () -> X
	exportFunc < std::vector<std::shared_ptr<graph::Graph> >()>("Func_VecGraph");
	exportFunc < std::string()>("Func_String");
	// Derivation -> X
	exportFunc<bool(const Derivation&) >("Func_BoolDerivation");
	exportFunc < std::string(const Derivation&)>("Func_StringDerivation");
	// DG::HyperEdge -> X
	exportFunc<bool(dg::DG::HyperEdge)>("Func_BoolDGHyperEdge");
	exportFunc < std::string(dg::DG::HyperEdge)>("Func_StringDGHyperEdge");
	// Graph -> X
	exportFunc<bool(std::shared_ptr<graph::Graph>)>("Func_BoolGraph");
	exportFunc < std::string(std::shared_ptr<graph::Graph>)>("Func_StringGraph");
	// Graph x DG -> X
	exportFunc < bool(std::shared_ptr<graph::Graph>, std::shared_ptr<dg::DG>)>("Func_BoolGraphDG");
	exportFunc < std::string(std::shared_ptr<graph::Graph>, std::shared_ptr<dg::DG>)>("Func_StringGraphDG");
	// Graph x DG x bool -> X
	exportFunc < std::string(std::shared_ptr<graph::Graph>, std::shared_ptr<dg::DG>, bool)>("Func_StringGraphDGBool");
	// Graph x Strategy::GraphState -> X
	exportFunc<bool(std::shared_ptr<graph::Graph>, const dg::Strategy::GraphState&)>("Func_BoolGraphDGStratGraphState");
	// Graph x Strategy::GraphState x bool -> X
	exportFunc<bool(std::shared_ptr<graph::Graph>, const dg::Strategy::GraphState&, bool)>("Func_BoolGraphDGStratGraphStateBool");
	// Graph x Graph x Strategy::GraphState -> X
	exportFunc<bool(std::shared_ptr<graph::Graph>, std::shared_ptr<graph::Graph>, const dg::Strategy::GraphState&)>("Func_BoolGraphGraphDGStratGraphState");
	// Strategy::GraphState -> X
	exportFunc<void(const dg::Strategy::GraphState&)>("Func_VoidDGStratGraphState");
}

} // namespace Py
} // namespace mod