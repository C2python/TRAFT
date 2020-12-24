#Project TRAFT by ZhangWen

MESSAGE (STATUS "CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}")

set (DIR_SRCS
    config.cc
    log.cc
    PrebufferedStream.cc
    util.cc
    Thread.cc
    signal.cc
    Timer.cc
    )
    #
# GRPC Protoc cmake 
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
if(CMAKE_CROSSCOMPILING)
  find_program(_PROTOBUF_PROTOC protoc)
else()
  set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()
# Find gRPC installation
# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP gRPC::grpc++)
if(CMAKE_CROSSCOMPILING)
  find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
  set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()

file(COPY include DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Proto file
get_filename_component(rf_proto "./protos/raft.proto" ABSOLUTE)
get_filename_component(rf_proto_path "${rf_proto}" PATH)

# Generated sources
set(rf_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/include/raft.pb.cc")
set(rf_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/include/raft.pb.h")
set(rf_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/include/raft.grpc.pb.cc")
set(rf_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/include/raft.grpc.pb.h")

message(STATUS "rf_proto=${rf_proto}")

add_custom_command(
      OUTPUT "${rf_proto_srcs}" "${rf_proto_hdrs}" "${rf_grpc_srcs}" "${rf_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}/include"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}/include"
        -I "${rf_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${rf_proto}"
      DEPENDS "${rf_proto}")

# Include generated *.pb.h files

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_library(traft_a STATIC ${DIR_SRCS})

add_executable(traft traft.cc ${rf_proto_srcs} ${rf_grpc_srcs})

find_package(Threads REQUIRED)

if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(traft PUBLIC "-pthread")
endif()

MESSAGE (STATUS "MAKE_THREAD_LIBS_INIT=${CMAKE_THREAD_LIBS_INIT}")

target_link_libraries(traft PUBLIC traft_a "${CMAKE_THREAD_LIBS_INIT}" ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

install (TARGETS traft DESTINATION bin)
