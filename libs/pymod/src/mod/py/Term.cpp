#include <mod/py/Common.hpp>

#include <mod/Term.hpp>

namespace mod {
namespace Py {

void Term_doExport() {
	py::def("mgu", &mod::Term::mgu);
}

} // namespace Py
} // namespace mod