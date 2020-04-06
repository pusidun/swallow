cmake_minimum_required(VERSION 3.0)
project(swallow VERSION 1.0)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS " -rdynamic -O3 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(swallow/base)

include_directories("${PROJECT_SOURCE_DIR}/swallow/base")
link_directories("${PROJECT_SOURCE_DIR}/lib")

set (
    LIBS
    pthread
    swallow
)

# 日志类测试
add_executable(log_example examples/log_example.cc)
target_link_libraries(log_example ${LIBS})

# 配置测试类
add_executable(config_example examples/config_example.cc)
target_link_libraries(config_example ${LIBS})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
