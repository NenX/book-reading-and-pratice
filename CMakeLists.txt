# cmake_minimum_required(VERSION 3.0.0)
# project(TLPI VERSION 0.1.0)



# add_executable(TLPI main.c)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)




cmake_minimum_required(VERSION 3.0.0)
project(cmaketest VERSION 0.1.0)

include_directories("${PROJECT_BINARY_DIR}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib")


# Find all main*.cpp files and store in list mains
file(GLOB_RECURSE mains RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src/*main.c")
file(GLOB libs RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/lib/*")

foreach(aa IN LISTS libs)
    message("-------- " ${aa})
endforeach()

foreach(mainfile IN LISTS mains)
    # Get file name without directory
    # https://blog.csdn.net/wzj_110/article/details/116431616

    get_filename_component(lastname ${mainfile} NAME_WE)
    get_filename_component(dirname ${mainfile} DIRECTORY)
    STRING( REGEX REPLACE "^.*([\\/]+)" "" prjName ${dirname})

    add_executable(${prjName} ${mainfile} ${libs})
endforeach()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
