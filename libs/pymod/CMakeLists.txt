if(NOT BUILD_PY_MOD)
    return()
endif()

###########################################################################
# Targets and Artefacts
###########################################################################

# libpymod
# -------------------------------------------------------------------------
add_library(pymod MODULE
        ${mod_pymod_INCLUDE_FILES}
        ${mod_pymod_SRC_FILES})
add_library(mod::pymod ALIAS pymod)
target_include_directories(pymod
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
target_link_libraries(pymod PUBLIC mod::libmod Boost::${PYTHON_TARGET} Python3::Python)
target_link_libraries(pymod PRIVATE -Wl,--no-undefined)
target_compile_options(pymod PRIVATE -Wall -Wextra
        -Wno-unused-parameter
        -Wno-comment
        -Wno-unused-local-typedefs)
set_target_properties(pymod PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON)
target_compile_definitions(pymod PRIVATE PYMOD_SOURCE)
if(ENABLE_IPO)
    check_ipo_supported(RESULT result OUTPUT output)
    if(result)
        set_property(TARGET pymod PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(FATAL_ERROR "Interprocedural optimization was requested ('ENABLE_IPO=on'), but it is not supported: ${output}")
    endif()
endif()

install(TARGETS pymod
        EXPORT PROJECT_exports
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/mod COMPONENT pymod_lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/mod COMPONENT pymod_lib
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}     COMPONENT pymod_lib)
install(DIRECTORY src/mod
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT pymod_dev
        FILES_MATCHING PATTERN "*.hpp")

# the plugin file
set(exportDir ${CMAKE_CURRENT_BINARY_DIR}/share/mod/plugins)
configure_file(share/mod/plugins/00_mod.in ${exportDir}/00_mod.in @ONLY)
install(FILES ${exportDir}/00_mod.in
        DESTINATION ${CMAKE_INSTALL_DATADIR}/mod/plugins/
        COMPONENT pymod_run)

# module files
install(DIRECTORY lib/mod
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT pymod_run
        FILES_MATCHING PATTERN "*.py")


# mod
# -------------------------------------------------------------------------
set(exportDir ${CMAKE_CURRENT_BINARY_DIR}/bin)
configure_file(bin/mod.in ${exportDir}/mod @ONLY)
install(PROGRAMS ${exportDir}/mod
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT pymod_run)


# Valgrind suppression files
# -------------------------------------------------------------------------
install(FILES
            share/mod/obabel.supp
            share/mod/python.supp
        DESTINATION ${CMAKE_INSTALL_DATADIR}/mod
        COMPONENT pymod_run)