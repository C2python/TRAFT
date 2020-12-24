# Copyright zhangwen

cmake_minimum_required(VERSION 3.15.1)

project(TRAFT)

include(ExternalProject)

option( BUILD_TESTS    "Build unit tests."  ON )

if (NOT CMAKE_BUILD_DIRECTORY)
    set(CMAKE_BUILD_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
if (NOT CMAKE_INSTALL_PREFIX)
   set (CMAKE_INSTALL_PREFIX ${CMAKE_BUILD_DIRECTORY})
endif()

# Builds c-ares project from the git submodule.
# Note: For all external projects, instead of using checked-out code, one could
# specify GIT_REPOSITORY and GIT_TAG to have cmake download the dependency directly,
# without needing to add a submodule to your project.
#ExternalProject_Add(c-ares
#  PREFIX c-ares
#  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/grpc/third_party/cares/cares"
#  CMAKE_CACHE_ARGS
#        -DCARES_SHARED:BOOL=OFF
#        -DCARES_STATIC:BOOL=ON
#        -DCARES_STATIC_PIC:BOOL=ON
#        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/c-ares
#)

# Builds protobuf project from the git submodule.
ExternalProject_Add(protobuf
  PREFIX protobuf
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/protobuf/cmake"
  CMAKE_CACHE_ARGS
        -Dprotobuf_BUILD_TESTS:BOOL=OFF
        -Dprotobuf_WITH_ZLIB:BOOL=OFF
        -Dprotobuf_MSVC_STATIC_RUNTIME:BOOL=OFF
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/protobuf
)

# Builds zlib project from the git submodule.
#ExternalProject_Add(zlib
#  PREFIX zlib
#  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/grpc/third_party/zlib"
#  CMAKE_CACHE_ARGS
#        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/zlib
#)

# the location where protobuf-config.cmake will be installed varies by platform
if (APPLE)
  set(_FINDPACKAGE_PROTOBUF_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/protobuf/lib/cmake/protobuf")
else ()
  set(_FINDPACKAGE_PROTOBUF_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/protobuf/lib64/cmake/protobuf")
endif()

# if OPENSSL_ROOT_DIR is set, propagate that hint path to the external projects with OpenSSL dependency.
set(_CMAKE_ARGS_OPENSSL_ROOT_DIR "")
MESSAGE("OPENSSL_ROOT_DIR = ${OPENSSL_ROOT_DIR}")
if (OPENSSL_ROOT_DIR)
  set(_CMAKE_ARGS_OPENSSL_ROOT_DIR "-DOPENSSL_ROOT_DIR:PATH=${OPENSSL_ROOT_DIR}")
endif()

# Builds gRPC based on locally checked-out sources and set arguments so that all the dependencies
# are correctly located.
ExternalProject_Add(grpc
  PREFIX grpc
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/grpc"
  CMAKE_CACHE_ARGS
        -DgRPC_INSTALL:BOOL=ON
        -DgRPC_BUILD_TESTS:BOOL=OFF
        -DgRPC_PROTOBUF_PROVIDER:STRING=package
        -DgRPC_PROTOBUF_PACKAGE_TYPE:STRING=CONFIG
        -DProtobuf_DIR:PATH=${_FINDPACKAGE_PROTOBUF_CONFIG_DIR}
        #-DgRPC_ZLIB_PROVIDER:STRING=package
        #-DZLIB_ROOT:STRING=${CMAKE_CURRENT_BINARY_DIR}/zlib
        #-DgRPC_CARES_PROVIDER:STRING=package
        #-Dc-ares_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}/c-ares/lib/cmake/c-ares
        #-DgRPC_SSL_PROVIDER:STRING=package
        #${_CMAKE_ARGS_OPENSSL_ROOT_DIR}
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/grpc
  #DEPENDS c-ares protobuf zlib
  DEPENDS protobuf
)

if ( BUILD_TESTS )
  enable_testing()
endif()

if (UNIX)
    set(GTEST_EXTRA_COMPILER_FLAGS "-fPIC")
endif()

set(GTEST_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${GTEST_EXTRA_COMPILER_FLAGS})
set(GTEST_C_FLAGS ${CMAKE_C_FLAGS} ${GTEST_EXTRA_COMPILER_FLAGS})
set(gtest_PREFIX ${CMAKE_BUILD_DIRECTORY}/gtest/gtest-prefix)
set(gtest_INSTALL ${CMAKE_BUILD_DIRECTORY}/gtest/gtest-install)

if (APPLE)
  set(GTest_DIR "${gtest_INSTALL}/lib/cmake/GTest/")
else()
  set(GTest_DIR "${gtest_INSTALL}/lib64/cmake/GTest/")
endif()

externalProject_Add(gtest
  PREFIX ${gtest_PREFIX}
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/googletest"
  INSTALL_DIR ${gtest_INSTALL}
  CMAKE_ARGS
  	-DCMAKE_BUILD_TYPE=Release
        -DCMAKE_INSTALL_PREFIX=${gtest_INSTALL}
        -DCMAKE_C_FLAGS=${GTEST_C_FLAGS}
        -DCMAKE_CXX_FLAGS=${GTEST_CXX_FLAGS}
        -DBUILD_GMOCK=OFF
  LOG_DOWNLOAD 1
  LOG_INSTALL 1
)

# Build the  projects itself using a CMakeLists.txt that assumes all the dependencies
# have already been installed.
# Even though helloworld is not really an "external project" from perspective of this build,
# we are still importing it using ExternalProject_Add because that allows us to use find_package()
# to locate all the dependencies (if we were building helloworld directly in this build we,
# we would have needed to manually import the libraries as opposed to reusing targets exported by
# gRPC and protobuf).

MESSAGE (STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")

externalProject_Add(traft
  PREFIX traft
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../"
  BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/traft/"
  #INSTALL_COMMAND ""
  CMAKE_CACHE_ARGS
        -DProtobuf_DIR:PATH=${_FINDPACKAGE_PROTOBUF_CONFIG_DIR}
        ${_CMAKE_ARGS_OPENSSL_ROOT_DIR}
        -DgRPC_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}/grpc/lib/cmake/grpc
        -DgRPC_INC:PATH=${CMAKE_CURRENT_BINARY_DIR}/grpc/include
        -DGTest_INC:PATH=${CMAKE_CURRENT_BINARY_DIR}/gtest/include
        -DGTest_DIR:PATH=${GTest_DIR}
        -DBUILD_TESTS:BOOL=${BUILD_TESTS}
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
  DEPENDS protobuf grpc gtest
)

if (BUILD_TESTS)
   
  add_test(NAME logtest COMMAND ${CMAKE_CURRENT_BINARY_DIR}/bin/logtest)

endif()
