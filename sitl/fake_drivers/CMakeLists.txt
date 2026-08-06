add_library(fake_drivers STATIC
    fake_i2c.cc
)

target_include_directories(fake_drivers
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(fake_drivers
    PUBLIC
        driver
)
