add_library(spi_app_bsp STATIC
    spi_app_bsp.cc
    ${CMAKE_SOURCE_DIR}/common/drivers/platform/stm32l4/st_spi_module.cc
    ${CMAKE_SOURCE_DIR}/common/drivers/platform/stm32l4/st_spi.cc
)

target_include_directories(spi_app_bsp PUBLIC
    .
    ${CMAKE_SOURCE_DIR}/common/drivers/platform/stm32l4
)

target_link_libraries(spi_app_bsp
    PUBLIC
        driver
)