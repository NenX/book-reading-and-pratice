cmake_minimum_required(VERSION 3.0.0)

project(haha)

file(GLOB_RECURSE libfiles PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} "libs/*.cpp" )

add_library(mylib ${libfiles})

add_executable(haha test.cpp)

target_link_libraries(haha mylib)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs")