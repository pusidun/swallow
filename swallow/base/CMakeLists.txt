# compiler shared library
set(BASE_SRCS
    config.cc
    coroutine.cc
    log.cc
    #process.cc
    redis.cc
    schedule.cc
    thread.cc
)
add_library(swallow SHARED ${BASE_SRCS})
target_link_libraries(swallow pthread hiredis)
set_target_properties(swallow PROPERTIES VERSION 1.0 S0vERSION 1)

# 动态库安装路径
install(TARGETS swallow DESTINATION ${PROJECT_SOURCE_DIR}/lib)
#头文件安装路径
file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION ${PROJECT_SOURCE_DIR}/include)