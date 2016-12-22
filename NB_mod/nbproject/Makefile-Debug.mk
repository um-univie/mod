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
	${OBJECTDIR}/_ext/27549c4c/parser.o \
	${OBJECTDIR}/_ext/9729d8dc/IO.o \
	${OBJECTDIR}/_ext/2754b308/Chem.o \
	${OBJECTDIR}/_ext/2754b308/Config.o \
	${OBJECTDIR}/_ext/2754b308/DG.o \
	${OBJECTDIR}/_ext/2754b308/DGGraphInterface.o \
	${OBJECTDIR}/_ext/2754b308/DGPrinter.o \
	${OBJECTDIR}/_ext/2754b308/DGStrat.o \
	${OBJECTDIR}/_ext/2754b308/Derivation.o \
	${OBJECTDIR}/_ext/2754b308/Error.o \
	${OBJECTDIR}/_ext/2754b308/Function.o \
	${OBJECTDIR}/_ext/2754b308/Graph.o \
	${OBJECTDIR}/_ext/2754b308/GraphGraphInterface.o \
	${OBJECTDIR}/_ext/2754b308/GraphPrinter.o \
	${OBJECTDIR}/_ext/2754b308/Misc.o \
	${OBJECTDIR}/_ext/2754b308/RC.o \
	${OBJECTDIR}/_ext/2754b308/Rule.o \
	${OBJECTDIR}/_ext/2754b308/RuleGraphInterface.o \
	${OBJECTDIR}/_ext/3b7f185e/MoleculeUtil.o \
	${OBJECTDIR}/_ext/3b7f185e/OBabel.o \
	${OBJECTDIR}/_ext/3b7f185e/Smiles.o \
	${OBJECTDIR}/_ext/3b7f185e/SmilesRead.o \
	${OBJECTDIR}/_ext/3b7f185e/SmilesWrite.o \
	${OBJECTDIR}/_ext/d81a8134/Dump.o \
	${OBJECTDIR}/_ext/d81a8134/Hyper.o \
	${OBJECTDIR}/_ext/d81a8134/NonHyper.o \
	${OBJECTDIR}/_ext/d81a8134/NonHyperDerivations.o \
	${OBJECTDIR}/_ext/d81a8134/NonHyperRuleComp.o \
	${OBJECTDIR}/_ext/da13872c/Add.o \
	${OBJECTDIR}/_ext/da13872c/DerivationPredicates.o \
	${OBJECTDIR}/_ext/da13872c/Execute.o \
	${OBJECTDIR}/_ext/da13872c/Filter.o \
	${OBJECTDIR}/_ext/da13872c/GraphState.o \
	${OBJECTDIR}/_ext/da13872c/Parallel.o \
	${OBJECTDIR}/_ext/da13872c/Repeat.o \
	${OBJECTDIR}/_ext/da13872c/Revive.o \
	${OBJECTDIR}/_ext/da13872c/Rule.o \
	${OBJECTDIR}/_ext/da13872c/Sequence.o \
	${OBJECTDIR}/_ext/da13872c/Sort.o \
	${OBJECTDIR}/_ext/da13872c/Strategy.o \
	${OBJECTDIR}/_ext/da13872c/Take.o \
	${OBJECTDIR}/_ext/34a0cedd/DFSEncoding.o \
	${OBJECTDIR}/_ext/34a0cedd/LabelledGraph.o \
	${OBJECTDIR}/_ext/34a0cedd/Merge.o \
	${OBJECTDIR}/_ext/d46a6fc5/Depiction.o \
	${OBJECTDIR}/_ext/d46a6fc5/Molecule.o \
	${OBJECTDIR}/_ext/34a0cedd/Single.o \
	${OBJECTDIR}/_ext/d81a81d7/DGRead.o \
	${OBJECTDIR}/_ext/d81a81d7/DGWrite.o \
	${OBJECTDIR}/_ext/d81a81d7/Derivation.o \
	${OBJECTDIR}/_ext/d81a81d7/FileHandle.o \
	${OBJECTDIR}/_ext/d81a81d7/GMLUtil.o \
	${OBJECTDIR}/_ext/d81a81d7/GraphRead.o \
	${OBJECTDIR}/_ext/d81a81d7/GraphWrite.o \
	${OBJECTDIR}/_ext/d81a81d7/IO.o \
	${OBJECTDIR}/_ext/d81a81d7/ParserCommon.o \
	${OBJECTDIR}/_ext/d81a81d7/RC.o \
	${OBJECTDIR}/_ext/d81a81d7/RuleRead.o \
	${OBJECTDIR}/_ext/d81a81d7/RuleWrite.o \
	${OBJECTDIR}/_ext/d81a82e2/ComposeRuleReal.o \
	${OBJECTDIR}/_ext/d81a82e2/Evaluator.o \
	${OBJECTDIR}/_ext/384aeefe/Random.o \
	${OBJECTDIR}/_ext/353d5686/LabelledRule.o \
	${OBJECTDIR}/_ext/5cc9b47c/Depiction.o \
	${OBJECTDIR}/_ext/5cc9b47c/Molecule.o \
	${OBJECTDIR}/_ext/353d5686/Real.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-O3 -fno-strict-aliasing -fPIC -pthread -Wall -Wextra -Wno-unused-local-typedefs -Wno-unused-parameter -Wno-comment -Wno-maybe-uninitialized -Wno-deprecated-declarations -Wno-sign-compare
CXXFLAGS=-O3 -fno-strict-aliasing -fPIC -pthread -Wall -Wextra -Wno-unused-local-typedefs -Wno-unused-parameter -Wno-comment -Wno-maybe-uninitialized -Wno-deprecated-declarations -Wno-sign-compare

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L${HOME}/programs/lib -Wl,-rpath,${HOME}/programs/lib -Wl,-Bdynamic -lboost_regex -lboost_system -lopenbabel -pthread -Wl,-Bdynamic

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk dist/lib/libmod.${CND_DLIB_EXT}

dist/lib/libmod.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p dist/lib
	${LINK.cc} -o dist/lib/libmod.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -rdynamic -Wl,--no-undefined -shared -fPIC

${OBJECTDIR}/_ext/27549c4c/parser.o: ../src/gml/parser.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/27549c4c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/27549c4c/parser.o ../src/gml/parser.cpp

${OBJECTDIR}/_ext/9729d8dc/IO.o: ../src/jla_boost/graph/dpo/IO.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/9729d8dc
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9729d8dc/IO.o ../src/jla_boost/graph/dpo/IO.cpp

${OBJECTDIR}/_ext/2754b308/Chem.o: ../src/mod/Chem.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/Chem.o ../src/mod/Chem.cpp

${OBJECTDIR}/_ext/2754b308/Config.o: ../src/mod/Config.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/Config.o ../src/mod/Config.cpp

${OBJECTDIR}/_ext/2754b308/DG.o: ../src/mod/DG.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/DG.o ../src/mod/DG.cpp

${OBJECTDIR}/_ext/2754b308/DGGraphInterface.o: ../src/mod/DGGraphInterface.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/DGGraphInterface.o ../src/mod/DGGraphInterface.cpp

${OBJECTDIR}/_ext/2754b308/DGPrinter.o: ../src/mod/DGPrinter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/DGPrinter.o ../src/mod/DGPrinter.cpp

${OBJECTDIR}/_ext/2754b308/DGStrat.o: ../src/mod/DGStrat.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/DGStrat.o ../src/mod/DGStrat.cpp

${OBJECTDIR}/_ext/2754b308/Derivation.o: ../src/mod/Derivation.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/Derivation.o ../src/mod/Derivation.cpp

${OBJECTDIR}/_ext/2754b308/Error.o: ../src/mod/Error.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/Error.o ../src/mod/Error.cpp

${OBJECTDIR}/_ext/2754b308/Function.o: ../src/mod/Function.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/Function.o ../src/mod/Function.cpp

${OBJECTDIR}/_ext/2754b308/Graph.o: ../src/mod/Graph.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/Graph.o ../src/mod/Graph.cpp

${OBJECTDIR}/_ext/2754b308/GraphGraphInterface.o: ../src/mod/GraphGraphInterface.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/GraphGraphInterface.o ../src/mod/GraphGraphInterface.cpp

${OBJECTDIR}/_ext/2754b308/GraphPrinter.o: ../src/mod/GraphPrinter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/GraphPrinter.o ../src/mod/GraphPrinter.cpp

${OBJECTDIR}/_ext/2754b308/Misc.o: ../src/mod/Misc.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/Misc.o ../src/mod/Misc.cpp

${OBJECTDIR}/_ext/2754b308/RC.o: ../src/mod/RC.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/RC.o ../src/mod/RC.cpp

${OBJECTDIR}/_ext/2754b308/Rule.o: ../src/mod/Rule.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/Rule.o ../src/mod/Rule.cpp

${OBJECTDIR}/_ext/2754b308/RuleGraphInterface.o: ../src/mod/RuleGraphInterface.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/RuleGraphInterface.o ../src/mod/RuleGraphInterface.cpp

${OBJECTDIR}/_ext/3b7f185e/MoleculeUtil.o: ../src/mod/lib/Chem/MoleculeUtil.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/3b7f185e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3b7f185e/MoleculeUtil.o ../src/mod/lib/Chem/MoleculeUtil.cpp

${OBJECTDIR}/_ext/3b7f185e/OBabel.o: ../src/mod/lib/Chem/OBabel.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/3b7f185e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3b7f185e/OBabel.o ../src/mod/lib/Chem/OBabel.cpp

${OBJECTDIR}/_ext/3b7f185e/Smiles.o: ../src/mod/lib/Chem/Smiles.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/3b7f185e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3b7f185e/Smiles.o ../src/mod/lib/Chem/Smiles.cpp

${OBJECTDIR}/_ext/3b7f185e/SmilesRead.o: ../src/mod/lib/Chem/SmilesRead.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/3b7f185e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3b7f185e/SmilesRead.o ../src/mod/lib/Chem/SmilesRead.cpp

${OBJECTDIR}/_ext/3b7f185e/SmilesWrite.o: ../src/mod/lib/Chem/SmilesWrite.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/3b7f185e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3b7f185e/SmilesWrite.o ../src/mod/lib/Chem/SmilesWrite.cpp

${OBJECTDIR}/_ext/d81a8134/Dump.o: ../src/mod/lib/DG/Dump.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a8134
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a8134/Dump.o ../src/mod/lib/DG/Dump.cpp

${OBJECTDIR}/_ext/d81a8134/Hyper.o: ../src/mod/lib/DG/Hyper.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a8134
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a8134/Hyper.o ../src/mod/lib/DG/Hyper.cpp

${OBJECTDIR}/_ext/d81a8134/NonHyper.o: ../src/mod/lib/DG/NonHyper.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a8134
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a8134/NonHyper.o ../src/mod/lib/DG/NonHyper.cpp

${OBJECTDIR}/_ext/d81a8134/NonHyperDerivations.o: ../src/mod/lib/DG/NonHyperDerivations.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a8134
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a8134/NonHyperDerivations.o ../src/mod/lib/DG/NonHyperDerivations.cpp

${OBJECTDIR}/_ext/d81a8134/NonHyperRuleComp.o: ../src/mod/lib/DG/NonHyperRuleComp.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a8134
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a8134/NonHyperRuleComp.o ../src/mod/lib/DG/NonHyperRuleComp.cpp

${OBJECTDIR}/_ext/da13872c/Add.o: ../src/mod/lib/DG/Strategies/Add.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Add.o ../src/mod/lib/DG/Strategies/Add.cpp

${OBJECTDIR}/_ext/da13872c/DerivationPredicates.o: ../src/mod/lib/DG/Strategies/DerivationPredicates.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/DerivationPredicates.o ../src/mod/lib/DG/Strategies/DerivationPredicates.cpp

${OBJECTDIR}/_ext/da13872c/Execute.o: ../src/mod/lib/DG/Strategies/Execute.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Execute.o ../src/mod/lib/DG/Strategies/Execute.cpp

${OBJECTDIR}/_ext/da13872c/Filter.o: ../src/mod/lib/DG/Strategies/Filter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Filter.o ../src/mod/lib/DG/Strategies/Filter.cpp

${OBJECTDIR}/_ext/da13872c/GraphState.o: ../src/mod/lib/DG/Strategies/GraphState.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/GraphState.o ../src/mod/lib/DG/Strategies/GraphState.cpp

${OBJECTDIR}/_ext/da13872c/Parallel.o: ../src/mod/lib/DG/Strategies/Parallel.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Parallel.o ../src/mod/lib/DG/Strategies/Parallel.cpp

${OBJECTDIR}/_ext/da13872c/Repeat.o: ../src/mod/lib/DG/Strategies/Repeat.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Repeat.o ../src/mod/lib/DG/Strategies/Repeat.cpp

${OBJECTDIR}/_ext/da13872c/Revive.o: ../src/mod/lib/DG/Strategies/Revive.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Revive.o ../src/mod/lib/DG/Strategies/Revive.cpp

${OBJECTDIR}/_ext/da13872c/Rule.o: ../src/mod/lib/DG/Strategies/Rule.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Rule.o ../src/mod/lib/DG/Strategies/Rule.cpp

${OBJECTDIR}/_ext/da13872c/Sequence.o: ../src/mod/lib/DG/Strategies/Sequence.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Sequence.o ../src/mod/lib/DG/Strategies/Sequence.cpp

${OBJECTDIR}/_ext/da13872c/Sort.o: ../src/mod/lib/DG/Strategies/Sort.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Sort.o ../src/mod/lib/DG/Strategies/Sort.cpp

${OBJECTDIR}/_ext/da13872c/Strategy.o: ../src/mod/lib/DG/Strategies/Strategy.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Strategy.o ../src/mod/lib/DG/Strategies/Strategy.cpp

${OBJECTDIR}/_ext/da13872c/Take.o: ../src/mod/lib/DG/Strategies/Take.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Take.o ../src/mod/lib/DG/Strategies/Take.cpp

${OBJECTDIR}/_ext/34a0cedd/DFSEncoding.o: ../src/mod/lib/Graph/DFSEncoding.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/34a0cedd
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/34a0cedd/DFSEncoding.o ../src/mod/lib/Graph/DFSEncoding.cpp

${OBJECTDIR}/_ext/34a0cedd/LabelledGraph.o: ../src/mod/lib/Graph/LabelledGraph.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/34a0cedd
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/34a0cedd/LabelledGraph.o ../src/mod/lib/Graph/LabelledGraph.cpp

${OBJECTDIR}/_ext/34a0cedd/Merge.o: ../src/mod/lib/Graph/Merge.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/34a0cedd
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/34a0cedd/Merge.o ../src/mod/lib/Graph/Merge.cpp

${OBJECTDIR}/_ext/d46a6fc5/Depiction.o: ../src/mod/lib/Graph/Properties/Depiction.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d46a6fc5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d46a6fc5/Depiction.o ../src/mod/lib/Graph/Properties/Depiction.cpp

${OBJECTDIR}/_ext/d46a6fc5/Molecule.o: ../src/mod/lib/Graph/Properties/Molecule.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d46a6fc5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d46a6fc5/Molecule.o ../src/mod/lib/Graph/Properties/Molecule.cpp

${OBJECTDIR}/_ext/34a0cedd/Single.o: ../src/mod/lib/Graph/Single.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/34a0cedd
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/34a0cedd/Single.o ../src/mod/lib/Graph/Single.cpp

${OBJECTDIR}/_ext/d81a81d7/DGRead.o: ../src/mod/lib/IO/DGRead.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/DGRead.o ../src/mod/lib/IO/DGRead.cpp

${OBJECTDIR}/_ext/d81a81d7/DGWrite.o: ../src/mod/lib/IO/DGWrite.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/DGWrite.o ../src/mod/lib/IO/DGWrite.cpp

${OBJECTDIR}/_ext/d81a81d7/Derivation.o: ../src/mod/lib/IO/Derivation.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/Derivation.o ../src/mod/lib/IO/Derivation.cpp

${OBJECTDIR}/_ext/d81a81d7/FileHandle.o: ../src/mod/lib/IO/FileHandle.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/FileHandle.o ../src/mod/lib/IO/FileHandle.cpp

${OBJECTDIR}/_ext/d81a81d7/GMLUtil.o: ../src/mod/lib/IO/GMLUtil.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/GMLUtil.o ../src/mod/lib/IO/GMLUtil.cpp

${OBJECTDIR}/_ext/d81a81d7/GraphRead.o: ../src/mod/lib/IO/GraphRead.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/GraphRead.o ../src/mod/lib/IO/GraphRead.cpp

${OBJECTDIR}/_ext/d81a81d7/GraphWrite.o: ../src/mod/lib/IO/GraphWrite.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/GraphWrite.o ../src/mod/lib/IO/GraphWrite.cpp

${OBJECTDIR}/_ext/d81a81d7/IO.o: ../src/mod/lib/IO/IO.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/IO.o ../src/mod/lib/IO/IO.cpp

${OBJECTDIR}/_ext/d81a81d7/ParserCommon.o: ../src/mod/lib/IO/ParserCommon.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/ParserCommon.o ../src/mod/lib/IO/ParserCommon.cpp

${OBJECTDIR}/_ext/d81a81d7/RC.o: ../src/mod/lib/IO/RC.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/RC.o ../src/mod/lib/IO/RC.cpp

${OBJECTDIR}/_ext/d81a81d7/RuleRead.o: ../src/mod/lib/IO/RuleRead.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/RuleRead.o ../src/mod/lib/IO/RuleRead.cpp

${OBJECTDIR}/_ext/d81a81d7/RuleWrite.o: ../src/mod/lib/IO/RuleWrite.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/RuleWrite.o ../src/mod/lib/IO/RuleWrite.cpp

${OBJECTDIR}/_ext/d81a82e2/ComposeRuleReal.o: ../src/mod/lib/RC/ComposeRuleReal.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a82e2
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a82e2/ComposeRuleReal.o ../src/mod/lib/RC/ComposeRuleReal.cpp

${OBJECTDIR}/_ext/d81a82e2/Evaluator.o: ../src/mod/lib/RC/Evaluator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a82e2
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a82e2/Evaluator.o ../src/mod/lib/RC/Evaluator.cpp

${OBJECTDIR}/_ext/384aeefe/Random.o: ../src/mod/lib/Random.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/384aeefe
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/384aeefe/Random.o ../src/mod/lib/Random.cpp

${OBJECTDIR}/_ext/353d5686/LabelledRule.o: ../src/mod/lib/Rules/LabelledRule.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/353d5686
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/353d5686/LabelledRule.o ../src/mod/lib/Rules/LabelledRule.cpp

${OBJECTDIR}/_ext/5cc9b47c/Depiction.o: ../src/mod/lib/Rules/Properties/Depiction.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5cc9b47c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5cc9b47c/Depiction.o ../src/mod/lib/Rules/Properties/Depiction.cpp

${OBJECTDIR}/_ext/5cc9b47c/Molecule.o: ../src/mod/lib/Rules/Properties/Molecule.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5cc9b47c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5cc9b47c/Molecule.o ../src/mod/lib/Rules/Properties/Molecule.cpp

${OBJECTDIR}/_ext/353d5686/Real.o: ../src/mod/lib/Rules/Real.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/353d5686
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../src/graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/353d5686/Real.o ../src/mod/lib/Rules/Real.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} dist/lib/libmod.${CND_DLIB_EXT}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
