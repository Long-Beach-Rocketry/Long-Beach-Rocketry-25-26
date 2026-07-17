
add_library(protobuff STATIC
    #test.cc
    pb_cmd.cc
    built/protobuff.pb.c
)

add_library(pb_cmd INTERFACE)

target_include_directories(pb_cmd INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(protobuff PUBLIC
    ${CMAKE_SOURCE_DIR}/external/nanopb
    ${CMAKE_CURRENT_SOURCE_DIR}/built
)

target_link_libraries(protobuff PUBLIC nanopb)

