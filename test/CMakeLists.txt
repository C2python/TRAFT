set (DIR_SRCS
    test_log.cc
    )

add_executable(logtest
    ${DIR_SRCS}
    )

add_dependencies(logtest traft_a)
#add_test(logtest logtest command logtest)
find_package(GTest CONFIG REQUIRED)
set(_GTEST_LIBGTEST GTest::gtest_main)
target_link_libraries(logtest ${_GTEST_LIBGTEST} traft_a)
INSTALL(TARGETS logtest DESTINATION bin)
#target_include_directories(logtest PRIVATE ${PROJECT_BINARY_DIR})
#target_include_directories(logtest ${THIRD_PARTY_INCS})
#target_include_directories(logtest PUBLIC ${PROJECT_SOURCE_DIR}/src)