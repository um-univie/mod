#include <mod/py/Common.hpp>

#include <mod/Post.hpp>

namespace mod::post::Py {

void Post_doExport() {
	// rst: .. function:: makeUniqueFilePrefix()
	// rst:
	// rst:		:returns: a string on the form ``out/iii_`` where ```iii``` is the next zero-padded integer
	// rst:			from an internal counter.
	// rst:		:rtype: str
	py::def("makeUniqueFilePrefix", &makeUniqueFilePrefix);

	struct Post {
		// a fake class to hax a "submodule"
	};
	// rst: .. class:: post
	// rst:
	// rst:		This class (which will be a submodule in the future)
	// rst:		contains various functions to manipulate post-processing (:ref:`mod_post`).
	// rst:		Commands for the post-processor are written to the command file ``out/post.sh``
	// rst:		which the post-processor executes internally as a Bash script.
	// rst:
	py::class_<Post, boost::noncopyable>("post", py::no_init)
			// rst:		.. staticmethod:: command(line)
			// rst:
			// rst:			Write the given text to the command file and write a newline character.
			// rst:
			// rst:			:param str line: the text to be written.
			// rst:
			// rst:			.. warning:: The contents of the command file is executed without any security checks.
			.def("command", &command).staticmethod("command")
					// rst:		.. staticmethod:: flushCommands()
					// rst:
					// rst:			Flush the command file buffer.
			.def("flushCommands", &flushCommands).staticmethod("flushCommands")
					// rst:		.. staticmethod:: disableCommands()
					// rst:
					// rst:			Disable command writing and flushing, also for commands emitted internally in the library.
			.def("disableCommands", &disableCommands).staticmethod("disableCommands")
					// rst:		.. staticmethod:: enableCommands()
					// rst:
					// rst:			Enable command writing and flushing, also for commands emitted internally in the library.
			.def("enableCommands", &enableCommands).staticmethod("enableCommands")
					// rst:		.. staticmethod:: reopenCommandFile()
					// rst:
					// rst:			Reopen the command file, which may be useful if it was modified externally while open by the library.
			.def("reopenCommandFile", &reopenCommandFile).staticmethod("reopenCommandFile")
					// rst:		.. staticmethod:: summaryChapter(heading)
					// rst:
					// rst:			Command the post-processor to insert a ``\chapter`` macro in the summary.
					// rst:
					// rst:			:param str heading: the chapter heading to insert.
			.def("summaryChapter", &summaryChapter).staticmethod("summaryChapter")
					// rst:		.. staticmethod:: summarySection(heading)
					// rst:
					// rst:			Command the post-processor to insert a ``\section`` macro in the summary.
					// rst:
					// rst:			:param str heading: the section heading to insert.
			.def("summarySection", &summarySection).staticmethod("summarySection")
					// rst:		.. staticmethod:: summaryRaw(latexCode, file=...)
					// rst:
					// rst:			Command the post-processor to insert the given code verbatim in the summary.
					// rst:
					// rst:			:param str latexCode: the code to insert.
					// rst:			:param str file: if given then that will be appended to a unique prefix for the final filename the code is stored in.
			.def("summaryRaw", static_cast<void (*)(const std::string &)>(&summaryRaw))
			.def("summaryRaw", static_cast<void (*)(const std::string &, const std::string &)>(&summaryRaw))
			.staticmethod("summaryRaw")
					// rst:		.. staticmethod:: summaryInput(filename)
					// rst:
					// rst:			Command the post-processor to insert a ``\input`` macro in the summary.
					// rst:
					// rst:			:param str filename: the filename to input.
			.def("summaryInput", &summaryInput).staticmethod("summaryInput")
					// rst:		.. staticmethod:: disableInvokeMake()
					// rst:		                  enableInvokeMake()
					// rst:
					// rst:			Disable/enable the invocation of Make in the post-processor.
					// rst:			The processing of commands and generation of Makefiles will still be carried out,
					// rst:			and Make invocation can be done manually afterwards through the post-processor
			.def("disableInvokeMake", &disableInvokeMake).staticmethod("disableInvokeMake")
			.def("enableInvokeMake", &enableInvokeMake).staticmethod("enableInvokeMake")
					// rst:		.. staticmethod:: disableCompileSummary()
					// rst:		                  enableCompileSummary()
					// rst:
					// rst:			Disable/enable the compilation of the final summary during post-processing.
					// rst:			The compilation can be invoked manually afterwards through the post-processor.
			.def("disableCompileSummary", &disableCompileSummary).staticmethod("disableCompileSummary")
			.def("enableCompileSummary", &enableCompileSummary).staticmethod("enableCompileSummary");
}

} // namespace mod::post::Py