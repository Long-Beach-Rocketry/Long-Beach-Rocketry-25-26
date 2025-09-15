add_library(hal STATIC
    st_gpio.cc
)

target_include_directories(hal PUBLIC
    .
    #bus #why this? i dont think needed
)

target_link_libraries(hal PUBLIC
    st_l4_support
    core #i dont see why needed
)