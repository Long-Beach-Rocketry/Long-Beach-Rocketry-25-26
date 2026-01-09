add_library(st_f4_support STATIC
    system_stm32f4xx.c
)

target_include_directories(st_f4_support PUBLIC .)
target_link_libraries(st_f4_support PUBLIC
    cmsis
    st_f4_hal
)

add_subdirectory(HAL)