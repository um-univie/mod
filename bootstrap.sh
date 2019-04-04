#!/bin/bash

function indentAndSlash {
	cat | sort | \
		sed "s/^.\\//	/" |	\
		sed "s/$/ \\\\/" |		\
		sed "\$s/\\\\//"
}

function doSrc {
	cat <<-EOF
	CLEANFILES =
	EXTRA_DIST =
	pkgconfigdir = \$(libdir)/pkgconfig
	pkgconfig_DATA = lib/pkgconfig/mod.pc

	bin_PROGRAMS =
	dist_bin_SCRIPTS = \\
		bin/mod
	pkgdata_DATA = 

	dist_pkgdata_DATA = \\
		share/mod/obabel.supp \\
		share/mod/python.supp
	pluginsdir = \$(pkgdatadir)/plugins
	plugins_DATA = share/mod/plugins/00_mod
	EXTRA_DIST += share/mod/plugins/00_mod.in

	# http://www.gnu.org/software/automake/manual/html_node/Scripts.html#Scripts
	share/mod/plugins/00_mod: \$(srcdir)/share/mod/plugins/00_mod.in
EOF
	echo '	mkdir -p share/mod/plugins'
	echo '	$(SED) \'
	echo "		-e 's|[@]libdir@|\$(libdir)|g' \\"
#	echo "		-e 's|[@]exec_prefix@|\$(exec_prefix)|g' \\"
	echo '		< "$<" > "$@"'

cat <<-EOF
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
	CLEANFILES += share/mod/commonPreamble.fmt
	
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
	libmod_la_LDFLAGS = \$(AM_LDFLAGS) -no-undefined -pthread -release \$(PACKAGE_VERSION)
	libmod_la_LIBADD =  \$(AM_LDLIBS) -lboost_regex -lboost_system

	if HAVE_OPENBABEL
	libmod_la_LIBADD += -lopenbabel
	endif
	libmod_la_LIBADD += -lpthread

	nobase_nodist_include_HEADERS = \\
		mod/BuildConfig.h
	nobase_include_HEADERS = \\
	EOF
	(
		find . -name "*.h" | grep mod/ | grep -v mod/Py/ 
		find . -name "*.hpp" | grep mod/ | grep -v mod/Py/ 
		find . -name "*.hpp" | grep jla_boost/
		find . -name "*.hpp" | grep gml/
	) | indentAndSlash 
	echo "libmod_la_SOURCES = \\"
	(
		find . -name "*.cpp" | grep mod/ | grep -v mod/Py/ 
		find . -name "*.cpp" | grep jla_boost/
		find . -name "*.cpp" | grep gml/
	) | indentAndSlash 

	cat <<-EOF
	# PyMOD
	if ENABLE_PYMOD
	pkglib_LTLIBRARIES = mod_.la
	mod__la_CPPFLAGS = \$(AM_CPPFLAGS)
	mod__la_CXXFLAGS = \$(AM_CXXFLAGS)
	mod__la_LDFLAGS = \$(AM_LDFLAGS) -module -release \$(PACKAGE_VERSION)
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
	echo "dist_hax_DATA = \\"
	find lib/mod -name "*.py" | indentAndSlash
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
autoreconf -fi
