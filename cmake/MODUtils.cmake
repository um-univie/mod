

function(make_py_test fileName testName extraEnv)
    set(workDir ${CMAKE_CURRENT_BINARY_DIR}/workDir/${testName})
    file(MAKE_DIRECTORY ${workDir})
    add_test(NAME ${testName}
            COMMAND ${CMAKE_INSTALL_FULL_BINDIR}/mod -f ${CMAKE_CURRENT_LIST_DIR}/${fileName}.py
            WORKING_DIRECTORY ${workDir})
    set_tests_properties(${testName} PROPERTIES
            ENVIRONMENT "MOD_NUM_POST_THREADS=1${extraEnv}")
    add_coverage_case(${testName})
endfunction()
