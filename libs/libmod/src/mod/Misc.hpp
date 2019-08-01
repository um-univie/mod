#ifndef MOD_MISC_H
#define MOD_MISC_H

#include <mod/BuildConfig.hpp>
#include <mod/graph/ForwardDecl.hpp>
#include <mod/dg/ForwardDecl.hpp>

#include <memory>
#include <string>

namespace mod {

// rst: .. function:: std::string version()
// rst:
// rst:		:returns: the version of MØD.
MOD_DECL std::string version();

// rst: .. function:: void rngReseed(unsigned int seed)
// rst:
// rst:		Reseed the random bit generator used in the library.
MOD_DECL void rngReseed(unsigned int seed);

MOD_DECL void post(const std::string &text);
MOD_DECL void postChapter(const std::string &chapterTitle);
MOD_DECL void postSection(const std::string &sectionTitle);

MOD_DECL void printGeometryGraph();


} // namespace mod

#endif	/* MOD_MISC_H */
