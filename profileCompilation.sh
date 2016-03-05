#!/bin/bash
OBJECTDIR="build/Debug/GNU-Linux-x86"

function getFiles {
	cd NB_mod
	make -pn |& grep "^OBJECTFILES =" | sed "s/^OBJECTFILES = /OBJECTFILES=/" | sort | uniq
}

function compile {
	f="$1"
	cd NB_mod
	rm -f $f
	output=$(make -f nbproject/Makefile-Debug.mk $f 2>&1)
	name=$(echo "$output" | grep g++ | awk '{print $NF}')
	t=$(echo "$output" | grep CPU)
	echo -e "$name\t$t"
	cd ..
}

files="$(getFiles)"
for f in $files; do
	base=$(basename $f)
	f=$(echo $f | sed "s!\${OBJECTDIR}!$OBJECTDIR!")
	if [ "x$1" != "x" ]; then
		if [ "$base" = "$1" ]; then
			compile $f
		fi
	else
		compile $f
	fi
done
