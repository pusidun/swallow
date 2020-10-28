# cmake minimum version
cmake_minimum_required(VERSION 3.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(swallow VERSION 1.0)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS " -rdynamic -O3 -g -Wall -Wno-deprecated -Werror -Wno-unused-function")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# dependencies
find_library(PTHREAD pthread)
find_package(Boost REQUIRED)
find_path(HIREDIS_INCLUDE_DIR hiredis/hiredis.h)
find_library(HIREDIS_LIBRARY NAMES hiredis)
find_path(GTEST_INCLUDE_DIR gtest/gtest.h)
find_library(GTEST_LIBRARY NAMES gtest)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()
if(HIREDIS_INCLUDE_DIR AND HIREDIS_LIBRARY)
  message(STATUS "found hiredis")
endif()
if(GTEST_INCLUDE_DIR AND GTEST_LIBRARY)
  message(STATUS "found gtest")
endif()

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${HIREDIS_INCLUDE_DIR})
link_directories(${HIREDIS_LIBRARY})
include_directories(${GTEST_INCLUDE_DIR})
link_directories(${GTEST_LIBRARY})
include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(swallow/base)
add_subdirectory(test)
add_subdirectory(examples)
add_subdirectory(benchmark)