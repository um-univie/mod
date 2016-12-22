#!/bin/bash

# the main logic for building is based on the auto-generated Makefile from Sphinx

sphinxBuild=${1:-sphinx-build}
topSrcDir=${2:-..}
topBuildDir=${3:-..}

which $sphinxBuild &> /dev/null
if [ $? -ne 0 ]; then
	echo "Error: '$sphinxBuild' was not found."
	exit 1
fi

function doBuild {
	allOpts="-d $topBuildDir/doc/doctrees $topSrcDir/doc/source"
	mkdir -p $topSrcDir/doc/source/_static
	$sphinxBuild -b html $allOpts $topBuildDir/doc/build/html
	echo "$sphinxBuild -b html $allOpts $topBuildDir/doc/build/html"
}

function outputRST {
	cat | sed "s/	/    /g" > $topSrcDir/doc/source/$1.rst
}

function filterCPP {
	cat | awk '
BEGIN {
	inClass = 0
	lineNum = 0
	inNestedClass = 0
	nestedLineNum = 0
	lastClass = ""
	lastNested = ""
}
{
	if($0 ~ /^[\t]*\/\/ rst: .. py:class::/) {
		sub(/^[\t]*\/\/ rst: .. py:class:: /, "")
		print ""
		print "Class ``" $0 "``"
		print "--------------------------------------------------------------------------------------------------------------------------------"
		print ""
		print ".. py:class:: " $0
	} else if($0 ~ /^[\t]*\/\/ rst:/) {
		sub(/^[\t]*\/\/ rst:[ 	]?/, "")
		if(inClass)
			if(inNested)
				nestedRST[nestedLineNum++] = $0
			else
				normalRST[lineNum++] = $0
		else
			print
	} else if($0 ~ /^[\t]*\/\* rst:/) {
		sub(/^[\t]*\/\* rst: ?/, "")
		sub(/\*\/[\t]*\\?/, "")
		if(inClass)
			if(inNested)
				nestedRST[nestedLineNum++] = $0
			else
				normalRST[lineNum++] = $0
		else
			print
	} else if($0 ~ /^[\t]*\/\/ rst-class:/) {
		sub(/^[\t]*\/\/ rst-class: /, "")
		lastClass = $0
		sub(/ : .*/, "", lastClass)
		print ""
		print "Class ``" lastClass "``"
		print "--------------------------------------------------------------------------------------------------------------------------------"
		print ""
		print ".. class:: " $0
		print "    "
	} else if($0 ~ /^[\t]*\/\/ rst-class-start:/) {
		inClass	= 1
		print ""
		print "Synopsis"
		print "^^^^^^^^"
		print ""
		print ".. code-block:: c++"
		print "    "
	} else if($0 ~/^[\t]*\/\/ rst-class-end:/) {
		inClass = 0
		if(lineNum > 0) {
			print ""
			print "Details"
			print "^^^^^^^"
			print ""
			print ".. cpp:namespace:: mod"
			print ""
			print ".. cpp:namespace-push:: " lastClass
			print ""
			for(i = 0; i < lineNum; i++) print normalRST[i]
			print ""
			print ".. cpp:namespace:: mod"
			print ""
			lineNum = 0
		}
		if(nestedLineNum > 0) {
			for(i = 0; i < nestedLineNum; i++) print nestedRST[i]
			print ""
			nestedLineNum = 0
		}
	} else if($0 ~/^[\t]*\/\/ rst-nested:/) {
		sub(/^[\t]*\/\/ rst-nested: /, "")
		if(!inClass) {
			print "Nested class outside class!" | "cat 1>&2"
			exit 1
		}
		inNested = 1
		nestedRST[nestedLineNum++] = ""
		nestedRST[nestedLineNum++] = "Class ``" $0 "``"
		nestedRST[nestedLineNum++] = "---------------------------------------------------------------"
		nestedRST[nestedLineNum++] = ""
		nestedRST[nestedLineNum++] = ".. class:: " $0
		nestedRST[nestedLineNum++] = ""
		lastNested = $0
	} else if($0 ~/^[\t]*\/\/ rst-nested-start:/) {
		nestedRST[nestedLineNum++] = ""
	} else if($0 ~/^[\t]*\/\/ rst-nested-end:/) {
		nestedRST[nestedLineNum++] = ""
		inNested = 0
	} else if(inClass) {
		if(!($0 ~/^[\t]*$/))
			print "\t"$0
	}
}
'
}

function getModHeaders {
	ls $topSrcDir/src/mod/*.h | sed \
		-e "s/.*\/src\/mod\///" -e "s/\.h$//" | sort
}

function getPyModCPPs {
	ls $topSrcDir/src/mod/Py/*.cpp | sed \
		-e "s/.*\/src\/mod\/Py\///" -e "s/\.cpp$//" | sort \
		| grep -v Module | grep -v Collections | grep -v Function
}

function makeIndex {
	function indexFiles {
		cat << "EOF"
	installation
	libmod/libmod
	pymod/pymod
	postmod/postmod
	modWrapper/modWrapper
	dataDesc/dataDesc
	dgStrat/dgStrat
	
	references
EOF
	}
	function data {
		cat << "EOF"
################################################
MedØlDatschgerl
################################################

.. toctree::
	:maxdepth: 1
	:numbered:
	
EOF
		indexFiles
cat << "EOF"


.. _overview:

Overview
========

This is the documentation for the MedØlDatschgerl (MØD) software package.
For additional information see the webpage: `<http://mod.imada.sdu.dk>`_.

The package contains the following components.

* libMØD, shared library (``mod``)
    The main library.
* PyMØD, Python 3 module (``mod``)
    Python bindings for the library and extra functionality for easier usage of
    many features.
*  PostMØD, Bash Script (``mod_post``)
    The post processor for compiling figures and summaries.
* The wrapper script, Bash Script (``mod``)
    A convenience wrapper for invoking a virtual machine (e.g., ``python3``)
    with user-supplied code.
    The wrapper handles output and invokes the post processor.
    Additionally, it can run the chosen virtual machine through ``gdb``
    and/or ``valgrind`` (either normal memcheck or callgrind).


Indices and Tables
==================

* :ref:`genindex`
EOF
cat << "EOF"

Table of Contents
=================

.. toctree::
	:maxdepth: 4
	:numbered:
	
EOF
		indexFiles
	}
	data | outputRST index
}

function makeLibMod {
	function data {
		local f=$1
		echo ".. _cpp-$f:"
		echo ""
		echo "**********************************************************"
		echo "$f.h"
		echo "**********************************************************"
		cat << "EOF"
.. default-domain:: cpp

.. py:currentmodule:: mod
.. cpp:namespace:: mod

EOF
		cat $topSrcDir/src/mod/$f.h | filterCPP
	}
	for f in $(getModHeaders); do
		data $f | outputRST libmod/$f
	done
	function dataToc {
		cat << "EOF"
.. toctree::
	:maxdepth: 2

EOF
		getModHeaders | sed 's/^/	/'
	}
	dataToc | outputRST libmod/Libmodtoc	
}

function makePyMod {
	function data {
		local f=$1
		echo ".. _py-$f:"
		echo ""
		echo "**********************************************************"
		echo "$f"
		echo "**********************************************************"
		cat << "EOF"
.. default-domain:: py

.. py:currentmodule:: mod
.. cpp:namespace:: mod

EOF
		cat $topSrcDir/src/mod/Py/$f.cpp | filterCPP
	}
	for f in $(getPyModCPPs); do
		data $f | outputRST pymod/$f
	done
	function dataToc {
		cat << "EOF"
.. toctree::
	:maxdepth: 2

EOF
		getPyModCPPs | sed 's/^/	/'
	}
	dataToc | outputRST pymod/Pymodtoc	
}

makeIndex
makeLibMod
makePyMod
doBuild
