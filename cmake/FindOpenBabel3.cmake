# - Try to find OpenBabel3
# Once done this will define
#
#  OPENBABEL3_FOUND - system has OpenBabel3
#
# Copyright (c) 2006, 2007 Carsten Niehaus, <cniehaus@gmx.de>
# Copyright (C) 2008 Marcus D. Hanwell <marcus@cryos.org>
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# Modified 2020 by Jakob Lykke Andersen, <jlandersen@imada.sdu.dk>
# Defines the library target OpenBabel::OpenBabel
# Sets OpenBabel3_FOUND as this is proper case for the name.

if(OPENBABEL3_INCLUDE_DIR AND OPENBABEL3_LIBRARIES AND OPENBABEL3_VERSION_MET)
    # in cache already
    set(OPENBABEL3_FOUND TRUE)
else()
    if(NOT WIN32)
        # Use the newer PkgConfig stuff
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(OPENBABEL3 openbabel-3>=${OpenBabel3_FIND_VERSION})

        # Maintain backwards compatibility with previous version of module
        if(OPENBABEL3_FOUND STREQUAL "1")
            set(OPENBABEL3_VERSION_MET TRUE)
            set(OPENBABEL3_INCLUDE_DIR ${OPENBABEL3_INCLUDE_DIRS})
        endif()
    else()
        set(OPENBABEL3_VERSION_MET TRUE)
    endif()

    if(OPENBABEL3_VERSION_MET)
        if(WIN32)
            if(NOT OPENBABEL3_INCLUDE_DIR)
                find_path(OPENBABEL3_INCLUDE_DIR openbabel-3.0/openbabel/obconversion.h
                        PATHS
                        ${_obIncDir}
                        ${GNUWIN32_DIR}/include
                        $ENV{OPENBABEL3_INCLUDE_DIR})
                if(OPENBABEL3_INCLUDE_DIR)
                    set(OPENBABEL3_INCLUDE_DIR ${OPENBABEL3_INCLUDE_DIR}/openbabel-3.0)
                endif()
            endif()
        endif()

        find_library(OPENBABEL3_LIBRARIES NAMES openbabel openbabel-3
                PATHS
                ${_obLinkDir}
                ${GNUWIN32_DIR}/lib
                $ENV{OPENBABEL3_LIBRARIES})
    endif()

    if(OPENBABEL3_INCLUDE_DIR AND OPENBABEL3_LIBRARIES AND OPENBABEL3_VERSION_MET)
        set(OPENBABEL3_FOUND TRUE)
    endif()

    if(OPENBABEL3_FOUND)
        if(NOT OpenBabel3_FIND_QUIETLY)
            message(STATUS "Found OpenBabel ${OpenBabel3_FIND_VERSION} or later: ${OPENBABEL3_LIBRARIES}")
        endif()
    else(OPENBABEL3_FOUND)
        if(OpenBabel3_FIND_REQUIRED)
            message(FATAL_ERROR "Could NOT find OpenBabel ${OpenBabel3_FIND_VERSION} or later."
                    "\nAdd an appropriate pkg-config path to CMAKE_PREFIX_PATH."
                    "\nCMAKE_PREFIX_PATH currently is: ${CMAKE_PREFIX_PATH}")
        endif()
    endif()

    mark_as_advanced(OPENBABEL3_INCLUDE_DIR OPENBABEL3_LIBRARIES)
endif()

# Search for Open Babel3 executable
if(OPENBABEL3_EXECUTABLE)
    # in cache already
    set(OPENBABEL3_EXECUTABLE_FOUND TRUE)
else()
    find_program(OPENBABEL3_EXECUTABLE NAMES babel
            PATHS
            [HKEY_CURRENT_USER\\SOFTWARE\\OpenBabel\ 3.0.0]
            $ENV{OPENBABEL3_EXECUTABLE})
    if(OPENBABEL3_EXECUTABLE)
        set(OPENBABEL3_EXECUTABLE_FOUND TRUE)
    endif()
    if(OPENBABEL3_EXECUTABLE_FOUND)
        message(STATUS "Found OpenBabel3 executable: ${OPENBABEL3_EXECUTABLE}")
    endif()
endif()

if(OPENBABEL3_FOUND)
    add_library(OpenBabel::OpenBabel SHARED IMPORTED)
    set_target_properties(OpenBabel::OpenBabel PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES ${OPENBABEL3_INCLUDE_DIR}
            IMPORTED_LOCATION ${OPENBABEL3_LINK_LIBRARIES})
endif()
set(OpenBabel3_FOUND ${OPENBABEL3_FOUND})