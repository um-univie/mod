#ifndef MOD_POST_HPP
#define MOD_POST_HPP

#include <mod/BuildConfig.hpp>

#include <fstream>
#include <string>

// rst: This header contains various functions to manipulate post-processing (:ref:`mod_post`).
// rst: Commands for the post-processor are written to the command file ``out/post.sh``
// rst: which the post-processor executes internally as a Bash script.
// rst:

namespace mod::post {

struct MOD_DECL FileHandle {
	// throws LogicError if the file can not be opened
	explicit FileHandle(const std::string &name);

	operator std::ostream &() {
		return stream;
	}

	operator std::string() {
		return name;
	}

	template<typename T>
	friend std::ostream &operator<<(FileHandle &s, T &&t) {
		return s.stream << std::forward<T>(t);
	}
public:
	std::ofstream stream;
	std::string name;
};


// rst: .. function:: std::string post::makeUniqueFilePrefix()
// rst:
// rst:		:returns: a string on the form ``out/iii_`` where ```iii``` is the next zero-padded integer
// rst:			from an internal counter.
MOD_DECL std::string makeUniqueFilePrefix();

// rst: .. function:: void post::command(const std::string &line)
// rst:
// rst:		Write the given text to the command file and write a newline character.
// rst:
// rst:		.. warning:: The contents of the command file is executed without any security checks.
MOD_DECL void command(const std::string &line);
// rst: .. function:: void post::flushCommands()
// rst:
// rst:		Flush the command file buffer.
MOD_DECL void flushCommands();
// rst: .. function:: void post::disableCommands()
// rst:               void post::enableCommands()
// rst:
// rst:		Disable/enable command writing and flushing, also for commands emitted internally in the library.
MOD_DECL void disableCommands();
MOD_DECL void enableCommands();

// rst: .. function:: void post::reopenCommandFile()
// rst:
// rst:		Reopen the command file, which may be useful if it was modified externally while open by the library.
MOD_DECL void reopenCommandFile();

// rst: .. function:: void post::summaryChapter(const std::string &heading)
// rst:
// rst:		Command the post-processor to insert a ``\chapter`` macro in the summary.
MOD_DECL void summaryChapter(const std::string &heading);
// rst: .. function:: void post::summarySection(const std::string &heading)
// rst:
// rst:		Command the post-processor to insert a ``\section`` macro in the summary.
MOD_DECL void summarySection(const std::string &heading);
// rst: .. function:: void post::summaryRaw(const std::string &latexCode)
// rst:               void post::summaryRaw(const std::string &latexCode, const std::string &file)
// rst:
// rst:		Command the post-processor to insert the given code verbatim in the summary.
// rst:		If `file` is given then that will be appended to a unique prefix for the final filename the code is stored in.
MOD_DECL void summaryRaw(const std::string &latexCode);
MOD_DECL void summaryRaw(const std::string &latexCode, const std::string &file);
// rst: .. function:: void post::summaryInput(const std::string &filename)
// rst:
// rst:		Command the post-processor to insert a ``\input`` macro in the summary.
MOD_DECL void summaryInput(const std::string &filename);

// rst: .. function:: void post::disableInvokeMake()
// rst:               void post::enableInvokeMake()
// rst:
// rst:		Disable/enable the invocation of Make in the post-processor.
// rst:		The processing of commands and generation of Makefiles will still be carried out,
// rst:		and Make invocation can be done manually afterwards through the post-processor
MOD_DECL void disableInvokeMake();
MOD_DECL void enableInvokeMake();
// rst: .. function:: void post::disableCompileSummary()
// rst:               void post::enableCompileSummary()
// rst:
// rst:		Disable/enable the compilation of the final summary during post-processing.
// rst:		The compilation can be invoked manually afterwards through the post-processor.
MOD_DECL void disableCompileSummary();
MOD_DECL void enableCompileSummary();

} // namespace mod::post

#endif // MOD_POST_HPP