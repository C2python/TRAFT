#Project TRAFT by ZhangWen

set (DIR_SRCS
    config.cc
    log.cc
    PrebufferedStream.cc
    util.cc
    )

add_library(traft_a STATIC ${DIR_SRCS})

add_executable(traft traft.cc)

find_package(Threads REQUIRED)

if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(traft PUBLIC "-pthread")
endif()

MESSAGE (STATUS "MAKE_THREAD_LIBS_INIT=${CMAKE_THREAD_LIBS_INIT}")

target_link_libraries(traft PUBLIC traft_a "${CMAKE_THREAD_LIBS_INIT}")

install (TARGETS traft DESTINATION bin)
