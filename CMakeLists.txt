cmake_minimum_required(VERSION 3.0)
project(swallow VERSION 1.0)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS " -rdynamic -O3 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")
set(
    THIRD_LIBRARY_INCLUDE
    ~/Downloads/redis-4.0.11/deps/hiredis
)
set(
    THIRD_LIBRARY_LIB
    ~/Downloads/redis-4.0.11/deps/hiredis
)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${THIRD_LIBRARY_INCLUDE})
link_directories(${THIRD_LIBRARY_LIB})

add_subdirectory(swallow/base)

include_directories("${PROJECT_SOURCE_DIR}/swallow/base")
link_directories("${PROJECT_SOURCE_DIR}/lib")

set (
    LIBS
    swallow
    hiredis
    pthread
)

# 日志类测试
add_executable(log_example examples/log_example.cc)
target_link_libraries(log_example ${LIBS})

# 配置测试类
#add_executable(config_example examples/config_example.cc)
#target_link_libraries(config_example ${LIBS})

# 协程测试类
add_executable(coroutine_example examples/coroutine_example.cc)
target_link_libraries(coroutine_example ${LIBS})

# 调度测试类
add_executable(schedule_example examples/schedule_example.cc)
target_link_libraries(schedule_example ${LIBS})

# Redis测试类
add_executable(redis_example examples/redis_example.cc)
target_link_libraries(redis_example ${LIBS})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
