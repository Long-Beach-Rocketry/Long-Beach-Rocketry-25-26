add_library(utils STATIC
    delay.cc
)

target_include_directories(utils PUBLIC
    .
)

# For embedded builds: include device + CMSIS headers
if (TARGET_DEVICE MATCHES "STM32")
    target_include_directories(utils PRIVATE
        ${CMAKE_SOURCE_DIR}/mcu_support/stm32/l4xx
        ${CMAKE_SOURCE_DIR}/mcu_support/CMSIS/include
    )
    target_link_libraries(utils PUBLIC cmsis)
endif()
