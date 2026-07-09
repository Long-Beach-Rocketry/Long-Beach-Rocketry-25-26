add_library(usart_bsp STATIC
    usart_app_bsp.cc
)

target_include_directories(usart_bsp PUBLIC
    .
    ../
)

target_link_libraries(usart_bsp
    PUBLIC
        driver
)