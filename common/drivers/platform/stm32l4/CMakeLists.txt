add_library(hal STATIC
    st_gpio.cc
    st_spi.cc
    st_i2c.cc
    st_sys_clock.cc
<<<<<<< HEAD
    st_usart.cc
=======
    st_pwm.cc
>>>>>>> 87f61cf (rebase with main)
)

target_include_directories(hal PUBLIC
    .
    ${CMAKE_SOURCE_DIR}/common/core/utils
    #bus #why this? i dont think needed
)

target_link_libraries(hal PUBLIC
    st_l4_support
    core #i dont see why needed
)