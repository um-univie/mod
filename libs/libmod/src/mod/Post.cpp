#include "Post.hpp"

#include <mod/Error.hpp>
#include <mod/lib/IO/IO.hpp>

#include <iomanip>

namespace mod::post {

FileHandle::FileHandle(std::string name) : name(name) {
	if(name.find("out/") != 0)
		throw LogicError("The file is not in the out/ folder.");
	stream.open(name.c_str());
	if(!stream)
		throw LogicError("Could not open file '" + name + "'.");
}

void command(const std::string &text) {
	lib::IO::post() << text << std::endl;
}

void reset() {
	lib::IO::postReset();
}

void flush() {
	lib::IO::post() << std::flush;
}

void disable() {
	lib::IO::postDisable();
}

void summaryChapter(const std::string &chapterTitle) {
	lib::IO::post() << "summaryChapter \"" << chapterTitle << "\"" << std::endl;
}

void summarySection(const std::string &sectionTitle) {
	lib::IO::post() << "summarySection \"" << sectionTitle << "\"" << std::endl;
}

std::string makeUniqueFilePrefix() {
	return lib::IO::getUniqueFilePrefix();
}

} // namespace mod::post