cmake_minimum_required(VERSION 3.10)

project(LibmodTestProject CXX)

find_package(mod REQUIRED)

add_executable(doStuff main.cpp)
target_link_libraries(doStuff mod::libmod)
