#Project TRAFT by ZhangWen

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")

set (DIR_SRCS
    config.cc
    log.cc
    PrebufferedStream.cc
    util.cc
    )

add_library(traft_a STATIC ${DIR_SRCS})

add_executable(traft traft.cc)
target_link_libraries(traft traft_a ${CMAKE_THREAD_LIBS_INIT})

install (TARGETS traft DESTINATION bin)
