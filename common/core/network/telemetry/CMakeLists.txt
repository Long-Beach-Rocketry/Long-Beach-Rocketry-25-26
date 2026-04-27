add_library(telemetry STATIC telemetry.cc)

target_include_directories(telemetry PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common/core/protobuff
    ${CMAKE_SOURCE_DIR}/common/core/protobuff/built
    ${CMAKE_SOURCE_DIR}/common/core/math
    ${CMAKE_SOURCE_DIR}/common/core/periph/bno055
    ${CMAKE_SOURCE_DIR}/common/core/periph/bmp390
    ${CMAKE_SOURCE_DIR}/external/nanopb
)

target_link_libraries(telemetry PUBLIC
    protobuff
    pb_cmd
    bno055
    bmp390
)
