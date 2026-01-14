add_library(blink_bsp STATIC
    l476_board.cc
    # hal_timebase.c
)

target_include_directories(blink_bsp PUBLIC 
  .
  ../
)

target_link_libraries(blink_bsp PUBLIC
    #st_l4_support #grabed in driver i think
  #      app
#        core
    driver
)

