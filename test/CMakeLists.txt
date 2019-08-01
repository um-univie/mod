if(NOT BUILD_TESTING)
    return()
endif()

if(BUILD_PY_MOD)
    foreach(fileName ${mod_TEST_PY_FILES})
        string(REPLACE "/" "__" testName "${fileName}")
        set(workDir ${CMAKE_CURRENT_BINARY_DIR}/workDir/${testName})
        file(MAKE_DIRECTORY ${workDir})
        add_test(NAME ${testName}
                COMMAND ${CMAKE_INSTALL_FULL_BINDIR}/mod -f ${CMAKE_CURRENT_LIST_DIR}/${fileName}.py
                WORKING_DIRECTORY ${workDir})
        set_tests_properties(${testName} PROPERTIES
                ENVIRONMENT MOD_NUM_POST_THREADS=1)
        add_coverage_case(${testName})
    endforeach()
endif()

add_subdirectory(cmake_add_subdirectory)
add_subdirectory(cmake_import)
