target_sources(generated PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/built/protobuff.pb.c
)
target_include_directories(generated PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/built
)