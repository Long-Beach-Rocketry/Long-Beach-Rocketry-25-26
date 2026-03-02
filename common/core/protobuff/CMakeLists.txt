add_library(protobuff STATIC
    proto.cc
    pb_cmd.cc
    protobuff.pb.c
)


add_library(cmd_msg INTERFACE)
target_include_directories(cmd_msg INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(protobuff PUBLIC
    ${CMAKE_SOURCE_DIR}/external/nanopb
)
