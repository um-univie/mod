#!/bin/bash
# Create an "installation" of the Python parts by symlinking them into
# build/checkPython/mod, and then run the Python tools.
root=$(git rev-parse --show-toplevel)
res=$?
if [ $res -ne 0 ]; then
	exit $res
fi
cd $root
mkdir -p build/checkPython/mod
rm build/checkPython/mod/*
for f in $(ls libs/pymod/lib/mod); do
	ln -s -T ../../../libs/pymod/lib/mod/$f build/checkPython/mod/$f
done
ln -s -T ../../../libs/epim/lib/mod/epim build/checkPython/mod/epim

mypy \
	--warn-redundant-casts \
	--warn-unused-ignores  \
	build/checkPython/mod
