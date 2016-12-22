#include <boost/python/module.hpp>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#define MOD_FILES() \
	(Chem)(Collections)(Config)(Derivation)(DG)(DGGraphInterface)                 \
	(DGPrinter)(DGStrat)(Error)(Function)(Graph)                                  \
	(GraphGraphInterface)(GraphPrinter)(Misc)(RC)(Rule)(RuleGraphInterface)

namespace mod {
namespace Py {
#define MDO_iterDecl(r, data, t) \
	void BOOST_PP_CAT(t, _doExport)();
BOOST_PP_SEQ_FOR_EACH(MDO_iterDecl, ~, MOD_FILES())
} // namespace Py
} // namespace mod

BOOST_PYTHON_MODULE(mod_) {
#define MDO_iterCall(r, data, t) \
		mod::Py::BOOST_PP_CAT(t, _doExport)();
	BOOST_PP_SEQ_FOR_EACH(MDO_iterCall, ~, MOD_FILES())
}
