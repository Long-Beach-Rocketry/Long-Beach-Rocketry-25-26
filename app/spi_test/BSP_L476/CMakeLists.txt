add_library(spi_app_bsp STATIC
    spi_app_bsp.cc
    ${CMAKE_SOURCE_DIR}/common/drivers/platform/stm32l4/st_spi_module.cc
    ${CMAKE_SOURCE_DIR}/common/drivers/platform/stm32l4/st_spi.cc
    ${CMAKE_SOURCE_DIR}/common/core/periph/chip_select/cs_gpio.cc
)

target_include_directories(spi_app_bsp PUBLIC
    .
    ${CMAKE_SOURCE_DIR}/common/drivers/platform/stm32l4
)

target_link_libraries(spi_app_bsp
    PUBLIC
        driver
)

# Ensure the chip_select implementation is linked into the BSP static library
## cs_gpio.cc is included directly in the spi_app_bsp sources above,
## so do not link the separate `chip_select` target (it would add -lchip_select
## which caused the linker to search for libchip_select.a).