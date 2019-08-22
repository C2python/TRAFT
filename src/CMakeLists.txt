#Project TRAFT by ZhangWen

cmake_minimum_required (VERSION 3.0)
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MAJOR 0)


#Project Info

PROJECT (TRAFT)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "--std=c++1y -pthread")

set (DIR_SRCS
    Config.cc
    Log.cc
    PrebufferedStream.cc
    Util.cc
    )

add_library(traft_a STATIC ${DIR_SRCS})
#target_link_libraries(traft_a pthread)

add_executable(traft traft.cc)
target_link_libraries(traft traft_a ${CMAKE_THREAD_LIBS_INIT})
