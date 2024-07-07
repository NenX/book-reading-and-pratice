cmake_minimum_required(VERSION 3.30.0)
project(test_co VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB libs RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_STANDARD  20)
add_executable(test_co "${libs}")
# add_executable(test_co main.cpp a.cpp b.cpp c.cpp d.cpp co_thread.cpp)

