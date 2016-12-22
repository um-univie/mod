#!/bin/bash
export AS_RLIMIT=300000000
root_PWD=$(pwd)
numThreads=2
doBuild="yes"
withOpenBabel="yes"
prefix="--prefix=$HOME/test/modTestInstall"
while true; do
	case $1 in
	-j)
		if [ "x$2" = "x" ]; then
			echo "Missing argument for '$1'"
			exit 1
		fi
		numThreads=$2
		shift
		shift
		;;
	"nooptional")
		withOpenBabel="no"
		shift
		;;
	"noobabel")
		withOpenBabel="no"
		shift
		;;
	"nobuild")
		doBuild="no"
		shift
		;;
	"--prefix="*)
		prefix=$1
		shift
		;;
	"")
		break
		;;
	*)
		echo "Unknown option '$1'"
		exit 1
	esac
done

args=""
if test "$withOpenBabel" = "yes"; then
	#args="$args	--with-OpenBabel=$HOME/programs"
	# Use the repo version now.
	args="$args	--with-OpenBabel=yes"
else
	args="$args --with-OpenBabel=no"
fi
args="$args	--with-boost=$HOME/programs$suffix"
args="$args	$prefix $@"

./repo-bootstrap.sh								\
	&& rm -rf preBuild && mkdir preBuild		\
	&& cd preBuild && ../configure $args		\
	&& make dist								\
	&& cd .. && rm -rf build && mkdir build		\
	&& cd build									\
	&& cp ../preBuild/mod-*.tar.gz ./			\
	&& tar xzf mod-*.tar.gz	&& cd mod-*			\
	&& mkdir build && cd build					\
	&& ../configure $args
res=$?
if [ $res -ne 0 ]; then
	echo "Error during configuration"
	exit $res
fi
if [ "$doBuild" = "no" ]; then
	echo "Not building due to user request"
else
	time make -j $numThreads						\
		&& rm -rf $HOME/test/modTestInstall			\
		&& make install
	res=$?
	if [ $res -ne 0 ]; then
		echo "Error during installation"
		exit $res
	fi
	function makeSource {
		cd $root_PWD/src
		for f in $(ls mod/*.h | grep -v BuildConfig.h); do
			echo "#include <$f>"
		done
		echo ""
		echo "#include <iostream>"
		echo ""
		echo "int main() {"
		echo "	auto g = mod::Graph::graphDFS(\"[T]\");"
		echo "	std::cout << \"Graph name:	\" << g->getName() << std::endl;"
		echo "	return 0;"
		echo "}"
	}
	function makeMakefile {
		echo 'PKG_CONFIG_PATH := $(PKG_CONFIG_PATH):$(HOME)/test/modTestInstall/lib/pkgconfig'
		echo 'CPPFLAGS        := $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags mod)'
		echo 'CXXFLAGS        := -std=c++14'
		echo 'LDLIBS          := $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs mod)'
		echo ""
		echo "test: test.o"
		echo '	$(CXX) -o test test.o $(LDLIBS)'
	}
	rm -rf $HOME/test/modTestCompile
	mkdir $HOME/test/modTestCompile
	makeSource > $HOME/test/modTestCompile/test.cpp
	makeMakefile > $HOME/test/modTestCompile/Makefile
	cd $HOME/test/modTestCompile
	echo "Test program:"
	echo "======================================================================"
	cat test.cpp
	echo "======================================================================"
	echo "Makefile:"
	echo "======================================================================"
	cat Makefile
	echo "======================================================================"
	make && ./test
	res=$?
	if [ $res -ne 0 ]; then
		echo "Compilation or run of test program failed."
		exit $res
	fi
	echo "Test wrapper"
	echo "======================================================================"
	$HOME/test/modTestInstall/bin/mod -e "smiles('O').print()"
	res=$?
	if [ $res -ne 0 ]; then
		echo "Running the wrapper script failed."
		exit $res
	fi
fi
cd $root_PWD/build/mod-*
cd build
make install-doc
