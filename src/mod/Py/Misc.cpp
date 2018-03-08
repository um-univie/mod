#include <mod/Py/Common.h>

#include <mod/Config.h>
#include <mod/Misc.h>
#include <mod/dg/GraphInterface.h>
#include <mod/lib/Graph/Single.h>
#include <mod/lib/Random.h>

#include <jla_boost/test/vf2.hpp>

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

namespace mod {
namespace Py {
namespace {

std::uintptr_t magicLibraryValue() {
	return reinterpret_cast<std::uintptr_t>(&mod::getConfig());
}

void reseed(unsigned int seed) {
	lib::Random::getInstance().reseed(seed);
}

} // namespace 

void Misc_doExport() {
	// rst: .. function:: magicLibraryValue()
	// rst:
	// rst:		:returns: a number unique for each instantiation of libMØD. This can be used to check for the diamond problem for shared libraries.
	// rst:
	// rst:		:rtype: uintptr_t
	py::def("magicLibraryValue", &magicLibraryValue);
	
	py::def("rngReseed", &reseed);

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
	
	py::def("printGeometryGraph", &mod::printGeometryGraph);

	// jla_boost tests
	py::def("test_vf2", &jla_boost::test::vf2);
}

} // namespace Py
} // namespace mod
