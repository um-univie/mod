#include <mod/py/Common.hpp>

#include <mod/Term.hpp>

namespace mod::Py {

void Term_doExport() {
	py::def("mgu", &mod::Term::mgu);
}

} // namespace mod::Py