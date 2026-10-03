add_library(timebase_bsp STATIC
    h723_board.cc
)

target_include_directories(timebase_bsp PUBLIC
    .
    ../
)

target_link_libraries(timebase_bsp
    PUBLIC
        driver
)