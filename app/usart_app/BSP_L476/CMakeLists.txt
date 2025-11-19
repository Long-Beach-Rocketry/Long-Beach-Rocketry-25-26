add_library(usart_app_bsp STATIC
    usart_app_bsp.cc
)

target_include_directories(usart_app_bsp PUBLIC 
    .
    ../
)
target_link_libraries(usart_app_bsp
    PUBLIC
        driver
)
