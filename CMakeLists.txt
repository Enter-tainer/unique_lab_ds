cmake_minimum_required(VERSION 3.15)
project(llrb)
find_package(Catch2 REQUIRED)
set(CMAKE_CXX_STANDARD 14)
add_executable(llrb main.cpp)
add_executable(llrb_test llrb_test.cpp)

target_link_libraries(llrb_test Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(llrb_test)