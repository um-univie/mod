#!/bin/bash
export CPPFLAGS=-I/usr/include/openbabel-2.0
export LIBRARY_PATH=/home/jla/ILOG_CPLEX/concert/lib/x86-64_linux/static_pic:/home/jla/ILOG_CPLEX/cplex/lib/x86-64_linux/static_pic
cd NB_mod
make clean
make -j 8 || exit $?
cd ..
cd NB_pymod
make clean
make -j 8 || exit $?
