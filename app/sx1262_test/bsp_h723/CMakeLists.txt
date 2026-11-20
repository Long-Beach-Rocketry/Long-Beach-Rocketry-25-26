add_library(sx1262_test_bsp STATIC
    h723_board.cc
)

target_include_directories(sx1262_test_bsp PUBLIC
    .
    ..
)

target_link_libraries(sx1262_test_bsp PUBLIC
    driver
    core
)