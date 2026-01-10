cmake_minimum_required(VERSION 3.13)

project(drv8245_test)

add_executable(drv8245_test
    main.cc
    board.h
)

target_link_libraries(drv8245_test PRIVATE drv8245)
