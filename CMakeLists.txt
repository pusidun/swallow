cmake_minimum_required(VERSION 3.0)
project(swallow)

add_compile_options(-std=c++11)

set(LIB_SRC
	swallow/base/log.cc
	)
	
add_library(swallow SHARED ${LIB_SRC})
set_target_properties(swallow PROPERTIES VERSION 1.0 S0vERSION 1)
find_library(PTHREAD pthread)

set(LIBS
		swallow
		pthread
	)
	
include_directories(swallow/base)
link_directories(${PROJECT_SOURCE_DIR}/lib)
add_executable(log_example "examples/log_example.cc")
target_link_libraries(log_example libswallow.so)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
