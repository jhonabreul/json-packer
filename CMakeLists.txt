cmake_minimum_required(VERSION 3.22)
project(json_packer LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

include(cmake/StandardProjectSettings.cmake)
include(cmake/NlohmannJSON.cmake)

add_subdirectory(src)

option(JP_BUILD_TESTS "Build tests" ON)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND JP_BUILD_TESTS)
    include(cmake/Catch2.cmake)
    enable_testing()
    add_subdirectory(test)
endif()
