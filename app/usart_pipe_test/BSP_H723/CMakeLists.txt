add_library(usart_pipe_test_bsp STATIC
    h723_board.cc
)

target_include_directories(usart_pipe_test_bsp PUBLIC
    .
    ../
)

target_link_libraries(usart_pipe_test_bsp
    PUBLIC
        driver
)