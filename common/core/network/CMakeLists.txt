add_subdirectory(pipeline)
add_subdirectory(telemetry)

add_library(network INTERFACE)
target_link_libraries(network INTERFACE 
    pipeline
    telemetry
)

target_include_directories(network INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)