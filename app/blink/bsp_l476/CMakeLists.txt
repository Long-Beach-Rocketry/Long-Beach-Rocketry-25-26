add_library(blink_bsp STATIC
    #blink_bsp.c
    hal_timebase.c
)

target_include_directories(blink_bsp PUBLIC .)

target_link_libraries(blink_bsp 
    PUBLIC
    st_l4_support #delete later, temp
  #      app
#        core
#        driver
)

