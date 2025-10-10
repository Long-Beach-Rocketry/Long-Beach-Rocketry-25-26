add_library(spi_app_bsp STATIC
    spi_app_bsp.cc
    spi_app_bsp.h
)

target_include_directories(spi_app_bsp PUBLIC .)

target_link_libraries(spi_app_bsp
    PUBLIC
        driver
)
