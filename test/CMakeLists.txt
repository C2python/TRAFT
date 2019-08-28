set (DIR_SRCS
    test_log.cc
    )

add_executable(logtest
    ${DIR_SRCS}
    )

add_dependencies(logtest gtest traft_a)
add_test(logtest logtest command logtest)
target_link_libraries(logtest ${THIRD_PARTY_LIBS} traft_a)
target_include_directories(logtest PRIVATE ${PROJECT_BINARY_DIR})
target_include_directories(logtest ${THIRD_PARTY_INCS})
target_include_directories(logtest PUBLIC ${PROJECT_SOURCE_DIR}/src)