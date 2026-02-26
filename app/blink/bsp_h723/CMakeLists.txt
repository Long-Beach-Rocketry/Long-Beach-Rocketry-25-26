add_library(blink_bsp STATIC
    h723_board.cc
    # hal_timebase.c
)

target_include_directories(blink_bsp PUBLIC 
  .
  ../
)

target_link_libraries(blink_bsp PUBLIC
    driver
)