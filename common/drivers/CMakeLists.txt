if (TARGET_DEVICE MATCHES "^(STM32L4)[0-9]+")
    add_subdirectory(platform/stm32l4)
endif()

add_library(driver INTERFACE)

target_include_directories(driver INTERFACE
    bus
    io  
)

target_link_libraries(driver INTERFACE
    hal
)