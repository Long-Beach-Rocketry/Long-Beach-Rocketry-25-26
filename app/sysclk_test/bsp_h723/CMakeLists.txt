add_library(sysclk_bsp STATIC
    h723_board.cc
)

target_include_directories(sysclk_bsp PUBLIC 
  .
  ../
)

target_link_libraries(sysclk_bsp PUBLIC
    #st_l4_support #grabed in driver i think
  #      app
#        core
    driver
)

