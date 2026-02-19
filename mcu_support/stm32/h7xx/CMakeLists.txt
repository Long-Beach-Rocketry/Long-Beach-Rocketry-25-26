add_library(st_h7_support STATIC
    system_stm32h7xx.c
)

target_include_directories(st_h7_support PUBLIC .)

target_link_libraries(st_h7_support PUBLIC
    cmsis
    st_h7_hal
)

add_subdirectory(HAL)