add_library(ms561101_bsp STATIC
    ms561101_board.cc
)

target_include_directories(ms561101_bsp PUBLIC
    .
    ../
)

target_link_libraries(ms561101_bsp PUBLIC
    driver
    core
)