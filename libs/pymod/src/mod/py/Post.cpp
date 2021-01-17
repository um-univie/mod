#include <mod/py/Common.hpp>

#include <mod/Post.hpp>

namespace mod::post::Py {

void Post_doExport() {
	py::def("post", &command);
	py::def("postReset", &reset);
	py::def("postFlush", &flush);
	py::def("postDisable", &disable);

	py::def("postChapter", &summaryChapter);
	py::def("postSection", &summarySection);

	// rst: .. function:: makeUniqueFilePrefix()
	// rst:
	// rst:		:returns: a unique file prefix from the ``out/`` folder.
	// rst:		:rtype: str
	py::def("makeUniqueFilePrefix", &makeUniqueFilePrefix);
}

} // namespace mod::post::Py