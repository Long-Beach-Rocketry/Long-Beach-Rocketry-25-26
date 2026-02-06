add_library(st_f7_support STATIC
    system_stm32f7xx.c
)

target_include_directories(st_f7_support PUBLIC .)
target_link_libraries(st_f7_support PUBLIC
    cmsis
    st_f7_hal
)

add_subdirectory(HAL)