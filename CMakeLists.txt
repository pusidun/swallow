# cmake minimum version
cmake_minimum_required(VERSION 3.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# version
project(swallow VERSION 1.0)

# cmake options
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS " -rdynamic -O3 -g -Wall -Wno-deprecated -Werror -Wno-unused-function")

# cmake enviroment
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# dependencies
find_library(PTHREAD pthread)

# Boost
find_package(Boost REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

# hiredis
find_path(HIREDIS_INCLUDE_DIR NAMES hiredis/hiredis.h PATHS /opt/module/redis-6.2.2/deps)
find_library(HIREDIS_LIBRARY NAMES hiredis PATHS /opt/module/redis-6.2.2/deps/hiredis)
if(HIREDIS_INCLUDE_DIR AND HIREDIS_LIBRARY)
  include_directories(${HIREDIS_INCLUDE_DIR})
  link_directories(${HIREDIS_LIBRARY})
  message(STATUS "hiredis found.")
endif()

# gtest
find_path(GTEST_INCLUDE_DIR gtest/gtest.h)
find_library(GTEST_LIBRARY NAMES gtest)
if(GTEST_INCLUDE_DIR AND GTEST_LIBRARY)
  include_directories(${GTEST_INCLUDE_DIR})
  link_directories(${GTEST_LIBRARY})
  message(STATUS "gtest found.")
endif()

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(swallow/base)
add_subdirectory(test)
add_subdirectory(examples)
add_subdirectory(benchmark)