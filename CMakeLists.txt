#Project TRAFT by ZhangWen

cmake_minimum_required (VERSION 3.5.1)

set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MAJOR 0)

project(TRAFT)

set (CMAKE_INSTALL_PREFIX ./)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)