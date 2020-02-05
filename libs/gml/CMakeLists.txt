###########################################################################
# Targets and Artefacts
###########################################################################

add_library(gml STATIC
        ${mod_gml_INCLUDE_FILES}
        ${mod_gml_SRC_FILES})
add_library(GML::gml ALIAS gml)
target_include_directories(gml
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${Boost_INCLUDE_DIR}) # we only use header-only libs from Boost, right?
target_link_libraries(gml PRIVATE -Wl,--no-undefined)
target_compile_options(gml PRIVATE -Wall -Wextra
        -Wno-unused-parameter
        -Wno-parentheses)
set_target_properties(gml PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON)
target_compile_definitions(gml PRIVATE GML_SOURCE)

target_add_coverage(gml)

install(TARGETS gml
        EXPORT PROJECT_exports
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT GML_lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT GML_lib
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT GML_lib)
install(DIRECTORY include/gml
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT GML_dev
        FILES_MATCHING PATTERN "*.hpp")


###########################################################################
# Tests
###########################################################################

if(BUILD_TESTING)
    if(NOT TARGET tests)
        add_custom_target(tests)
    endif()
    add_dependencies(tests ${mod_gml_TEST_FILES})

    set(sanFlags -g -fsanitize=undefined -fsanitize=address -fsanitize=leak)
    foreach(testName ${mod_gml_TEST_FILES})
        add_executable(${testName} EXCLUDE_FROM_ALL test/${testName}.cpp)
        target_compile_options(${testName} PRIVATE -Wall -Wextra -Werror -pedantic
                -Wno-unused-parameter
                -Wno-parentheses)
        target_link_libraries(${testName} PRIVATE GML::gml)
        if(BUILD_TESTING_SANITIZERS AND NOT BUILD_COVERAGE)
            target_compile_options(${testName} PRIVATE ${sanFlags})
            target_link_libraries(${testName} PRIVATE ${sanFlags})
        endif()
        target_add_coverage(${testName})
        add_test(${testName} ${testName})
        add_coverage_case(${testName})
    endforeach()
endif()