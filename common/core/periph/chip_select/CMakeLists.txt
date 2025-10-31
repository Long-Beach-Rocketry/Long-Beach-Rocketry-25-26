add_library(chip_select INTERFACE)

target_sources(chip_select INTERFACE
    cs_gpio.cc
)

target_include_directories(chip_select INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(chip_select INTERFACE
    driver
)