AC_PREREQ([2.68])
# http://stackoverflow.com/questions/8559456/read-a-version-number-from-a-file-in-configure-ac
AC_INIT([mod], m4_esyscmd_s([cat VERSION]), [jakob@caput.dk])
AC_CONFIG_AUX_DIR([auxConfig])
AC_CONFIG_SRCDIR([src/mod/Config.cpp])
AM_INIT_AUTOMAKE([1.11 foreign silent-rules subdir-objects -Wall -Werror tar-ustar nostdinc])
# this AM_PROG_AR call should not be needed, but see http://debbugs.gnu.org/cgi/bugreport.cgi?bug=11401%3E
AM_PROG_AR
LT_INIT([shared disable-static])
#AC_SUBST([LIBTOOL_DEPS])

AC_CONFIG_MACRO_DIR([m4])
m4_include([m4/ax_prefix_config_h.m4])
m4_include([m4/ax_boost_base.m4])

AC_CONFIG_HEADERS([src/mod/BuildConfig.h_])
AX_PREFIX_CONFIG_H([src/mod/BuildConfig.h], [MOD], [src/mod/BuildConfig.h_])


##########################################################################
# Constants and Variable Information
##########################################################################
# search build-generated sources, distributed source, and our Boost files
AM_CPPFLAGS="-I\$(top_builddir)/src -I\$(top_srcdir)/src"
AM_CPPFLAGS="$AM_CPPFLAGS -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_RESULT_OF_USE_DECLTYPE $BOOST_CPPFLAGS -DBOOST_SYSTEM_NO_DEPRECATED"
AM_CXXFLAGS="-std=c++14 -Wno-unused-local-typedefs -Wno-unused-parameter -Wno-comment -Wno-maybe-uninitialized -Wno-deprecated-declarations -fno-stack-protector"
AM_LDFLAGS="-no-undefined"
AM_LDLIBS=""
# so we can restore before each test
CPPFLAGS=${CPPFLAGS:-""}
CXXFLAGS=${CXXFLAGS:-"-g -O3"}
LDFLAGS=${LDFLAGS:-""}
LDLIBS=${LDLIBS:-""}
LIBS=${LIBS:-""}
orig_CPPFLAGS=$CPPFLAGS
orig_CXXFLAGS=$CXXFLAGS
orig_LDFLAGS=$LDFLAGS
orig_LDLIBS=$LDLIBS
orig_LIBS=$LIBS


# Top level options 
##########################################################################
# Documentation ----------------------------------------------------------
default_enable_doc_checks="yes"
# PostMØD ----------------------------------------------------------------
default_enable_postmod="yes"
# PyMØD ------------------------------------------------------------------
default_enable_pymod="yes"
# ENABLE_PYMOD

# Individual checks
##########################################################################
# Boost ------------------------------------------------------------------
boostReqVersion="1.64.0"
# perm_group -------------------------------------------------------------
permGroupReqVersion="v0.3"
# graph_canon ------------------------------------------------------------
graphCanonReqVersion="v0.3"
# Graphviz --------------------------------------------------------------
# if PostMØD checks
# Open Babel -------------------------------------------------------------
default_with_OpenBabel="yes"
obabelReqVersion="2.3.2"
# HAVE_OPENBABEL
# pdf2svg ----------------------------------------------------------------
# if PostMØD checks
# pdflatex ---------------------------------------------------------------
# if PostMØD checks
# Python -----------------------------------------------------------------
# PYTHON_LDLIBS
# Sphinx -----------------------------------------------------------------
default_enable_sphinx="yes"
# ENABLE_SPHINX
# Boost Python (depends on Boost and Python) -----------------------------
# BOOST_PYTHON_LIB_NAME

##########################################################################
# Top level options 
##########################################################################
# PostMØD ----------------------------------------------------------------
AC_ARG_ENABLE([postmod],
	[AS_HELP_STRING([--enable-postmod=yes|no],
		[Enable/disable building of the post-processor.])],
	[
if test "$enable_postmod" != "yes" -a "$enable_postmod" != "no"; then
	AC_MSG_ERROR(["Invalid value '$enable_postmod' for --enable-postmod."])
fi
], [enable_postmod=$default_enable_postmod])
AM_CONDITIONAL([ENABLE_POSTMOD], [test "$enable_postmod" = "yes"])

# PyMØD ------------------------------------------------------------------
AC_ARG_ENABLE([pymod],
	[AS_HELP_STRING([--enable-pymod=yes|no],
		[Enable/disable building of the Python bindings for MØD.])],
	[
if test "$enable_pymod" != "yes" -a "$enable_pymod" != "no"; then
	AC_MSG_ERROR(["Invalid value '$enable_pymod' for --enable-pymod."])
fi	
], [enable_pymod=$default_enable_pymod])
AM_CONDITIONAL([ENABLE_PYMOD], [test "$enable_pymod" = "yes"])

# Check documentation requirements --------------------------------------
AC_ARG_ENABLE([doc_checks],
	[AS_HELP_STRING([--enable-doc-checks=yes|no],
		[Enable/disable configure checks for building the documentation.])],
	[
if test "$enable_doc_checks" != "yes" -a "$enable_doc_checks" != "no"; then
	AC_MSG_ERROR(["Invalid value '$enable_doc_checks' for --enable-doc-checks."])
fi
], [enable_doc_checks=$default_enable_doc_checks])

##########################################################################
# Options for individual dependencies
##########################################################################

Ipaths=()
Lpaths=()
function printPaths {
	if test ${#Ipaths[@]} > 0; then
		AC_MSG_NOTICE([Specified I paths are:])
		for p in ${Ipaths[[@]]}; do
			AC_MSG_NOTICE([   $p])
		done
	fi
	if test ${#Lpaths[@]} > 0; then
		AC_MSG_NOTICE([Specified L paths are:])
		for p in ${Lpaths[[@]]}; do
			AC_MSG_NOTICE([   $p])
		done
	fi
}
doPathSort="no"
function restoreFlags {
	CPPFLAGS=$orig_CPPFLAGS
	CXXFLAGS=$orig_CXXFLAGS
	LDFLAGS=$orig_LDFLAGS
	LDLIBS=$orig_LDLIBS
	LIBS=$orig_LIBS
	if test "$doPathSort" = "yes"; then
		doPathSort="no"
		Ipaths=($(for p in ${Ipaths[[@]]}; do echo $p; done | sort | uniq))
		Lpaths=($(for p in ${Lpaths[[@]]}; do echo $p; done | sort | uniq))
	fi
	for p in ${Ipaths[[@]]}; do
		CPPFLAGS+=" -I$p"
	done
	for p in ${Lpaths[[@]]}; do
		LDFLAGS+=" -L$p -Wl,-rpath -Wl,$p"
	done
}
function addIPath {
	Ipaths+=("$1")
	doPathSort="yes"
}
function addLPath {
	Lpaths+=("$1")
	doPathSort="yes"
	PKG_CONFIG_PATH+=":$1/pkgconfig"
	export PKG_CONFIG_PATH
}

function checkGivenPath {
	# GCC thinks it's funny to give a hard error with a -I path that is a subfolder of a file
	# (e.g., /bin/cat/foo), so check that a given path is not regular file.
	if test -f "$1"; then
		AC_MSG_ERROR([Path "$1" given to $2 is a regular file. It must be a folder.])
	fi
}

# Boost ------------------------------------------------------------------
AC_ARG_WITH([boost],
	[AS_HELP_STRING([--with-boost=arg],
		[yes: enable Boost, <path>: add specified location for dependency searching, no: not an option, you need Boost.])],
	[], [with_boost=yes])
if test "$with_boost" != "yes" -a "$with_boost" != "no"; then
	checkGivenPath "$with_boost" --with-boost
	addIPath "$with_boost/include"
	addLPath "$with_boost/lib"
fi

# perm_group -------------------------------------------------------------
AC_ARG_WITH([perm_group],
    [AS_HELP_STRING([--with-perm_group=arg],
		[yes: enable PermGroup, <path>: add specified location for dependency searching, no: not an option, you need PermGroup.])],
	[], [with_perm_group=yes])
if test "$with_perm_group" != "yes" -a "$with_perm_group" != "no"; then
	checkGivenPath "$with_perm_group" --with-perm_group
	addIPath "$with_perm_group/include"
	addLPath "$with_perm_group/lib"
fi

# graph_canon ------------------------------------------------------------
AC_ARG_WITH([graph_canon],
	[AS_HELP_STRING([--with-graph_canon=arg],
		[yes: enable GraphCanon, <path>: add specified location for dependency searching, no: not an option, you need GraphCanon.])],
	[], [with_graph_canon=yes])
if test "$with_graph_canon" != "yes" -a "$with_graph_canon" != "no"; then
	checkGivenPath "$with_graph_canon" --with-graph_canon
	addIPath "$with_graph_canon/include"
	addLPath "$with_graph_canon/lib"
fi

# Open Babel -------------------------------------------------------------
AC_ARG_WITH([OpenBabel],
	[AS_HELP_STRING([--with-OpenBabel=prefix],
		[yes: enable Open Babel, no: disable Open Babel, <path>: use Open Babel from specified location.])],
	[], [with_OpenBabel=$default_with_OpenBabel])
if test "$with_OpenBabel" != no -a "$with_OpenBabel" != "yes"; then
	checkGivenPath "$with_OpenBabel" --with-OpenBabel
	addIPath "$with_OpenBabel/include"
	addLPath "$with_OpenBabel/lib"
fi

# Python -----------------------------------------------------------------
if test "$enable_pymod" = "yes"; then
	AC_CHECK_PROGS([PYTHON_CONFIG], [python3-config], [no])
	if test "$PYTHON_CONFIG" = "no"; then
		AC_MSG_ERROR([python3-config could not be found.])
	fi
	for p in $(python3-config --includes); do
		inc="${p#-I}"
		addIPath "$inc"
	done
fi

##########################################################################
# Check Stuff
##########################################################################

# Compiler Stuff ---------------------------------------------------------
AC_PROG_CXX
AC_PROG_CXXCPP
CXXFLAGS=""
AC_LANG([C++])

# Boost ------------------------------------------------------------------
restoreFlags
if test "$with_boost" = "no"; then
	AC_MSG_ERROR([You have to want Boost libraries. --with-boost=no is not an option.])
else
	AC_CHECK_HEADER([boost/version.hpp],
		[],
		[AC_MSG_NOTICE([Boost not found.])
		 printPaths
		 if test $with_boost != "yes"; then
			AC_MSG_ERROR([Is the path "$with_boost" correct?])
		 else
			AC_MSG_ERROR([Use --with-boost=<path> to specify where Boost is installed.])
		 fi
	])

	AC_MSG_CHECKING(for Boost >= $boostReqVersion)
	# these checks are based on the ax_boost_base.m4 macros
	expr "$boostReqVersion" : '[[0-9]]\{1,\}\.[[0-9]]\{1,\}\.[[0-9]]\{1,\}$' &> /dev/null
	if test $? -ne 0; then
		AC_MSG_ERROR([Boost version "$boostReqVersion" is invalid. It most be on the form MAJOR.MINOR.PATCH])
	fi
	major=$(expr "$boostReqVersion" : '\([[0-9]]\{1,\}\)\.[[0-9]]\{1,\}\.[[0-9]]\{1,\}$')
	minor=$(expr "$boostReqVersion" : '[[0-9]]\{1,\}\.\([[0-9]]\{1,\}\)\.[[0-9]]\{1,\}$')
	patch=$(expr "$boostReqVersion" : '[[0-9]]\{1,\}\.[[0-9]]\{1,\}\.\([[0-9]]\{1,\}\)$')
	reqVersion=$(expr \( $major \* 1000 \+ $minor \) \* 100 + $patch)
	AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
		#include <boost/version.hpp>
		#if BOOST_VERSION >= $reqVersion
		// yay
		#else
		#error Too old
		#endif
	]])], [versionOk="yes"], [versionOk="no"])
	AC_MSG_RESULT($versionOk)
	if test $versionOk = "no"; then
		AC_MSG_ERROR([Boost version too old.])
	fi
fi


# perm_group -------------------------------------------------------------
restoreFlags
if test "$with_perm_group" = "no"; then
	AC_MSG_ERROR([You have to want the PermGroup library. --with-perm_group=no is not an option.])
else
	PKG_CHECK_MODULES([perm_group], [perm-group >= $permGroupReqVersion], [], [
		AC_MSG_NOTICE([$perm_group_PKG_ERRORS])
		AC_MSG_NOTICE([PKG_CONFIG_PATH=$PKG_CONFIG_PATH])
		printPaths
		AC_MSG_ERROR([Use --with-perm_group=prefix, or set PKG_CONFIG_PATH for non-standard install prefix.])])
	addIPath "$(pkg-config perm-group --variable=pkgincludedir)"
fi
	
# graph_canon ------------------------------------------------------------
restoreFlags
if test "$with_graph_canon" = "no"; then
	AC_MSG_ERROR([You have to want the GraphCanon library. --with-graph_canon=no is not an option.])
else
	PKG_CHECK_MODULES([graph_canon], [graph-canon >= $graphCanonReqVersion], [], [
		AC_MSG_NOTICE([$graph_canon_PKG_ERRORS])
		AC_MSG_NOTICE([PKG_CONFIG_PATH=$PKG_CONFIG_PATH])
		printPaths
		AC_MSG_ERROR([Use --with-graph_canon=prefix, or set PKG_CONFIG_PATH for non-standard install prefix.])])
	addIPath "$(pkg-config graph-canon --variable=includedir)"
fi

# Graphviz ---------------------------------------------------------------
restoreFlags
if test "$enable_postmod" = "yes"; then
	AC_CHECK_PROGS([DOT], [dot], [no])
	if test "$DOT" = "no"; then
		AC_MSG_ERROR([dot (from Graphviz) is essential for PostMØD.
$postmod_check_fail_msg])
	fi

	graphviz_format_fail_msg="Please report false positives/negatives on the Graphviz image formats checks."

	AC_MSG_CHECKING([Graphviz image format, cairo svg output])
	dot -P 2>&1 | grep "cairo_device_svg -> output_svg" &> /dev/null
	if test $? -ne 0; then
		AC_MSG_RESULT([not found])
		AC_MSG_ERROR([svg output of dot is essential for PostMØD.
$graphviz_format_fail_msg
$postmod_check_fail_msg])
	else
		AC_MSG_RESULT([found])
	fi

	AC_MSG_CHECKING([Graphviz image format, cairo pdf output])
	dot -P 2>&1 | grep "cairo_device_pdf -> output_pdf" &> /dev/null
	if test $? -ne 0; then
		AC_MSG_RESULT([not found])
		AC_MSG_ERROR([pdf output of dot is essential for PostMØD.
$graphviz_format_fail_msg
$postmod_check_fail_msg])
	else
		AC_MSG_RESULT([found])
	fi

	AC_MSG_CHECKING([Graphviz image format, cairo svg input via rsvg])
	dot -P 2>&1 | grep "rsvg_loadimage_svg -> render_cairo" &> /dev/null
	if test $? -ne 0; then
		AC_MSG_RESULT([not found])
		AC_MSG_ERROR([svg input of dot is essential for PostMØD.
$graphviz_format_fail_msg
$postmod_check_fail_msg])
	else
		AC_MSG_RESULT([found])
	fi
fi

# Open Babel -------------------------------------------------------------
restoreFlags
if test "$with_OpenBabel" != "no"; then
	AC_DEFINE(HAVE_OPENBABEL, 1, [Define if you have Open Babel available.])
	PKG_CHECK_MODULES([OpenBabel], [openbabel-2.0 >= $obabelReqVersion], [], [
		AC_MSG_NOTICE([$OpenBabel_PKG_ERRORS])
		AC_MSG_NOTICE([PKG_CONFIG_PATH=$PKG_CONFIG_PATH])
		printPaths
		AC_MSG_ERROR([Use --with-OpenBabel=prefix, or set PKG_CONFIG_PATH for non-standard install prefix.])])
	addIPath "$(pkg-config openbabel-2.0 --variable=pkgincludedir)"
fi
AM_CONDITIONAL([HAVE_OPENBABEL], [test "$with_OpenBabel" != "no"])

# pdf2svg ----------------------------------------------------------------
restoreFlags
if test "$enable_postmod" = "yes"; then
	AC_CHECK_PROGS([PDF2SVG], [pdf2svg], [no])
	if test "$PDF2SVG" = "no"; then
		AC_MSG_ERROR([pdf2svg is essential for PostMØD.
$postmod_check_fail_msg])
	fi
fi

# pdflatex ---------------------------------------------------------------
restoreFlags
if test "$enable_postmod" = "yes"; then
	AC_CHECK_PROGS([PDFLATEX], [pdflatex], [no])
	if test "$PDFLATEX" = "no"; then
		AC_MSG_ERROR([pdflatex is essential for PostMØD.
$postmod_check_fail_msg])
	fi
fi

# Python -----------------------------------------------------------------
restoreFlags
if test "$enable_pymod" = "yes"; then
	AC_CHECK_HEADER([Python.h], [], [
		printPaths
		AC_MSG_ERROR([Could not find Python headers.])
	])
	PYTHON_LDLIBS=$(python3-config --libs)
	AC_SUBST([PYTHON_LDLIBS])
	PYTHON_VERSION=$(python3 -c "import sys; print(sys.version_info.minor);")
fi

# Sphinx -----------------------------------------------------------------
restoreFlags
AC_CHECK_PROGS([SPHINXBUILD], [sphinx-build sphinx-build3 sphinx-build-3 sphinx-build2], [no])
if test "$SPHINXBUILD" = "no"; then
	if test "$enable_doc_checks" = "yes"; then
		AC_MSG_ERROR([sphinx-build is required for building the documentation.
This error can be suppressed with '--disable-doc-checks'])
	else
		AC_MSG_WARN([sphinx-build is required for building the documentation.
This warning will be an error without '--disable-doc-checks'])
	fi
fi
AM_CONDITIONAL([ENABLE_SPHINX], [test "$SPHINXBUILD" != "no"])

# Boost Python -----------------------------------------------------------
if test "$enable_pymod" = "yes"; then
	AC_CHECK_HEADER([boost/python/module.hpp],
		[],
		[AC_MSG_ERROR([Could not find Boost.Python.])])
	# Find the proper name for the library.
	# Ubuntu thinks it's great to only have the -py3X names.
	# PYTHON_VERSION was set when we checked for python3
	BOOST_PYTHON_LIB_NAME=""
	for libName in boost_python{3,3$PYTHON_VERSION,-py3$PYTHON_VERSION}; do
		restoreFlags
		LIBS+=" -l$libName $PYTHON_LDLIBS"
		AC_MSG_CHECKING([for Boost.Python library, using Python 3 (lib name=$libName)])
		AC_LINK_IFELSE([
			AC_LANG_SOURCE([[
				#include <boost/python/module.hpp>
				using namespace boost::python;
				BOOST_PYTHON_MODULE(test) { throw "Boost::Python test."; }
				int main() {}]])],
			[
				AC_MSG_RESULT([yes])
				BOOST_PYTHON_LIB_NAME="$libName"
				break
			],
			[
				AC_MSG_RESULT([no])
			])
	done
	if test "x$BOOST_PYTHON_LIB_NAME" = "x"; then
		AC_MSG_ERROR([Could not link test program using Boost.Python.])
	fi
	AC_SUBST([BOOST_PYTHON_LIB_NAME])
fi

##########################################################################
# pkg-config file
##########################################################################
restoreFlags
PKG_INCLUDES=""
for p in ${Ipaths[[@]]}; do
	PKG_INCLUDES+=" -I$p"
done
PKG_LIBS="-pthread"

AC_SUBST([PKG_INCLUDES])
AC_SUBST([PKG_LIBS])

##########################################################################
# Finalize
##########################################################################
restoreFlags
AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])
AC_SUBST([AM_LDLIBS])

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	doc/Makefile
	src/lib/pkgconfig/mod.pc])
AC_OUTPUT
