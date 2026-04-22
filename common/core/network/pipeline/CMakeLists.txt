add_library(pipeline STATIC usart_pipe.cc)

target_include_directories(pipeline PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common/drivers/bus
    ${CMAKE_SOURCE_DIR}/common/core/network
    ${CMAKE_SOURCE_DIR}/common/core/protobuff
    ${CMAKE_SOURCE_DIR}/external/nanopb
    ${CMAKE_SOURCE_DIR}/common/core/protobuff/built
)
