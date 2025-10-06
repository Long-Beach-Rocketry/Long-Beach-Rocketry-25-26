add_library(spi_app_bsp STATIC
    spi_app_bsp.cc
)

# Make the current directory visible for includes (spi_bsp.h)
target_include_directories(spi_app_bsp PUBLIC .)

# Link to your SPI driver library
target_link_libraries(spi_app_bsp
    PUBLIC
        driver
)