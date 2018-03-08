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
	${OBJECTDIR}/_ext/4d98cc98/vf2.o \
	${OBJECTDIR}/_ext/2754b308/Chem.o \
	${OBJECTDIR}/_ext/2754b308/Config.o \
	${OBJECTDIR}/_ext/2754b308/Derivation.o \
	${OBJECTDIR}/_ext/2754b308/Error.o \
	${OBJECTDIR}/_ext/2754b308/Function.o \
	${OBJECTDIR}/_ext/2754b308/Misc.o \
	${OBJECTDIR}/_ext/2754b308/Term.o \
	${OBJECTDIR}/_ext/f98eccea/DG.o \
	${OBJECTDIR}/_ext/f98eccea/GraphInterface.o \
	${OBJECTDIR}/_ext/f98eccea/Printer.o \
	${OBJECTDIR}/_ext/f98eccea/Strategies.o \
	${OBJECTDIR}/_ext/5108d387/Automorphism.o \
	${OBJECTDIR}/_ext/5108d387/Graph.o \
	${OBJECTDIR}/_ext/5108d387/GraphInterface.o \
	${OBJECTDIR}/_ext/5108d387/Printer.o \
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
	${OBJECTDIR}/_ext/34a0cedd/Canonicalisation.o \
	${OBJECTDIR}/_ext/34a0cedd/DFSEncoding.o \
	${OBJECTDIR}/_ext/34a0cedd/LabelledGraph.o \
	${OBJECTDIR}/_ext/d46a6fc5/Depiction.o \
	${OBJECTDIR}/_ext/d46a6fc5/Molecule.o \
	${OBJECTDIR}/_ext/d46a6fc5/Term.o \
	${OBJECTDIR}/_ext/34a0cedd/Single.o \
	${OBJECTDIR}/_ext/d81a81d7/DGRead.o \
	${OBJECTDIR}/_ext/d81a81d7/DGWrite.o \
	${OBJECTDIR}/_ext/d81a81d7/Derivation.o \
	${OBJECTDIR}/_ext/d81a81d7/FileHandle.o \
	${OBJECTDIR}/_ext/d81a81d7/GMLUtil.o \
	${OBJECTDIR}/_ext/d81a81d7/GraphRead.o \
	${OBJECTDIR}/_ext/d81a81d7/GraphWrite.o \
	${OBJECTDIR}/_ext/d81a81d7/IO.o \
	${OBJECTDIR}/_ext/d81a81d7/RC.o \
	${OBJECTDIR}/_ext/d81a81d7/RuleRead.o \
	${OBJECTDIR}/_ext/d81a81d7/RuleWrite.o \
	${OBJECTDIR}/_ext/d81a81d7/StereoRead.o \
	${OBJECTDIR}/_ext/d81a81d7/StereoWrite.o \
	${OBJECTDIR}/_ext/d81a81d7/Term.o \
	${OBJECTDIR}/_ext/d81a82e2/ComposeRuleReal.o \
	${OBJECTDIR}/_ext/d81a82e2/Evaluator.o \
	${OBJECTDIR}/_ext/384aeefe/Random.o \
	${OBJECTDIR}/_ext/353d5686/LabelledRule.o \
	${OBJECTDIR}/_ext/5cc9b47c/Depiction.o \
	${OBJECTDIR}/_ext/5cc9b47c/Molecule.o \
	${OBJECTDIR}/_ext/5cc9b47c/String.o \
	${OBJECTDIR}/_ext/5cc9b47c/Term.o \
	${OBJECTDIR}/_ext/353d5686/Real.o \
	${OBJECTDIR}/_ext/a9543190/Any.o \
	${OBJECTDIR}/_ext/a9543190/Configuration.o \
	${OBJECTDIR}/_ext/a9543190/Linear.o \
	${OBJECTDIR}/_ext/a9543190/Tetrahedral.o \
	${OBJECTDIR}/_ext/a9543190/TrigonalPlanar.o \
	${OBJECTDIR}/_ext/74113c49/EdgeCategory.o \
	${OBJECTDIR}/_ext/74113c49/EmbeddingEdge.o \
	${OBJECTDIR}/_ext/74113c49/GeometryGraph.o \
	${OBJECTDIR}/_ext/74113c49/Inference.o \
	${OBJECTDIR}/_ext/384aeefe/StringStore.o \
	${OBJECTDIR}/_ext/3b86c8fd/RawTerm.o \
	${OBJECTDIR}/_ext/3b86c8fd/WAM.o \
	${OBJECTDIR}/_ext/d115d9a3/Composer.o \
	${OBJECTDIR}/_ext/d115d9a3/CompositionExpr.o \
	${OBJECTDIR}/_ext/d115d9a3/GraphInterface.o \
	${OBJECTDIR}/_ext/d115d9a3/Rule.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-O3 -fno-strict-aliasing -fPIC -pthread -Wall -Wextra -Wno-unused-local-typedefs -Wno-unused-parameter -Wno-comment -Wno-maybe-uninitialized -Wno-deprecated-declarations -Wno-sign-compare -Wno-ignored-attributes -ftemplate-backtrace-limit=0
CXXFLAGS=-O3 -fno-strict-aliasing -fPIC -pthread -Wall -Wextra -Wno-unused-local-typedefs -Wno-unused-parameter -Wno-comment -Wno-maybe-uninitialized -Wno-deprecated-declarations -Wno-sign-compare -Wno-ignored-attributes -ftemplate-backtrace-limit=0

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L${HOME}/programs/lib -Wl,-rpath,'${HOME}/programs/lib' -Wl,-Bdynamic -lboost_regex -lboost_system -lopenbabel -pthread -Wl,-Bdynamic

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk dist/lib/libmod.${CND_DLIB_EXT}

dist/lib/libmod.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p dist/lib
	${LINK.cc} -o dist/lib/libmod.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -rdynamic -Wl,--no-undefined -shared -fPIC

${OBJECTDIR}/_ext/27549c4c/parser.o: ../src/gml/parser.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/27549c4c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/27549c4c/parser.o ../src/gml/parser.cpp

${OBJECTDIR}/_ext/9729d8dc/IO.o: ../src/jla_boost/graph/dpo/IO.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/9729d8dc
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9729d8dc/IO.o ../src/jla_boost/graph/dpo/IO.cpp

${OBJECTDIR}/_ext/4d98cc98/vf2.o: ../src/jla_boost/test/vf2.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/4d98cc98
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/4d98cc98/vf2.o ../src/jla_boost/test/vf2.cpp

${OBJECTDIR}/_ext/2754b308/Chem.o: ../src/mod/Chem.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/Chem.o ../src/mod/Chem.cpp

${OBJECTDIR}/_ext/2754b308/Config.o: ../src/mod/Config.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/Config.o ../src/mod/Config.cpp

${OBJECTDIR}/_ext/2754b308/Derivation.o: ../src/mod/Derivation.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/Derivation.o ../src/mod/Derivation.cpp

${OBJECTDIR}/_ext/2754b308/Error.o: ../src/mod/Error.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/Error.o ../src/mod/Error.cpp

${OBJECTDIR}/_ext/2754b308/Function.o: ../src/mod/Function.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/Function.o ../src/mod/Function.cpp

${OBJECTDIR}/_ext/2754b308/Misc.o: ../src/mod/Misc.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/Misc.o ../src/mod/Misc.cpp

${OBJECTDIR}/_ext/2754b308/Term.o: ../src/mod/Term.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2754b308
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2754b308/Term.o ../src/mod/Term.cpp

${OBJECTDIR}/_ext/f98eccea/DG.o: ../src/mod/dg/DG.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eccea
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eccea/DG.o ../src/mod/dg/DG.cpp

${OBJECTDIR}/_ext/f98eccea/GraphInterface.o: ../src/mod/dg/GraphInterface.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eccea
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eccea/GraphInterface.o ../src/mod/dg/GraphInterface.cpp

${OBJECTDIR}/_ext/f98eccea/Printer.o: ../src/mod/dg/Printer.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eccea
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eccea/Printer.o ../src/mod/dg/Printer.cpp

${OBJECTDIR}/_ext/f98eccea/Strategies.o: ../src/mod/dg/Strategies.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f98eccea
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f98eccea/Strategies.o ../src/mod/dg/Strategies.cpp

${OBJECTDIR}/_ext/5108d387/Automorphism.o: ../src/mod/graph/Automorphism.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5108d387
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5108d387/Automorphism.o ../src/mod/graph/Automorphism.cpp

${OBJECTDIR}/_ext/5108d387/Graph.o: ../src/mod/graph/Graph.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5108d387
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5108d387/Graph.o ../src/mod/graph/Graph.cpp

${OBJECTDIR}/_ext/5108d387/GraphInterface.o: ../src/mod/graph/GraphInterface.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5108d387
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5108d387/GraphInterface.o ../src/mod/graph/GraphInterface.cpp

${OBJECTDIR}/_ext/5108d387/Printer.o: ../src/mod/graph/Printer.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5108d387
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5108d387/Printer.o ../src/mod/graph/Printer.cpp

${OBJECTDIR}/_ext/3b7f185e/MoleculeUtil.o: ../src/mod/lib/Chem/MoleculeUtil.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/3b7f185e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3b7f185e/MoleculeUtil.o ../src/mod/lib/Chem/MoleculeUtil.cpp

${OBJECTDIR}/_ext/3b7f185e/OBabel.o: ../src/mod/lib/Chem/OBabel.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/3b7f185e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3b7f185e/OBabel.o ../src/mod/lib/Chem/OBabel.cpp

${OBJECTDIR}/_ext/3b7f185e/Smiles.o: ../src/mod/lib/Chem/Smiles.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/3b7f185e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3b7f185e/Smiles.o ../src/mod/lib/Chem/Smiles.cpp

${OBJECTDIR}/_ext/3b7f185e/SmilesRead.o: ../src/mod/lib/Chem/SmilesRead.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/3b7f185e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3b7f185e/SmilesRead.o ../src/mod/lib/Chem/SmilesRead.cpp

${OBJECTDIR}/_ext/3b7f185e/SmilesWrite.o: ../src/mod/lib/Chem/SmilesWrite.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/3b7f185e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3b7f185e/SmilesWrite.o ../src/mod/lib/Chem/SmilesWrite.cpp

${OBJECTDIR}/_ext/d81a8134/Dump.o: ../src/mod/lib/DG/Dump.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a8134
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a8134/Dump.o ../src/mod/lib/DG/Dump.cpp

${OBJECTDIR}/_ext/d81a8134/Hyper.o: ../src/mod/lib/DG/Hyper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a8134
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a8134/Hyper.o ../src/mod/lib/DG/Hyper.cpp

${OBJECTDIR}/_ext/d81a8134/NonHyper.o: ../src/mod/lib/DG/NonHyper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a8134
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a8134/NonHyper.o ../src/mod/lib/DG/NonHyper.cpp

${OBJECTDIR}/_ext/d81a8134/NonHyperDerivations.o: ../src/mod/lib/DG/NonHyperDerivations.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a8134
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a8134/NonHyperDerivations.o ../src/mod/lib/DG/NonHyperDerivations.cpp

${OBJECTDIR}/_ext/d81a8134/NonHyperRuleComp.o: ../src/mod/lib/DG/NonHyperRuleComp.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a8134
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a8134/NonHyperRuleComp.o ../src/mod/lib/DG/NonHyperRuleComp.cpp

${OBJECTDIR}/_ext/da13872c/Add.o: ../src/mod/lib/DG/Strategies/Add.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Add.o ../src/mod/lib/DG/Strategies/Add.cpp

${OBJECTDIR}/_ext/da13872c/DerivationPredicates.o: ../src/mod/lib/DG/Strategies/DerivationPredicates.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/DerivationPredicates.o ../src/mod/lib/DG/Strategies/DerivationPredicates.cpp

${OBJECTDIR}/_ext/da13872c/Execute.o: ../src/mod/lib/DG/Strategies/Execute.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Execute.o ../src/mod/lib/DG/Strategies/Execute.cpp

${OBJECTDIR}/_ext/da13872c/Filter.o: ../src/mod/lib/DG/Strategies/Filter.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Filter.o ../src/mod/lib/DG/Strategies/Filter.cpp

${OBJECTDIR}/_ext/da13872c/GraphState.o: ../src/mod/lib/DG/Strategies/GraphState.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/GraphState.o ../src/mod/lib/DG/Strategies/GraphState.cpp

${OBJECTDIR}/_ext/da13872c/Parallel.o: ../src/mod/lib/DG/Strategies/Parallel.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Parallel.o ../src/mod/lib/DG/Strategies/Parallel.cpp

${OBJECTDIR}/_ext/da13872c/Repeat.o: ../src/mod/lib/DG/Strategies/Repeat.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Repeat.o ../src/mod/lib/DG/Strategies/Repeat.cpp

${OBJECTDIR}/_ext/da13872c/Revive.o: ../src/mod/lib/DG/Strategies/Revive.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Revive.o ../src/mod/lib/DG/Strategies/Revive.cpp

${OBJECTDIR}/_ext/da13872c/Rule.o: ../src/mod/lib/DG/Strategies/Rule.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Rule.o ../src/mod/lib/DG/Strategies/Rule.cpp

${OBJECTDIR}/_ext/da13872c/Sequence.o: ../src/mod/lib/DG/Strategies/Sequence.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Sequence.o ../src/mod/lib/DG/Strategies/Sequence.cpp

${OBJECTDIR}/_ext/da13872c/Sort.o: ../src/mod/lib/DG/Strategies/Sort.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Sort.o ../src/mod/lib/DG/Strategies/Sort.cpp

${OBJECTDIR}/_ext/da13872c/Strategy.o: ../src/mod/lib/DG/Strategies/Strategy.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Strategy.o ../src/mod/lib/DG/Strategies/Strategy.cpp

${OBJECTDIR}/_ext/da13872c/Take.o: ../src/mod/lib/DG/Strategies/Take.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/da13872c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/da13872c/Take.o ../src/mod/lib/DG/Strategies/Take.cpp

${OBJECTDIR}/_ext/34a0cedd/Canonicalisation.o: ../src/mod/lib/Graph/Canonicalisation.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/34a0cedd
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/34a0cedd/Canonicalisation.o ../src/mod/lib/Graph/Canonicalisation.cpp

${OBJECTDIR}/_ext/34a0cedd/DFSEncoding.o: ../src/mod/lib/Graph/DFSEncoding.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/34a0cedd
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/34a0cedd/DFSEncoding.o ../src/mod/lib/Graph/DFSEncoding.cpp

${OBJECTDIR}/_ext/34a0cedd/LabelledGraph.o: ../src/mod/lib/Graph/LabelledGraph.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/34a0cedd
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/34a0cedd/LabelledGraph.o ../src/mod/lib/Graph/LabelledGraph.cpp

${OBJECTDIR}/_ext/d46a6fc5/Depiction.o: ../src/mod/lib/Graph/Properties/Depiction.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d46a6fc5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d46a6fc5/Depiction.o ../src/mod/lib/Graph/Properties/Depiction.cpp

${OBJECTDIR}/_ext/d46a6fc5/Molecule.o: ../src/mod/lib/Graph/Properties/Molecule.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d46a6fc5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d46a6fc5/Molecule.o ../src/mod/lib/Graph/Properties/Molecule.cpp

${OBJECTDIR}/_ext/d46a6fc5/Term.o: ../src/mod/lib/Graph/Properties/Term.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d46a6fc5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d46a6fc5/Term.o ../src/mod/lib/Graph/Properties/Term.cpp

${OBJECTDIR}/_ext/34a0cedd/Single.o: ../src/mod/lib/Graph/Single.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/34a0cedd
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/34a0cedd/Single.o ../src/mod/lib/Graph/Single.cpp

${OBJECTDIR}/_ext/d81a81d7/DGRead.o: ../src/mod/lib/IO/DGRead.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/DGRead.o ../src/mod/lib/IO/DGRead.cpp

${OBJECTDIR}/_ext/d81a81d7/DGWrite.o: ../src/mod/lib/IO/DGWrite.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/DGWrite.o ../src/mod/lib/IO/DGWrite.cpp

${OBJECTDIR}/_ext/d81a81d7/Derivation.o: ../src/mod/lib/IO/Derivation.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/Derivation.o ../src/mod/lib/IO/Derivation.cpp

${OBJECTDIR}/_ext/d81a81d7/FileHandle.o: ../src/mod/lib/IO/FileHandle.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/FileHandle.o ../src/mod/lib/IO/FileHandle.cpp

${OBJECTDIR}/_ext/d81a81d7/GMLUtil.o: ../src/mod/lib/IO/GMLUtil.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/GMLUtil.o ../src/mod/lib/IO/GMLUtil.cpp

${OBJECTDIR}/_ext/d81a81d7/GraphRead.o: ../src/mod/lib/IO/GraphRead.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/GraphRead.o ../src/mod/lib/IO/GraphRead.cpp

${OBJECTDIR}/_ext/d81a81d7/GraphWrite.o: ../src/mod/lib/IO/GraphWrite.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/GraphWrite.o ../src/mod/lib/IO/GraphWrite.cpp

${OBJECTDIR}/_ext/d81a81d7/IO.o: ../src/mod/lib/IO/IO.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/IO.o ../src/mod/lib/IO/IO.cpp

${OBJECTDIR}/_ext/d81a81d7/RC.o: ../src/mod/lib/IO/RC.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/RC.o ../src/mod/lib/IO/RC.cpp

${OBJECTDIR}/_ext/d81a81d7/RuleRead.o: ../src/mod/lib/IO/RuleRead.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/RuleRead.o ../src/mod/lib/IO/RuleRead.cpp

${OBJECTDIR}/_ext/d81a81d7/RuleWrite.o: ../src/mod/lib/IO/RuleWrite.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/RuleWrite.o ../src/mod/lib/IO/RuleWrite.cpp

${OBJECTDIR}/_ext/d81a81d7/StereoRead.o: ../src/mod/lib/IO/StereoRead.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/StereoRead.o ../src/mod/lib/IO/StereoRead.cpp

${OBJECTDIR}/_ext/d81a81d7/StereoWrite.o: ../src/mod/lib/IO/StereoWrite.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/StereoWrite.o ../src/mod/lib/IO/StereoWrite.cpp

${OBJECTDIR}/_ext/d81a81d7/Term.o: ../src/mod/lib/IO/Term.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a81d7
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a81d7/Term.o ../src/mod/lib/IO/Term.cpp

${OBJECTDIR}/_ext/d81a82e2/ComposeRuleReal.o: ../src/mod/lib/RC/ComposeRuleReal.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a82e2
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a82e2/ComposeRuleReal.o ../src/mod/lib/RC/ComposeRuleReal.cpp

${OBJECTDIR}/_ext/d81a82e2/Evaluator.o: ../src/mod/lib/RC/Evaluator.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d81a82e2
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d81a82e2/Evaluator.o ../src/mod/lib/RC/Evaluator.cpp

${OBJECTDIR}/_ext/384aeefe/Random.o: ../src/mod/lib/Random.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/384aeefe
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/384aeefe/Random.o ../src/mod/lib/Random.cpp

${OBJECTDIR}/_ext/353d5686/LabelledRule.o: ../src/mod/lib/Rules/LabelledRule.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/353d5686
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/353d5686/LabelledRule.o ../src/mod/lib/Rules/LabelledRule.cpp

${OBJECTDIR}/_ext/5cc9b47c/Depiction.o: ../src/mod/lib/Rules/Properties/Depiction.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5cc9b47c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5cc9b47c/Depiction.o ../src/mod/lib/Rules/Properties/Depiction.cpp

${OBJECTDIR}/_ext/5cc9b47c/Molecule.o: ../src/mod/lib/Rules/Properties/Molecule.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5cc9b47c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5cc9b47c/Molecule.o ../src/mod/lib/Rules/Properties/Molecule.cpp

${OBJECTDIR}/_ext/5cc9b47c/String.o: ../src/mod/lib/Rules/Properties/String.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5cc9b47c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5cc9b47c/String.o ../src/mod/lib/Rules/Properties/String.cpp

${OBJECTDIR}/_ext/5cc9b47c/Term.o: ../src/mod/lib/Rules/Properties/Term.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5cc9b47c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5cc9b47c/Term.o ../src/mod/lib/Rules/Properties/Term.cpp

${OBJECTDIR}/_ext/353d5686/Real.o: ../src/mod/lib/Rules/Real.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/353d5686
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/353d5686/Real.o ../src/mod/lib/Rules/Real.cpp

${OBJECTDIR}/_ext/a9543190/Any.o: ../src/mod/lib/Stereo/Configuration/Any.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/a9543190
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a9543190/Any.o ../src/mod/lib/Stereo/Configuration/Any.cpp

${OBJECTDIR}/_ext/a9543190/Configuration.o: ../src/mod/lib/Stereo/Configuration/Configuration.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/a9543190
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a9543190/Configuration.o ../src/mod/lib/Stereo/Configuration/Configuration.cpp

${OBJECTDIR}/_ext/a9543190/Linear.o: ../src/mod/lib/Stereo/Configuration/Linear.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/a9543190
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a9543190/Linear.o ../src/mod/lib/Stereo/Configuration/Linear.cpp

${OBJECTDIR}/_ext/a9543190/Tetrahedral.o: ../src/mod/lib/Stereo/Configuration/Tetrahedral.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/a9543190
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a9543190/Tetrahedral.o ../src/mod/lib/Stereo/Configuration/Tetrahedral.cpp

${OBJECTDIR}/_ext/a9543190/TrigonalPlanar.o: ../src/mod/lib/Stereo/Configuration/TrigonalPlanar.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/a9543190
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a9543190/TrigonalPlanar.o ../src/mod/lib/Stereo/Configuration/TrigonalPlanar.cpp

${OBJECTDIR}/_ext/74113c49/EdgeCategory.o: ../src/mod/lib/Stereo/EdgeCategory.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/74113c49
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/74113c49/EdgeCategory.o ../src/mod/lib/Stereo/EdgeCategory.cpp

${OBJECTDIR}/_ext/74113c49/EmbeddingEdge.o: ../src/mod/lib/Stereo/EmbeddingEdge.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/74113c49
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/74113c49/EmbeddingEdge.o ../src/mod/lib/Stereo/EmbeddingEdge.cpp

${OBJECTDIR}/_ext/74113c49/GeometryGraph.o: ../src/mod/lib/Stereo/GeometryGraph.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/74113c49
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/74113c49/GeometryGraph.o ../src/mod/lib/Stereo/GeometryGraph.cpp

${OBJECTDIR}/_ext/74113c49/Inference.o: ../src/mod/lib/Stereo/Inference.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/74113c49
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/74113c49/Inference.o ../src/mod/lib/Stereo/Inference.cpp

${OBJECTDIR}/_ext/384aeefe/StringStore.o: ../src/mod/lib/StringStore.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/384aeefe
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/384aeefe/StringStore.o ../src/mod/lib/StringStore.cpp

${OBJECTDIR}/_ext/3b86c8fd/RawTerm.o: ../src/mod/lib/Term/RawTerm.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/3b86c8fd
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3b86c8fd/RawTerm.o ../src/mod/lib/Term/RawTerm.cpp

${OBJECTDIR}/_ext/3b86c8fd/WAM.o: ../src/mod/lib/Term/WAM.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/3b86c8fd
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3b86c8fd/WAM.o ../src/mod/lib/Term/WAM.cpp

${OBJECTDIR}/_ext/d115d9a3/Composer.o: ../src/mod/rule/Composer.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d115d9a3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d115d9a3/Composer.o ../src/mod/rule/Composer.cpp

${OBJECTDIR}/_ext/d115d9a3/CompositionExpr.o: ../src/mod/rule/CompositionExpr.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d115d9a3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d115d9a3/CompositionExpr.o ../src/mod/rule/CompositionExpr.cpp

${OBJECTDIR}/_ext/d115d9a3/GraphInterface.o: ../src/mod/rule/GraphInterface.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d115d9a3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d115d9a3/GraphInterface.o ../src/mod/rule/GraphInterface.cpp

${OBJECTDIR}/_ext/d115d9a3/Rule.o: ../src/mod/rule/Rule.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d115d9a3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DBOOST_RESULT_OF_USE_DECLTYPE -DBOOST_SPIRIT_USE_PHOENIX_V3 -DBOOST_SYSTEM_NO_DEPRECATED -I${CND_BASEDIR}/../../perm_group/include -I${CND_BASEDIR}/../../graph_canon/include -I${CND_BASEDIR}/../src/mod/lib/boost/include -I${CND_BASEDIR}/../src -I${CND_BASEDIR}/srcExtra -I/usr/include/openbabel-2.0 -I${HOME}/programs/include -std=c++14 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d115d9a3/Rule.o ../src/mod/rule/Rule.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
