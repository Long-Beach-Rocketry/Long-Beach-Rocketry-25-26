add_library(protobuff STATIC
    proto.cc
    protobuff.pb.c
)


target_include_directories(protobuff PUBLIC
    ${CMAKE_SOURCE_DIR}/external/nanopb
)


add_executable(protobufftest proto.cc)

target_link_libraries(protobufftest PRIVATE 
    protobuff
    nanopb
)