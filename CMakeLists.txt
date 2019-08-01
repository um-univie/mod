cmake_minimum_required(VERSION 3.10)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

###########################################################################
# Package Properties
###########################################################################
file(READ "VERSION" PROJECT_VERSION)
string(STRIP "${PROJECT_VERSION}" PROJECT_VERSION) # remove the newline

project(mod VERSION ${PROJECT_VERSION})
set(mod_VERSION ${PROJECT_VERSION} CACHE INTERNAL "" FORCE)
set(PNAME_FILE mod) # how the project name should be written in file names
set(CPACK_PACKAGE_CONTACT "Jakob Lykke Andersen <jlandersen@imada.sdu.dk>")


set(CMAKE_CXX_FLAGS_OPTDEBUG "-g -O3")
set(CMAKE_C_FLAGS_OPTDEBUG "-g -O3")
set(CMAKE_EXE_LINKER_FLAGS_OPTDEBUG "")
set(CMAKE_SHARED_LINKER_FLAGS_OPTDEBUG "")
mark_as_advanced(CMAKE_CXX_FLAGS_OPTDEBUG CMAKE_C_FLAGS_OPTDEBUG CMAKE_EXE_LINKER_FLAGS_OPTDEBUG CMAKE_SHARED_LINKER_FLAGS_OPTDEBUG)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
            "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel OptDebug."
            FORCE)
endif()


###########################################################################
# Options
###########################################################################
option(BUILD_DOC "Enable documentation building." ON)
option(BUILD_POST_MOD "Enable building of the post processor." ON)
option(BUILD_PY_MOD "Enable building of the Python bindings." ON)
option(BUILD_EXAMPLES "Enable example building." OFF)
option(BUILD_TESTING "Enable test building." OFF)
option(BUILD_TESTING_SANITIZERS "Compile tests with sanitizers." ON)
option(BUILD_COVERAGE "Enable code coverage." OFF)

option(ENABLE_SYMBOL_HIDING "Hide internal symbols in the library." ON)
option(ENABLE_DEP_SYMBOL_HIDING "Hide symbols provided by library dependencies." ON)
option(ENABLE_IPO "Using link-time optimization." ON)
include(CheckIPOSupported)

option(USE_NESTED_GRAPH_CANON "Use the GraphCanon version in external/graph_canon." ON)
option(WITH_OPENBABEL "Whether features depending on Open Babel are enabled or not." ON)


get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
    set(BUILD_DOC 0)
    set(BUILD_EXAMPLES 0)
    set(BUILD_TESTING 0)
    set(BUILD_COVERAGE 0)
endif()
if(NOT BUILD_TESTING)
    set(BUILD_EXAMPLES OFF)
endif()

enable_testing()  # should be included here to add the targets in the top-level folder

###########################################################################
# Individual checks
###########################################################################
set(CMAKE_CXX_STANDARD 14)

set(libmod_config_dependencies "")
set(libmod_config_find_files "FindPackageHandleStandardArgs.cmake;FindPackageMessage.cmake;FindPkgConfig.cmake;SelectLibraryConfigurations.cmake")


# Boost
# -------------------------------------------------------------------------
set(v 1.64.0)
if(BUILD_PY_MOD)
    foreach(PY 3 34 35 36 37 38 39)
        set(lib "python${PY}")
        find_package(Boost ${v} QUIET COMPONENTS ${lib})
        if(Boost_FOUND)
            find_package(Boost ${v} COMPONENTS ${lib})
            set(PYTHON_TARGET ${lib})
            break()
        endif()
    endforeach()
    if(NOT Boost_FOUND)
        find_package(Boost ${v} REQUIRED COMPONENTS python3)
        message(FATAL_ERROR "Could not find Boost.Python for Python 3. Tried 'python' wih suffixes 3, 34, 35, 36, 37, 38, and 39.")
    endif()
endif()
find_package(Boost ${v} REQUIRED COMPONENTS iostreams regex system)
string(APPEND libmod_config_dependencies "set(BOOST_FOUND 0)\n") # TODO: remove with CMake 3.15 is required https://gitlab.kitware.com/cmake/cmake/issues/18590
string(APPEND libmod_config_dependencies "find_dependency(Boost ${v} COMPONENTS iostreams regex system)\n")
list(APPEND libmod_config_find_files "FindBoost.cmake")


# GraphCanon
# -------------------------------------------------------------------------
set(v 0.4)
if(USE_NESTED_GRAPH_CANON)
    message(STATUS "GraphCanon: using external/graph_canon")
    add_subdirectory(external/graph_canon)
    if(GraphCanon_VERSION VERSION_LESS v)
        message(FATAL_ERROR "Nested GraphCanon version not new enough: is ${GraphCanon_VERSION}, needs ${v}."
                " Try again after 'git submodule update --init --recursive; ./bootstrap.sh'."
                " Otherwise please report as bug.")
    endif()
else()
    find_package(GraphCanon ${v} REQUIRED)
    string(APPEND libmod_config_dependencies "find_dependency(GraphCanon ${v})\n")
    message(STATUS "GraphCanon version: ${GraphCanon_VERSION}")
endif()


# Graphviz
# -------------------------------------------------------------------------
if(BUILD_POST_MOD)
    find_program(DOT dot)
    if(NOT DOT)
        message(FATAL_ERROR "Could not find the dot executable (from Graphviz) required for PostMØD.\n"
                "Disable with -DBUILD_POST_MOD=no, or add the folder of the executable to PATH.")# or give with -DCMAKE_PROGRAM_PATH=paths (currently '${CMAKE_PROGRAM_PATH}').")
    endif()
    message(STATUS "Found dot: ${DOT}")

    execute_process(
            COMMAND ${DOT} -P
            COMMAND grep "cairo_device_svg -> output_svg"
            RESULT_VARIABLE res
            OUTPUT_QUIET ERROR_QUIET)
    if(${res} EQUAL 0)
        message(STATUS "Found dot to support cairo svg output.")
    else()
        message(FATAL_ERROR "The dot command does not seem to support cairo svg output, which is needed for PostMØD."
                " Disable with -DBUILD_POST_MOD=no\n")
    endif()
    execute_process(
            COMMAND ${DOT} -P
            COMMAND grep "cairo_device_pdf -> output_pdf"
            RESULT_VARIABLE res
            OUTPUT_QUIET ERROR_QUIET)
    if(${res} EQUAL 0)
        message(STATUS "Found dot to support cairo pdf output.")
    else()
        message(FATAL_ERROR "The dot command does not seem to support cairo pdf output, which is needed for PostMØD."
                " Disable with -DBUILD_POST_MOD=no\n")
    endif()
    execute_process(
            COMMAND ${DOT} -P
            COMMAND grep "rsvg_loadimage_svg -> render_cairo"
            RESULT_VARIABLE res
            OUTPUT_QUIET ERROR_QUIET)
    if(${res} EQUAL 0)
        message(STATUS "Found dot to support svg to cairo conversion.")
    else()
        message(FATAL_ERROR "The dot command does not seem to support svg to cairo conversion, which is needed for PostMØD."
                " Disable with -DBUILD_POST_MOD=no\n")
    endif()
endif()


# Open Babel
# -------------------------------------------------------------------------
if(WITH_OPENBABEL)
    find_package(OpenBabel2 2.3.2 REQUIRED)
    string(APPEND libmod_config_dependencies "set(OpenBabel2_FOUND 0)\n") # TODO: remove with CMake 3.15 is required https://gitlab.kitware.com/cmake/cmake/issues/18590
    string(APPEND libmod_config_dependencies "find_dependency(OpenBabel2 2.3.2)\n")
    list(APPEND libmod_config_find_files "FindOpenBabel2.cmake")
    set(MOD_HAVE_OPENBABEL TRUE)
endif()


if(BUILD_POST_MOD)
    # pdf2svg
    # ---------------------------------------------------------------------
    find_program(PDF2SVG pdf2svg)
    if(NOT PDF2SVG)
        message(FATAL_ERROR "Could not find pdf2svg program, required for post_mod.")
    else()
        message(STATUS "Found pdf2svg: ${PDF2SVG}")
    endif()

    # pdflatex
    # ---------------------------------------------------------------------
    find_package(LATEX REQUIRED COMPONENTS PDFLATEX)
endif()


    string(APPEND libmod_config_dependencies "set(PNAPI_FOUND 0)\n") # TODO: remove with CMake 3.15 is required https://gitlab.kitware.com/cmake/cmake/issues/18590
# Python
# -------------------------------------------------------------------------
if(BUILD_PY_MOD)
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
endif()


# Sphinx
# -------------------------------------------------------------------------
if(BUILD_DOC)
    find_program(SPHINX NAMES sphinx-build sphinx-build3 sphinx-build-3 sphinx-build2)
    if(SPHINX)
        message(STATUS "Sphinx: ${SPHINX}")
    else()
        message(FATAL_ERROR "Sphinx not found, required for building the documentation. Use --DBUILD_DOC=off to disable.")
    endif()
endif()


# Threads
# -------------------------------------------------------------------------
find_package(Threads REQUIRED)
string(APPEND libmod_config_dependencies "find_dependency(Threads)\n")


###########################################################################
# Targets and Artefacts
###########################################################################
include(CMakeFiles.txt)
include(Coverage)
include(GNUInstallDirs)

if(NOT hasParent)
    set(CMAKE_NO_BUILTIN_CHRPATH TRUE)                 #
    # https://gitlab.kitware.com/cmake/community/wikis/doc/cmake/RPATH-handling
    set(CMAKE_SKIP_BUILD_RPATH FALSE)                  # use RPATH in the build tree
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)          # but not the RPATH used in installation
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}:${CMAKE_INSTALL_FULL_LIBDIR}/mod") # instead, add this one
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)        # in addition to those deduced automatically
endif()

add_subdirectory(libs/gml)
add_subdirectory(libs/jla_boost)
add_subdirectory(libs/libmod)
add_subdirectory(libs/pymod)
add_subdirectory(libs/post_mod)
add_subdirectory(doc)
add_subdirectory(examples)
add_subdirectory(test)


# Packaging
# -------------------------------------------------------------------------
string(TOUPPER ${PROJECT_NAME} PNAME_UPPER)
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})  # because the default doesn't include the tweak number
set(CPACK_GENERATOR "DEB")
#set(CPACK_GENERATOR "DEB" "RPM")
set(CPACK_DEB_COMPONENT_INSTALL ON)
#set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS ON)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

# we use CACHE INTERNAL to make it work when in a subproject
set(CPACK_DEBIAN_${PNAME_UPPER}_RUN_PACKAGE_SHLIBDEPS ON                                          CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_LIB_PACKAGE_SHLIBDEPS ON                                          CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_DEV_PACKAGE_SHLIBDEPS ON                                          CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_DOC_PACKAGE_SHLIBDEPS ON                                          CACHE INTERNAL "")

set(CPACK_COMPONENT_${PNAME_UPPER}_RUN_DEPENDS ${PNAME_UPPER}_LIB                                 CACHE INTERNAL "")
set(CPACK_COMPONENT_${PNAME_UPPER}_DEV_DEPENDS ${PNAME_UPPER}_LIB                                 CACHE INTERNAL "")

set(CPACK_DEBIAN_${PNAME_UPPER}_RUN_PACKAGE_NAME    "${PNAME_FILE}"                               CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_LIB_PACKAGE_NAME "lib${PNAME_FILE}"                               CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_DEV_PACKAGE_NAME "lib${PNAME_FILE}-dev"                           CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_DOC_PACKAGE_NAME    "${PNAME_FILE}-doc"                           CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_RUN_FILE_NAME       "${PNAME_FILE}_${PROJECT_VERSION}.deb"        CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_LIB_FILE_NAME    "lib${PNAME_FILE}_${PROJECT_VERSION}.deb"        CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_DEV_FILE_NAME    "lib${PNAME_FILE}-dev_${PROJECT_VERSION}.deb"    CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_DOC_FILE_NAME       "${PNAME_FILE}-doc_${PROJECT_VERSION}.deb"    CACHE INTERNAL "")

#set(CPACK_RPM_${PNAME_UPPER}_RUN_PACKAGE_NAME       "${PNAME_FILE}"                              CACHE INTERNAL "")
#set(CPACK_RPM_${PNAME_UPPER}_LIB_PACKAGE_NAME    "lib${PNAME_FILE}"                              CACHE INTERNAL "")
#set(CPACK_RPM_${PNAME_UPPER}_DEV_PACKAGE_NAME    "lib${PNAME_FILE}-devel"                        CACHE INTERNAL "")
#set(CPACK_RPM_${PNAME_UPPER}_DOC_PACKAGE_NAME       "${PNAME_FILE}-doc"                          CACHE INTERNAL "")
#set(CPACK_RPM_${PNAME_UPPER}_RUN_FILE_NAME          "${PNAME_FILE}-${PROJECT_VERSION}.rpm"       CACHE INTERNAL "")
#set(CPACK_RPM_${PNAME_UPPER}_LIB_FILE_NAME       "lib${PNAME_FILE}-${PROJECT_VERSION}.rpm"       CACHE INTERNAL "")
#set(CPACK_RPM_${PNAME_UPPER}_DEV_FILE_NAME       "lib${PNAME_FILE}-devel-${PROJECT_VERSION}.rpm" CACHE INTERNAL "")
#set(CPACK_RPM_${PNAME_UPPER}_DOC_FILE_NAME          "${PNAME_FILE}-doc-${PROJECT_VERSION}.rpm"   CACHE INTERNAL "")

if(NOT hasParent)
    set(CPACK_SOURCE_PACKAGE_FILE_NAME
            "${PNAME_FILE}-${CPACK_PACKAGE_VERSION}")
    set(CPACK_SOURCE_GENERATOR "TGZ")
    set(CPACK_SOURCE_IGNORE_FILES
            ".swp$"
            "~$"
            "__pycache__"
            "*.pyc$"
            "/.git"
            "/.idea"
            "/build"
            "/stage"
            "/test/py/.*/out/"
            "/test/py/.*/summary/"
            "/test/py/.*/.gdb_history"
            "/test/py/graph/myGraph.gml"
            "/test/py/rule/myRule.gml"
            "/bootstrap.sh"
            "/external")
    add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
    include(CPack)
endif()
