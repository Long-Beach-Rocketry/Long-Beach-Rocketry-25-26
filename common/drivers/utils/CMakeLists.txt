add_library(driver_utils STATIC
    delay.cc
)

target_include_directories(driver_utils PUBLIC
    .
)

# For embedded builds: link the device support lib for DWT/SysTick register access
if (TARGET_DEVICE MATCHES "^(STM32H7)[0-9]+")
    target_link_libraries(driver_utils PRIVATE st_h7_support)
elseif (TARGET_DEVICE MATCHES "^(STM32L4)[0-9]+")
    target_link_libraries(driver_utils PRIVATE st_l4_support)
endif()
