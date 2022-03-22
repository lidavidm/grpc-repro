cmake_minimum_required(VERSION 3.20)
project(Repro CXX)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)

include_directories(SYSTEM "${PROTOBUF_INCLUDE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

add_custom_command(
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/Flight.pb.cc"
         "${CMAKE_CURRENT_BINARY_DIR}/Flight.pb.h"
         "${CMAKE_CURRENT_BINARY_DIR}/Flight.grpc.pb.cc"
         "${CMAKE_CURRENT_BINARY_DIR}/Flight.grpc.pb.h"
  COMMAND protobuf::protoc
  ARGS "--cpp_out=${CMAKE_CURRENT_BINARY_DIR}"
       "--grpc_out=${CMAKE_CURRENT_BINARY_DIR}"
       "-I${CMAKE_SOURCE_DIR}/src"
       "--plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
       "${CMAKE_SOURCE_DIR}/src/Flight.proto"
  DEPENDS src/Flight.proto
  )

add_executable(repro src/main.cc "${CMAKE_CURRENT_BINARY_DIR}/Flight.pb.cc" "${CMAKE_CURRENT_BINARY_DIR}/Flight.grpc.pb.cc")
target_link_libraries(repro gRPC::grpc++ protobuf::libprotobuf)
