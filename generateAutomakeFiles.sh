#!/bin/bash

function indentAndSlash {
	cat | sort | \
		sed "s/^.\\//	/" |	\
		sed "s/$/ \\\\/" |		\
		sed "\$s/\\\\//"
}

function doSrc {
	cat <<-EOF
	pkgconfigdir = \$(libdir)/pkgconfig
	pkgconfig_DATA = lib/pkgconfig/mod.pc

	bin_PROGRAMS =
	dist_bin_SCRIPTS = \\
		bin/mod
	pkgdata_DATA = 

	dist_pkgdata_DATA = \\
		share/mod/libSBML.supp \\
		share/mod/obabel.supp \\
		share/mod/python.supp

	if ENABLE_POSTMOD
	dist_bin_SCRIPTS += \\
		bin/mod_post \\
		bin/mod_genSummaryMakefile
	dist_pkgdata_DATA += \\
		share/mod/commonPreamble.tex \\
		share/mod/figureTemplate.tex \\
		share/mod/summary.tex \\
		share/mod/mod.sty \\
		share/mod/mod.mk
	pkgdata_DATA += \\
		share/mod/commonPreamble.fmt
	
	share/mod/commonPreamble.fmt: \$(srcdir)/share/mod/commonPreamble.tex
	EOF
	echo "	mkdir -p \$(builddir)/share/mod"
	echo "	cd \$(builddir)/share/mod && pdflatex \\"
	echo "		-interaction=nonstopmode -halt-on-error \\"
	echo "		-ini \"&pdflatex \input{\$(abs_srcdir)/share/mod/commonPreamble.tex}\usepackage{\$(abs_srcdir)/share/mod/mod}\dump\""
	echo "endif"


	cat <<-EOF
	lib_LTLIBRARIES = libmod.la

	libmod_la_CPPFLAGS = \$(AM_CPPFLAGS)
	libmod_la_CXXFLAGS = \$(AM_CXXFLAGS)
	libmod_la_LDFLAGS = \$(AM_LDFLAGS) -no-undefined -pthread
	libmod_la_LIBADD =  \$(AM_LDLIBS) -lboost_regex -lboost_system

	if HAVE_OPENBABEL
	libmod_la_LIBADD += -lopenbabel
	endif
	if HAVE_SBML
	libmod_la_LIBADD += -lsbml
	endif
	libmod_la_LIBADD += -lpthread

	nobase_nodist_include_HEADERS = \\
		mod/BuildConfig.h
	nobase_include_HEADERS = \\
	EOF
	(
		find . -name "*.h" | grep mod/ | grep -v mod/Py/ 
		find . -name "*.hpp" | grep jla_boost/ | grep -v mod/Py/
	) | indentAndSlash 
	echo "libmod_la_SOURCES = \\"
	(
		find . -name "*.cpp" | grep mod/ | grep -v mod/Py/ 
		find . -name "*.cpp" | grep jla_boost/ | grep -v mod/Py/ 
	) | indentAndSlash 

	cat <<-EOF
	# PyMOD
	if ENABLE_PYMOD
	pkglib_LTLIBRARIES = mod_.la
	mod__la_CPPFLAGS = \$(AM_CPPFLAGS)
	mod__la_CXXFLAGS = \$(AM_CXXFLAGS)
	mod__la_LDFLAGS = \$(AM_LDFLAGS) -module
	mod__la_LIBADD = \$(AM_LDLIBS)
	mod__la_LIBADD += libmod.la
	mod__la_LIBADD += -l\$(BOOST_PYTHON_LIB_NAME)
	mod__la_LIBADD += \$(PYTHON_LDLIBS)
	
	nobase_include_HEADERS += \\
	EOF
	find . -name "*.h" | grep mod/Py/ | indentAndSlash
	echo "mod__la_SOURCES = \\"
	find . -name "*.cpp" | grep mod/Py/ | indentAndSlash

	echo "haxdir = \$(pkglibdir)" # because automake doesn't like data in lib
	echo "dist_hax_DATA = lib/mod/__init__.py"
	echo "dist_hax_DATA += lib/mod/latex.py"
	echo ""
	echo "endif"
}

function doDoc {
	echo "EXTRA_DIST = \\"
	function extraDist {
		echo "./makeDocs.sh"
		find "./source/" -type f
	}
	extraDist | indentAndSlash
}

echo "VERSION"
git describe --tags --always > VERSION
cd src
rm -f Makefile.am
echo "src/Makefile.am"
doSrc >> Makefile.am
cd ..
cd doc
rm -rf Makefile.am
echo "doc/Makefile.am"
doDoc >> Makefile.am
cd ..
