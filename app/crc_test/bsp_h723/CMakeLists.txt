add_library(crc_bsp STATIC
    h723_board.cc
)

target_include_directories(crc_bsp PUBLIC
    .
    ../
)

target_link_libraries(crc_bsp
    PUBLIC
        driver
)