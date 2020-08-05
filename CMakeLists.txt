#Project TRAFT by ZhangWen


cmake_minimum_required (VERSION 3.5.1)

#option( BUILD_TESTS    "Build unit tests."  ON )

set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MAJOR 0)

project(TRAFT)

MESSAGE (STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")


# Set Default CMAKE_BUILD_TYPE as Release
set (default_build_type Release)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall")

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

MESSAGE (STATUS "CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")

MESSAGE (STATUS "CMAKE_INSTALL_LIBDIR = ${CMAKE_INSTALL_LIBDIR}")


add_subdirectory(src)


MESSAGE (STATUS "CMAKE_BUILD_DIRECTORY = ${CMAKE_BUILD_DIRECTORY}")

if ( BUILD_TESTS )

    add_subdirectory(test)

endif()


		