if (TARGET_DEVICE MATCHES "^(STM32L4)[0-9]+")
    add_subdirectory(platform/stm32l4)
    add_subdirectory(platform/bno055)
endif()

<<<<<<< HEAD
=======
add_subdirectory(utils)
>>>>>>> f3e1c03 (IMU driver)

add_library(driver INTERFACE)

target_include_directories(driver INTERFACE
    bus
    io 
)

target_link_libraries(driver INTERFACE
    hal
    utils
)