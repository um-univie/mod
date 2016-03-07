#include <mod/Py/Common.h>

#include "Function.h"

#include <mod/Derivation.h>
#include <mod/DG.h>
#include <mod/DGStrat.h>

namespace mod {
class Graph;
namespace Py {

void Function_doExport() {
	// () -> X
	exportFunc < std::vector<std::shared_ptr<mod::Graph> >()>("Func_VecGraph");
	exportFunc < std::string()>("Func_String");
	// Derivation -> X
	exportFunc<bool(const Derivation&) >("Func_BoolDerivation");
	exportFunc < std::string(const Derivation&)>("Func_StringDerivation");
	// DerivationRef -> X
	exportFunc<bool(DerivationRef)>("Func_BoolDerivationRef");
	exportFunc < std::string(DerivationRef)>("Func_StringDerivationRef");
	// Graph -> X
	exportFunc<bool(std::shared_ptr<mod::Graph>)>("Func_BoolGraph");
	exportFunc < std::string(std::shared_ptr<mod::Graph>)>("Func_StringGraph");
	// Graph x DG -> X
	exportFunc < bool(std::shared_ptr<mod::Graph>, std::shared_ptr<mod::DG>)>("Func_BoolGraphDG");
	exportFunc < std::string(std::shared_ptr<mod::Graph>, std::shared_ptr<mod::DG>)>("Func_StringGraphDG");
	// Graph x DG x bool -> X
	exportFunc < std::string(std::shared_ptr<Graph>, std::shared_ptr<DG>, bool)>("Func_StringGraphDGBool");
	// Graph x DGStrat::GraphState -> X
	exportFunc<bool(std::shared_ptr<mod::Graph>, const mod::DGStrat::GraphState&)>("Func_BoolGraphDGStratGraphState");
	// Graph x DGStrat::GraphState x bool -> X
	exportFunc<bool(std::shared_ptr<mod::Graph>, const mod::DGStrat::GraphState&, bool)>("Func_BoolGraphDGStratGraphStateBool");
	// Graph x Graph x DGStrat::GraphState -> X
	exportFunc<bool(std::shared_ptr<mod::Graph>, std::shared_ptr<mod::Graph>, const mod::DGStrat::GraphState&)>("Func_BoolGraphGraphDGStratGraphState");
	// DGStrat::GraphState -> X
	exportFunc<void(const mod::DGStrat::GraphState&)>("Func_VoidDGStratGraphState");
}

} // namespace Py
} // namespace mod