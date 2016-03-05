#include <mod/Py/Common.h>

#include <mod/Derivation.h>
#include <mod/DG.h>
#include <mod/RC.h>

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <vector>

namespace mod {
// This is just hax for the vector_indexing_suite, which requires == and !=.
// Currently the Boost sandbox has a version where the operators can be disabled.

bool operator==(const Derivation&, const Derivation&) {
	std::cerr << "Derivation does not support ==" << std::endl;
	std::exit(1);
}

namespace RCExp {

bool operator==(const Expression&, const Expression&) {
	std::cerr << "RCExpExp does not support ==" << std::endl;
	std::exit(1);
}

} // namespace RCExp
} // namespace mod

namespace mod {
class DGStrat;
class Graph;
class Rule;
namespace Py {

void Collections_doExport() {
	// Vector
	using VecDerivation = std::vector<mod::Derivation>;
	py::class_<VecDerivation>("VecDerivation").def(py::vector_indexing_suite<VecDerivation, true>());
	using VecDerivationRef = std::vector<mod::DerivationRef>;
	py::class_<VecDerivationRef>("VecDerivationRef").def(py::vector_indexing_suite<VecDerivationRef, true>());
	using VecDGStrat = std::vector<std::shared_ptr<mod::DGStrat> >;
	py::class_<VecDGStrat>("VecDGStrat").def(py::vector_indexing_suite<VecDGStrat, true>());
	using VecGraph = std::vector<std::shared_ptr<mod::Graph> >;
	py::class_<VecGraph>("VecGraph").def(py::vector_indexing_suite<VecGraph, true>());
	using VecRule = std::vector<std::shared_ptr<mod::Rule> >;
	py::class_<VecRule>("VecRule").def(py::vector_indexing_suite<VecRule, true>());

	using VecRCExpExp = std::vector<mod::RCExp::Expression>;
	py::class_<VecRCExpExp>("VecRCExpExp").def(py::vector_indexing_suite<VecRCExpExp, true>());

	// Pair
	py::to_python_converter<std::pair<std::string, std::string>, PairToTupleConverter<std::string, std::string> >();
}

} // namespace Py
} // namespace mod