#include <mod/Py/Common.h>

#include <mod/Derivation.h>
#include <mod/dg/DG.h>
#include <mod/dg/GraphInterface.h>
#include <mod/dg/Strategies.h>
#include <mod/graph/Graph.h>
#include <mod/rule/CompositionExpr.h>
#include <mod/rule/Rule.h>

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <vector>

namespace mod {
// This is just hax for the vector_indexing_suite, which requires == and !=.
// Currently the Boost sandbox has a version where the operators can be disabled.

bool operator==(const Derivation&, const Derivation&) {
	std::cerr << "Derivation does not support ==" << std::endl;
	std::exit(1);
}

namespace rule {
namespace RCExp {

bool operator==(const Expression&, const Expression&) {
	std::cerr << "RCExpExp does not support ==" << std::endl;
	std::exit(1);
}

} // namespace RCExp
} // namespace rule
} // namespace mod

namespace mod {
namespace Py {

void Collections_doExport() {
	// Vector
	using VecDerivation = std::vector<mod::Derivation>;
	py::class_<VecDerivation>("VecDerivation").def(py::vector_indexing_suite<VecDerivation, true>());
	using VecDGVertex = std::vector<dg::DG::Vertex>;
	py::class_<VecDGVertex>("VecDGVertex").def(py::vector_indexing_suite<VecDGVertex, true>());
	using VecDGHyperEdge = std::vector<dg::DG::HyperEdge>;
	py::class_<VecDGHyperEdge>("VecDGHyperEdge").def(py::vector_indexing_suite<VecDGHyperEdge, true>());
	using VecDGStrat = std::vector<std::shared_ptr<dg::Strategy> >;
	py::class_<VecDGStrat>("VecDGStrat").def(py::vector_indexing_suite<VecDGStrat, true>());
	using VecGraph = std::vector<std::shared_ptr<graph::Graph> >;
	py::class_<VecGraph>("VecGraph").def(py::vector_indexing_suite<VecGraph, true>());
	using VecRule = std::vector<std::shared_ptr<rule::Rule> >;
	py::class_<VecRule>("VecRule").def(py::vector_indexing_suite<VecRule, true>());
	using VecPairString = std::vector<std::pair<std::string, std::string> >;
	py::class_<VecPairString>("VecPairString").def(py::vector_indexing_suite<VecPairString, true>());

	using VecRCExpExp = std::vector<rule::RCExp::Expression>;
	py::class_<VecRCExpExp>("VecRCExpExp").def(py::vector_indexing_suite<VecRCExpExp, true>());

	// Pair
	py::to_python_converter<std::pair<std::string, std::string>, PairToTupleConverter<std::string, std::string> >();
	py::to_python_converter<std::pair<int, int>, PairToTupleConverter<int, int> >();
}

} // namespace Py
} // namespace mod
