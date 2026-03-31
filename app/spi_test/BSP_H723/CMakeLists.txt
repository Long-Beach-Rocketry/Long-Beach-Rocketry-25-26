add_library(spi_app_bsp STATIC
    spi_app_bsp.cc
    ${CMAKE_SOURCE_DIR}/common/drivers/platform/stm32h7/st_spi.cc
    ${CMAKE_SOURCE_DIR}/common/core/periph/chip_select/gpio_cs.cc
)

target_include_directories(spi_app_bsp PUBLIC
    .
    ${CMAKE_SOURCE_DIR}/app/spi_test
    ${CMAKE_SOURCE_DIR}/common/drivers/platform/stm32h7
    ${CMAKE_SOURCE_DIR}/common/core/periph/chip_select
)

target_link_libraries(spi_app_bsp
    PUBLIC
        driver
        core
)
