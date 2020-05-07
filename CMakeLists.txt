#Project TRAFT by ZhangWen

cmake_minimum_required (VERSION 3.5.1)

option( BUILD_TESTS    "Build unit tests."  ON )

set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MAJOR 0)

project(TRAFT)

MESSAGE (STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")

if ( NOT CMAKE_INSTALL_PREFIX)
    set (CMAKE_INSTALL_PREFIX "/usr/local")
endif()

#Enable Testing Cmake. Remove it.
set (CMAKE_INSTALL_PREFIX ${${PROJECT_BINARY_DIR}})

# Set Default CMAKE_BUILD_TYPE as Release
set (default_build_type Release)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall")

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

MESSAGE (STATUS "CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")

include(ExternalProject)

add_subdirectory(src)

if (NOT CMAKE_BUILD_DIRECTORY)
    set(CMAKE_BUILD_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

if ( BUILD_TESTS )

    enable_testing( )

    include(cmake/gtest.cmake)
    list(APPEND THIRD_PARTY_LIBS PRIVATE ${gtest_LIBRARIES})
    list(APPEND THIRD_PARTY_INCS PRIVATE ${gtest_INCLUDE_DIRS})

    add_subdirectory(test)

endif()
