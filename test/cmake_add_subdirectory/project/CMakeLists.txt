cmake_minimum_required(VERSION 3.10)

project(CMakeTestProject CXX)

add_subdirectory(${mod_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/mod)

add_executable(doStuff main.cpp)
target_link_libraries(doStuff mod::libmod)