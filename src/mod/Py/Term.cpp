#include <mod/Py/Common.h>

#include <mod/Term.h>

namespace mod {
namespace Py {

void Term_doExport() {
	py::def("mgu", &mod::Term::mgu);
}

} // namespace Py
} // namespace mod