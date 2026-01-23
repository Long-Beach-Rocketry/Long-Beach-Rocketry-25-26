add_library(hal STATIC
    st_gpio.cc
    st_spi.cc
    st_i2c.cc
    st_sys_clock.cc
    st_pwm.cc
    st_usart.cc
)

target_include_directories(hal PUBLIC
    .
    ${CMAKE_SOURCE_DIR}/common/core/utils
    ${CMAKE_SOURCE_DIR}/common/drivers/time
    )

target_link_libraries(hal PUBLIC
    st_l4_support
    core #i dont see why needed
)