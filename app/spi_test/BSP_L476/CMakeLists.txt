add_library(spi_app_bsp STATIC
    spi_app_bsp.cc
    ${CMAKE_SOURCE_DIR}/common/drivers/platform/stm32l4/st_spi_module.cc
    ${CMAKE_SOURCE_DIR}/common/drivers/platform/stm32l4/st_spi.cc
    ${CMAKE_SOURCE_DIR}/common/core/periph/chip_select/gpio_cs.cc
)

target_include_directories(spi_app_bsp PUBLIC
    .
    ${CMAKE_SOURCE_DIR}/common/drivers/platform/stm32l4
    ${CMAKE_SOURCE_DIR}/common/core/periph/chip_select
)

target_link_libraries(spi_app_bsp
    PUBLIC
        driver
        core
)