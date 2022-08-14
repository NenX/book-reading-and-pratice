
cmake_minimum_required(VERSION 3.0.0)
project(cmaketest VERSION 0.1.0)

# pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
# pthread


include_directories("${PROJECT_BINARY_DIR}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib")


# Find all main*.cpp files and store in list mains
file(GLOB_RECURSE mains RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB libs RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/lib/*")



foreach(mainfile IN LISTS mains)
    # Get file name without directory
    # https://blog.csdn.net/wzj_110/article/details/116431616

    get_filename_component(lastname ${mainfile} NAME_WE)
    get_filename_component(dirname ${mainfile} DIRECTORY)
    STRING( REGEX REPLACE "^.*([\\/]+)" "" lastDirName ${dirname})


    set(targetName ${lastDirName}_${lastname})

    file(GLOB local_headers RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${dirname}/*.h")

    # message("------------:" ${local_headers})
    add_executable(${targetName} ${mainfile} ${libs} ${local_headers})

    if(lastDirName MATCHES "(8|21)")
        # crypt()
        target_link_libraries(${targetName} PRIVATE libcrypt.so)
        # crypt()

    else()
    endif()

    if(lastDirName MATCHES "(9)")
        # pthread
        target_link_libraries(${targetName} PRIVATE Threads::Threads)
        # pthread
    endif()
    

endforeach()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
