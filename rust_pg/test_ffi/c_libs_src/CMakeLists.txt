# 在同级目录新建一个 build 目录编译
cmake_minimum_required(VERSION 3.10)
project(static_for_rust C)
project(dylib_for_rust C)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/../c_libs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/../c_libs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/../c_libs)
set(CMAKE_C_STANDARD 99)

add_library(static_for_rust static.c)
add_library(dylib_for_rust SHARED dylib.c)


# ==============