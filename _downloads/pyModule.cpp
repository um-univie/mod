#include <boost/python.hpp>

#include "stuff.h"

#include <mod/Config.h>

namespace py = boost::python;

namespace {
	// this can be used to make sure the extension and mod is using the same shared library
	uintptr_t magicLibraryValue() {
		return (uintptr_t)&mod::getConfig();
	}
}

BOOST_PYTHON_MODULE(awesome) {
	py::def("magicLibraryValue", &magicLibraryValue);

	py::def("doStuff", &awesome::doStuff);
}
