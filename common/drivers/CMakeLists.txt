if (TARGET_DEVICE MATCHES "^(STM32L4)[0-9]+")
    add_subdirectory(platform/stm32l4)
    add_subdirectory(platform/bno055)
endif()

add_subdirectory(utils)

add_library(driver INTERFACE)

target_include_directories(driver INTERFACE
    bus
    time
    io 
)

target_link_libraries(driver INTERFACE
    hal
    utils
)