###########################################################################
# Targets and Artefacts
###########################################################################

add_library(jla_boost STATIC
        ${mod_jla_boost_INCLUDE_FILES}
        ${mod_jla_boost_SRC_FILES})
add_library(JLA::boost ALIAS jla_boost)
target_include_directories(jla_boost
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(jla_boost PUBLIC GraphCanon::graph_canon Boost::boost)
target_compile_options(jla_boost PRIVATE -Wall -Wextra -pedantic
        -Wno-unused-parameter)
set_target_properties(jla_boost PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON)
target_compile_definitions(jla_boost PRIVATE JLA_BOOST_SOURCE)

target_add_coverage(jla_boost)

install(TARGETS jla_boost
        EXPORT PROJECT_exports
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT JLA_BOOST_lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT JLA_BOOST_lib
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT JLA_BOOST_lib)
install(DIRECTORY include/jla_boost
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT JLA_BOOST_dev
        FILES_MATCHING PATTERN "*.hpp")


###########################################################################
# Tests
###########################################################################

if(BUILD_TESTING)
    if(NOT TARGET tests)
        add_custom_target(tests)
    endif()
    add_dependencies(tests ${mod_jla_boost_TEST_CPP_FILES})

    if("x${CMAKE_CXX_COMPILER_ID}" STREQUAL "xAppleClang")
        set(sanFlags -g -fsanitize=undefined -fsanitize=address)
    else()
        set(sanFlags -g -fsanitize=undefined -fsanitize=address -fsanitize=leak)
    endif()
    foreach(testName ${mod_jla_boost_TEST_CPP_FILES})
        add_executable(${testName} EXCLUDE_FROM_ALL test/${testName}.cpp)
        target_link_libraries(${testName} PRIVATE JLA::boost)
        if(BUILD_TESTING_SANITIZERS AND NOT BUILD_COVERAGE)
            target_compile_options(${testName} PRIVATE ${sanFlags})
            target_link_libraries(${testName} PRIVATE ${sanFlags})
        endif()
        target_add_coverage(${testName})
        add_test(${testName} ${testName})
        add_coverage_case(${testName})
    endforeach()
endif()