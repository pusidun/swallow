# 日志类
add_executable(log_example log_example.cc)
target_link_libraries(log_example swallow)

# 配置类
#add_executable(config_example config_example.cc)
#target_link_libraries(config_example swallow)

# 协程类
add_executable(coroutine_example coroutine_example.cc)
target_link_libraries(coroutine_example swallow)

# 调度类
add_executable(schedule_example schedule_example.cc)
target_link_libraries(schedule_example swallow)

# Redis类
add_executable(redis_example redis_example.cc)
target_link_libraries(redis_example swallow hiredis)

# Process
add_executable(process_example process_example.cc)
target_link_libraries(process_example swallow)