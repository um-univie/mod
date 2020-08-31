if(NOT BUILD_TESTING)
    return()
endif()

if(NOT TARGET tests)
    add_custom_target(tests)
endif()

set(sanFlags -g -fsanitize=undefined -fsanitize=address -fsanitize=leak)
foreach(fileName ${mod_TEST_CPP_FILES})
    string(REPLACE "/" "__" testName "${fileName}")
    set(workDir ${CMAKE_CURRENT_BINARY_DIR}/workDir/${testName})
    file(MAKE_DIRECTORY ${workDir})
    add_executable(${testName} EXCLUDE_FROM_ALL ${fileName}.cpp)
    target_compile_options(${testName} PRIVATE
            -Wall -Wextra -Werror -pedantic -Wno-comment)
    target_link_libraries(${testName} PRIVATE mod::libmod)
    if(BUILD_TESTING_SANITIZERS AND NOT BUILD_COVERAGE)
        target_compile_options(${testName} PRIVATE ${sanFlags})
        target_link_libraries(${testName} PRIVATE ${sanFlags})
    endif()
    add_dependencies(tests ${testName})
    target_add_coverage(${testName})
    add_test(NAME ${testName}
            COMMAND ${testName}
            WORKING_DIRECTORY ${workDir})
    add_coverage_case(${testName})
endforeach()

if(BUILD_PY_MOD)
    function(make_py_test testName extraEnv)
        set(workDir ${CMAKE_CURRENT_BINARY_DIR}/workDir/${testName})
        file(MAKE_DIRECTORY ${workDir})
        add_test(NAME ${testName}
                COMMAND ${CMAKE_INSTALL_FULL_BINDIR}/mod -f ${CMAKE_CURRENT_LIST_DIR}/${fileName}.py
                WORKING_DIRECTORY ${workDir})
        set_tests_properties(${testName} PROPERTIES
                ENVIRONMENT "MOD_NUM_POST_THREADS=1${extraEnv}")
        add_coverage_case(${testName})
    endfunction()

    foreach(fileName ${mod_TEST_PY_FILES})
        string(REPLACE "/" "__" testName "${fileName}")
        make_py_test(${testName} "")
    endforeach()
endif()

add_subdirectory(cmake_add_subdirectory)
add_subdirectory(cmake_import)