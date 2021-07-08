#ifndef MOD_POST_HPP
#define MOD_POST_HPP

#include <mod/BuildConfig.hpp>

#include <fstream>
#include <string>

namespace mod::post {

struct MOD_DECL FileHandle {
	// throws LogicError if the file can not be opened
	explicit FileHandle(std::string name);

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

MOD_DECL void command(const std::string &text);
MOD_DECL void reset();
MOD_DECL void flush();
MOD_DECL void disable();

MOD_DECL void summaryChapter(const std::string &chapterTitle);
MOD_DECL void summarySection(const std::string &sectionTitle);

// rst: .. function:: std::string post::makeUniqueFilePrefix()
// rst:
// rst:		:returns: a unique file prefix from the ``out/`` folder.
MOD_DECL std::string makeUniqueFilePrefix();

} // namespace mod::post

#endif // MOD_POST_HPP