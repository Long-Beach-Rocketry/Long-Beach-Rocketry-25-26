add_library(st_l4_support STATIC
    system_stm32l4xx.c
)

target_include_directories(st_l4_support PUBLIC .)

target_link_libraries(st_l4_support PUBLIC
    cmsis
    st_l4_hal
)

add_subdirectory(HAL)