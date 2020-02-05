# BuildConfig.h
# -------------------------------------------------------------------------
configure_file(BuildConfig.hpp.in include/mod/BuildConfig.hpp @ONLY)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/mod
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT mod_dev)


# libmod
# -------------------------------------------------------------------------
add_library(libmod SHARED
        ${mod_libmod_INCLUDE_FILES}
        ${mod_libmod_SRC_FILES})
add_library(mod::libmod ALIAS libmod)
target_add_coverage(libmod)
target_include_directories(libmod
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        ${Boost_INCLUDE_DIR})
if(ENABLE_DEP_SYMBOL_HIDING)
    target_link_libraries(libmod PRIVATE
            $<$<CXX_COMPILER_ID:GNU>:-Wl,--exclude-libs,ALL>
            $<$<CXX_COMPILER_ID:Clang>:-Wl,--exclude-libs,ALL>
            $<$<CXX_COMPILER_ID:AppleClang>:-Wl,--exclude-libs,ALL>)
endif()
target_link_libraries(libmod
        PUBLIC
        JLA::boost
        PRIVATE
        GML::gml Threads::Threads
        Boost::iostreams Boost::system
        GraphCanon::graph_canon
        $<$<BOOL:${WITH_OPENBABEL}>:OpenBabel::OpenBabel>
        )
target_compile_options(libmod PRIVATE -Wall -Wextra
        -Wno-unused-parameter
        -Wno-comment
        -Wno-sign-compare
        -Wno-unused-local-typedefs
        -Wno-unused-const-variable # gives false positive in the ILP wrapper
        $<$<CXX_COMPILER_ID:Clang>:-Wno-unused-private-field>
        )
target_compile_options(libmod PUBLIC
        $<$<CXX_COMPILER_ID:Clang>:-Wno-mismatched-tags>
        $<$<CXX_COMPILER_ID:AppleClang>:-Wno-mismatched-tags>)
set_target_properties(libmod PROPERTIES OUTPUT_NAME "mod")
if(ENABLE_SYMBOL_HIDING)
    set_target_properties(libmod PROPERTIES
            CXX_VISIBILITY_PRESET hidden
            # The inlines in Open Babel needs to be visible for the plugins to work.
            # GCC does fine, but not Clang, hence the generator expression.
            $<$<CXX_COMPILER_ID:GNU>VISIBILITY_INLINES_HIDDEN ON>
            )
endif()
target_compile_definitions(libmod PRIVATE MOD_SOURCE)
if(ENABLE_IPO)
    check_ipo_supported(RESULT result OUTPUT output)
    if(result)
        set_property(TARGET libmod PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(FATAL_ERROR "Interprocedural optimization was requested ('ENABLE_IPO=on'), but it is not supported: ${output}")
    endif()
endif()

install(TARGETS libmod
        EXPORT PROJECT_exports
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT mod_lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT mod_lib
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT mod_lib)
install(DIRECTORY src/mod
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT mod_dev
        FILES_MATCHING PATTERN "*.hpp")


# CMake Package Support
# -------------------------------------------------------------------------
include(CMakePackageConfigHelpers)
set(exportDir ${CMAKE_CURRENT_BINARY_DIR}/lib/cmake/${PROJECT_NAME})
configure_file(modConfig.cmake ${exportDir}/modConfig.cmake @ONLY)
foreach(f ${libmod_config_find_files})
    configure_file(${PROJECT_SOURCE_DIR}/cmake/${f} ${exportDir}/${f} COPYONLY)
endforeach()
write_basic_package_version_file(
        ${exportDir}/${PROJECT_NAME}Config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)
install(EXPORT PROJECT_exports
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION lib/cmake/${PROJECT_NAME}
        FILE ${PROJECT_NAME}Targets.cmake)
install(DIRECTORY ${exportDir}
        DESTINATION lib/cmake
        COMPONENT mod_dev)


# pkg-config file
# -------------------------------------------------------------------------
set(exportDir ${CMAKE_CURRENT_BINARY_DIR}/lib/pkgconfig)
configure_file(lib/pkgconfig/mod.pc.in ${exportDir}/mod.pc @ONLY)
install(FILES ${exportDir}/mod.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        COMPONENT mod_dev)
