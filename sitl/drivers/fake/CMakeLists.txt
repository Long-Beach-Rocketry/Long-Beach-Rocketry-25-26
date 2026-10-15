add_library(fake STATIC
    fake_i2c.cc
)

target_include_directories(fake
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(fake
    PUBLIC
        driver
)
