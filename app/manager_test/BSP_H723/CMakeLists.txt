add_library(manager_test_bsp STATIC
    h723_board.cc
)

target_include_directories(manager_test_bsp PUBLIC
    .
    ..
)

target_link_libraries(manager_test_bsp PUBLIC
    driver
    core
    manager
)
