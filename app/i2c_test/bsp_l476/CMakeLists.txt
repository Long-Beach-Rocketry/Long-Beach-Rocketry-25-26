add_library(i2c_bsp STATIC
    i2c_test_bsp.cc
)

target_include_directories(i2c_bsp PUBLIC 
  .
  ../
)

target_link_libraries(i2c_bsp PUBLIC
    #st_l4_support #grabed in driver i think
  #      app
#        core
    driver
)

