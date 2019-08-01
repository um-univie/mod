#include <mod/py/Common.hpp>

#include <mod/Config.hpp>
#include <mod/Misc.hpp>
#include <mod/dg/GraphInterface.hpp>
#include <mod/lib/Graph/Single.hpp>

#include <mod/py/Function.hpp>

#include <boost/core/noncopyable.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

namespace mod {
namespace Py {

// rst:
// rst: .. function:: prefixFilename(name)
// rst:
// rst: 	Utility function for converting script-relative paths to current-working-directory-relative paths
// rst: 	(see also :py:func:`include`).
// rst: 	This function is used in all PyMØD functions that takes a filename as argument.
// rst: 	To circumvent this prefixing use the :py:class:`CWDPath` class.
// rst:
// rst: 	:returns: `name` prefixed with all strings pushed with :py:func:`pushFilePrefix`
// rst: 		and popped yet.
// rst: 	:rtype: string
// rst:
// rst: .. function:: pushFilePrefix(s)
// rst:
// rst: 	Push another prefix used in :py:func:`prefixFilename`.
// rst: 	The prefixes are concatenated in the order they are pushed.
// rst: 	No directory delimiters are inserted, so they must explicitly be part of the pushed prefixes.
// rst: 	If the argument starts with ``/``, then during concatenation all previously pushed prefixes are ignored.
// rst:
// rst: 	:param s: the string to push as a prefix to be concatenated in :py:func:`prefixFilename`.
// rst: 	:type s: string
// rst:
// rst: .. function:: popFilePrefix()
// rst:
// rst: 	Pop a previously pushed prefix.
// rst:
// rst: .. class:: CWDPath
// rst:
// rst: 	A dummy class to wrap a filename in to disable prefixing.
// rst:
// rst: 	For example, if a graph GML file is to be loaded relative to the current file
// rst: 	(assuming :py:func:`include` is used for script inclusion), then it can be done with
// rst:
// rst: 	.. code-block:: python
// rst:
// rst: 		g = graphGML(f)
// rst:
// rst: 	If the file is located relative to the current working directory
// rst: 	(i.e., where the :any:`mod` wrapper script were invoked from), then the graph loading should be done as
// rst:
// rst: 	.. code-block:: python
// rst:
// rst: 		g = graphGML(CWDPath(f))
// rst:
// rst: 	.. function:: __init__(self, f)
// rst:
// rst: 		Wrap a filename.
// rst:

namespace {

std::uintptr_t magicLibraryValue() {
	return reinterpret_cast<std::uintptr_t> (&mod::getConfig());
}

} // namespace 

void Misc_doExport() {
	// rst: .. function:: magicLibraryValue()
	// rst:
	// rst:		:returns: a number unique for each instantiation of libMØD. This can be used to check for the diamond problem for shared libraries.
	// rst:
	// rst:		:rtype: uintptr_t
	py::def("magicLibraryValue", &magicLibraryValue);

	// rst: .. function:: version()
	// rst:
	// rst:		:returns: the version of MØD.
	// rst:		:rtype: string
	py::def("version", &mod::version);

	// rst: .. function:: rngReseed(seed)
	// rst:
	// rst: 		Reseed the random bit generator used in the library.
	py::def("rngReseed", &mod::rngReseed);

	py::def("post", &mod::post);
	py::def("postChapter", &mod::postChapter);
	py::def("postSection", &mod::postSection);

	
	py::def("printGeometryGraph", &mod::printGeometryGraph);
}

} // namespace Py
} // namespace mod
