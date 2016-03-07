#include <mod/Py/Common.h>

#include <mod/Config.h>
#include <mod/Misc.h>

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

namespace mod {
namespace Py {
namespace {

uintptr_t magicLibraryValue() {
	return (uintptr_t)&mod::getConfig();
}

} // namespace 

void Misc_doExport() {
	// rst: .. function:: magicLibraryValue()
	// rst:
	// rst:		:returns: a number unique for each instantiation of libMØD. This can be used to check for the diamond problem for shared libraries.
	// rst:
	// rst:		:rtype: uintptr_t
	py::def("magicLibraryValue", &magicLibraryValue);

	py::def("prefixFilename", &mod::prefixFilename);
	py::def("pushFilePrefix", &mod::pushFilePrefix);
	py::def("popFilePrefix", &mod::popFilePrefix);
	py::def("post", &mod::post);
	py::def("postChapter", &mod::postChapter);
	py::def("postSection", &mod::postSection);

	// rst: .. function:: version()
	// rst:
	// rst:		:returns: the version of MØD.
	// rst:		:rtype: string
	py::def("version", &mod::version);
}

} // namespace Py
} // namespace mod
