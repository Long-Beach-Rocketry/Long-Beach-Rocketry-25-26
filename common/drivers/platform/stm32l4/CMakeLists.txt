add_library(hal STATIC
    st_gpio.cc
    st_i2c.cc
    st_i2c_module.cc
)

target_include_directories(hal PUBLIC
    .
    #bus #why this? i dont think needed
)

target_link_libraries(hal PUBLIC
    st_l4_support
    core #i dont see why needed
)