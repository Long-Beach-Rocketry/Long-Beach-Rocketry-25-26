add_library(w25q INTERFACE)

target_sources(w25q INTERFACE
    w25q.cc
)

target_include_directories(w25q INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common/drivers/bus
)

target_link_libraries(w25q INTERFACE
    driver
    chip_select
)