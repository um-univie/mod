#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=time g++
CXX=time g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/f98eca90/Chem.o \
	${OBJECTDIR}/_ext/f98eca90/Collections.o \
	${OBJECTDIR}/_ext/f98eca90/Config.o \
	${OBJECTDIR}/_ext/f98eca90/DG.o \
	${OBJECTDIR}/_ext/f98eca90/DGGraphInterface.o \
	${OBJECTDIR}/_ext/f98eca90/DGPrinter.o \
	${OBJECTDIR}/_ext/f98eca90/DGStrat.o \
	${OBJECTDIR}/_ext/f98eca90/Derivation.o \
	${OBJECTDIR}/_ext/f98eca90/Error.o \
	${OBJECTDIR}/_ext/f98eca90/Function.o \
	${OBJECTDIR}/_ext/f98eca90/Graph.o \
	${OBJECTDIR}/_ext/f98eca90/GraphGraphInterface.o \
	${OBJECTDIR}/_ext/f98eca90/GraphPrinter.o \
	${OBJECTDIR}/_ext/f98eca90/Misc.o \
	${OBJECTDIR}/_ext/f98eca90/Module.o \
	${OBJECTDIR}/_ext/f98eca90/RC.o \
	${OBJECTDIR}/_ext/f98eca90/Rule.o \
	${OBJECTDIR}/_ext/f98eca90/RuleGraphInterface.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-O3 -fno-strict-aliasing -fPIC -Wall -Wextra -Wno-unused-local-typedefs -Wno-comment -Wno-deprecated-declarations
CXXFLAGS=-O3 -fno-strict-aliasing -fPIC -Wall -Wextra -Wno-unused-local-typedefs -Wno-comment -Wno-deprecated-declarations

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L${HOME}/programs/lib -Wl,-rpath,${HOME}/stuff/code/mod/NB_mod/dist/lib -Wl,-rpath,${HOME}/programs/lib -Wl,-Bdynamic -Wl,-rpath,../NB_mod/dist/lib -L../NB_mod/dist/lib -lmod -lboost_python3 `pkg-config --libs python3`  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk dist/lib/mod/mod_.so

dist/lib/mod/mod_.so: ../NB_mod/dist/lib/libmod.so

dist/lib/mod/mod_.so: ${OBJECTFILES}
	${MKDIR} -p dist/lib/mod
	g++ -o dist/lib/mod/mod_.so ${OBJECTFILES} ${LDLIBSOPTIONS} -rdynamic -Wl,--no-undefined -shared -fPIC

${OBJECTDIR}/_ext/f98eca90/Chem.o: ../src/mod/Py/Chem.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eca90
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3=1 -I${HOME}/programs/include -I${CND_BASEDIR}/../srcExtra -I${CND_BASEDIR}/../src -I/usr/include/python3.3m -I/usr/include/python3.2mu `pkg-config --cflags python3` -std=c++14  -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eca90/Chem.o ../src/mod/Py/Chem.cpp

${OBJECTDIR}/_ext/f98eca90/Collections.o: ../src/mod/Py/Collections.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eca90
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3=1 -I${HOME}/programs/include -I${CND_BASEDIR}/../srcExtra -I${CND_BASEDIR}/../src -I/usr/include/python3.3m -I/usr/include/python3.2mu `pkg-config --cflags python3` -std=c++14  -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eca90/Collections.o ../src/mod/Py/Collections.cpp

${OBJECTDIR}/_ext/f98eca90/Config.o: ../src/mod/Py/Config.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eca90
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3=1 -I${HOME}/programs/include -I${CND_BASEDIR}/../srcExtra -I${CND_BASEDIR}/../src -I/usr/include/python3.3m -I/usr/include/python3.2mu `pkg-config --cflags python3` -std=c++14  -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eca90/Config.o ../src/mod/Py/Config.cpp

${OBJECTDIR}/_ext/f98eca90/DG.o: ../src/mod/Py/DG.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eca90
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3=1 -I${HOME}/programs/include -I${CND_BASEDIR}/../srcExtra -I${CND_BASEDIR}/../src -I/usr/include/python3.3m -I/usr/include/python3.2mu `pkg-config --cflags python3` -std=c++14  -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eca90/DG.o ../src/mod/Py/DG.cpp

${OBJECTDIR}/_ext/f98eca90/DGGraphInterface.o: ../src/mod/Py/DGGraphInterface.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eca90
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3=1 -I${HOME}/programs/include -I${CND_BASEDIR}/../srcExtra -I${CND_BASEDIR}/../src -I/usr/include/python3.3m -I/usr/include/python3.2mu `pkg-config --cflags python3` -std=c++14  -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eca90/DGGraphInterface.o ../src/mod/Py/DGGraphInterface.cpp

${OBJECTDIR}/_ext/f98eca90/DGPrinter.o: ../src/mod/Py/DGPrinter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eca90
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3=1 -I${HOME}/programs/include -I${CND_BASEDIR}/../srcExtra -I${CND_BASEDIR}/../src -I/usr/include/python3.3m -I/usr/include/python3.2mu `pkg-config --cflags python3` -std=c++14  -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eca90/DGPrinter.o ../src/mod/Py/DGPrinter.cpp

${OBJECTDIR}/_ext/f98eca90/DGStrat.o: ../src/mod/Py/DGStrat.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eca90
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3=1 -I${HOME}/programs/include -I${CND_BASEDIR}/../srcExtra -I${CND_BASEDIR}/../src -I/usr/include/python3.3m -I/usr/include/python3.2mu `pkg-config --cflags python3` -std=c++14  -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eca90/DGStrat.o ../src/mod/Py/DGStrat.cpp

${OBJECTDIR}/_ext/f98eca90/Derivation.o: ../src/mod/Py/Derivation.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eca90
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3=1 -I${HOME}/programs/include -I${CND_BASEDIR}/../srcExtra -I${CND_BASEDIR}/../src -I/usr/include/python3.3m -I/usr/include/python3.2mu `pkg-config --cflags python3` -std=c++14  -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eca90/Derivation.o ../src/mod/Py/Derivation.cpp

${OBJECTDIR}/_ext/f98eca90/Error.o: ../src/mod/Py/Error.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eca90
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3=1 -I${HOME}/programs/include -I${CND_BASEDIR}/../srcExtra -I${CND_BASEDIR}/../src -I/usr/include/python3.3m -I/usr/include/python3.2mu `pkg-config --cflags python3` -std=c++14  -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eca90/Error.o ../src/mod/Py/Error.cpp

${OBJECTDIR}/_ext/f98eca90/Function.o: ../src/mod/Py/Function.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eca90
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3=1 -I${HOME}/programs/include -I${CND_BASEDIR}/../srcExtra -I${CND_BASEDIR}/../src -I/usr/include/python3.3m -I/usr/include/python3.2mu `pkg-config --cflags python3` -std=c++14  -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eca90/Function.o ../src/mod/Py/Function.cpp

${OBJECTDIR}/_ext/f98eca90/Graph.o: ../src/mod/Py/Graph.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eca90
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3=1 -I${HOME}/programs/include -I${CND_BASEDIR}/../srcExtra -I${CND_BASEDIR}/../src -I/usr/include/python3.3m -I/usr/include/python3.2mu `pkg-config --cflags python3` -std=c++14  -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eca90/Graph.o ../src/mod/Py/Graph.cpp

${OBJECTDIR}/_ext/f98eca90/GraphGraphInterface.o: ../src/mod/Py/GraphGraphInterface.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eca90
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3=1 -I${HOME}/programs/include -I${CND_BASEDIR}/../srcExtra -I${CND_BASEDIR}/../src -I/usr/include/python3.3m -I/usr/include/python3.2mu `pkg-config --cflags python3` -std=c++14  -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eca90/GraphGraphInterface.o ../src/mod/Py/GraphGraphInterface.cpp

${OBJECTDIR}/_ext/f98eca90/GraphPrinter.o: ../src/mod/Py/GraphPrinter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eca90
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3=1 -I${HOME}/programs/include -I${CND_BASEDIR}/../srcExtra -I${CND_BASEDIR}/../src -I/usr/include/python3.3m -I/usr/include/python3.2mu `pkg-config --cflags python3` -std=c++14  -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eca90/GraphPrinter.o ../src/mod/Py/GraphPrinter.cpp

${OBJECTDIR}/_ext/f98eca90/Misc.o: ../src/mod/Py/Misc.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eca90
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3=1 -I${HOME}/programs/include -I${CND_BASEDIR}/../srcExtra -I${CND_BASEDIR}/../src -I/usr/include/python3.3m -I/usr/include/python3.2mu `pkg-config --cflags python3` -std=c++14  -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eca90/Misc.o ../src/mod/Py/Misc.cpp

${OBJECTDIR}/_ext/f98eca90/Module.o: ../src/mod/Py/Module.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eca90
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3=1 -I${HOME}/programs/include -I${CND_BASEDIR}/../srcExtra -I${CND_BASEDIR}/../src -I/usr/include/python3.3m -I/usr/include/python3.2mu `pkg-config --cflags python3` -std=c++14  -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eca90/Module.o ../src/mod/Py/Module.cpp

${OBJECTDIR}/_ext/f98eca90/RC.o: ../src/mod/Py/RC.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eca90
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3=1 -I${HOME}/programs/include -I${CND_BASEDIR}/../srcExtra -I${CND_BASEDIR}/../src -I/usr/include/python3.3m -I/usr/include/python3.2mu `pkg-config --cflags python3` -std=c++14  -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eca90/RC.o ../src/mod/Py/RC.cpp

${OBJECTDIR}/_ext/f98eca90/Rule.o: ../src/mod/Py/Rule.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eca90
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3=1 -I${HOME}/programs/include -I${CND_BASEDIR}/../srcExtra -I${CND_BASEDIR}/../src -I/usr/include/python3.3m -I/usr/include/python3.2mu `pkg-config --cflags python3` -std=c++14  -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eca90/Rule.o ../src/mod/Py/Rule.cpp

${OBJECTDIR}/_ext/f98eca90/RuleGraphInterface.o: ../src/mod/Py/RuleGraphInterface.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eca90
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3=1 -I${HOME}/programs/include -I${CND_BASEDIR}/../srcExtra -I${CND_BASEDIR}/../src -I/usr/include/python3.3m -I/usr/include/python3.2mu `pkg-config --cflags python3` -std=c++14  -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eca90/RuleGraphInterface.o ../src/mod/Py/RuleGraphInterface.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} dist/lib/mod/mod_.so

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
