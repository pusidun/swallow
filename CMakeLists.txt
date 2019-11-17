cmake_minimum_required(VERSION 3.0)
project(swallow VERSION 1.0)
set(CMAKE_VERBOSE_MAKEFILE ON)
#set(CMAKE_CXX_FLAGS "$ENV(CXXFLAGS) -rdynamic -O3 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(swallow/base)

add_executable(logtest examples/log_example.cc)
target_link_libraries(logtest PUBLIC swallow)
target_include_directories(logtest PUBLIC
                           "${PROJECT_BINARY_DIR}"
						   "${PROJECT_SOURCE_DIR}/swallow/base"
                           )

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
