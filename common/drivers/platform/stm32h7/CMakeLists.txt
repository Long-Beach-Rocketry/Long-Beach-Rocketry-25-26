add_library(hal STATIC
    st_gpio.cc
)

target_include_directories(hal PUBLIC
    .
    ${CMAKE_SOURCE_DIR}/common/core/utils
    #bus #why this? i dont think needed
)

target_link_libraries(hal PUBLIC
    st_h7_support
    core #i dont see why needed
)