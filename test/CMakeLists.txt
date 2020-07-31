add_executable(test_log test_log.cc)
target_link_libraries(test_log swallow gtest)

add_executable(test_redis test_redis.cc)
target_link_libraries(test_redis swallow gtest hiredis)