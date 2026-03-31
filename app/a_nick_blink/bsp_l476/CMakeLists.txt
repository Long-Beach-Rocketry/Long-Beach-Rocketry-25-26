add_library(a_nick_blink_bsp STATIC
    l476_board.cc
    # hal_timebase.c
)

target_include_directories(a_nick_blink_bsp PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}          # .../bsp_l476
    ${CMAKE_CURRENT_LIST_DIR}/..       # .../a_nick_blink
)

target_link_libraries(a_nick_blink_bsp PUBLIC
    #st_l4_support #grabed in driver i think
  #      app
#        core
    driver
)

