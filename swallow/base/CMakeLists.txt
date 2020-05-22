set(LIB_SRC
    log.cc
    coroutine.cc
    thread.cc
    schedule.cc
   )
add_library(swallow SHARED ${LIB_SRC})
set_target_properties(swallow PROPERTIES VERSION 1.0 S0vERSION 1)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)