#ifndef MOD_MISC_H
#define MOD_MISC_H

#include <mod/graph/ForwardDecl.h>
#include <mod/dg/ForwardDecl.h>

#include <memory>
#include <string>

namespace mod {

std::string prefixFilename(const std::string &name);
void pushFilePrefix(const std::string &prefix);
void popFilePrefix();

void post(const std::string &text);
void postChapter(const std::string &chapterTitle);
void postSection(const std::string &sectionTitle);

void printGeometryGraph();

// rst: .. function:: std::string version()
// rst:
// rst:		:returns: the version of MØD.
std::string version();

} // namespace mod

#endif	/* MOD_MISC_H */