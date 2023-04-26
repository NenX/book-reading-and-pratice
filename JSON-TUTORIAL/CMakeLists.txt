cmake_minimum_required(VERSION 3.0.0)

project(tinyjson_test)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -Wall")
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/tinyjson")

file(GLOB_RECURSE tinyjsonfiles RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "tinyjson/*.c")


add_library(tinyjson ${tinyjsonfiles})

add_executable(tinyjson_test test.c)

target_link_libraries(tinyjson_test tinyjson)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tinyjson/tinyjson.h DESTINATION ${CMAKE_BINARY_DIR}/)

