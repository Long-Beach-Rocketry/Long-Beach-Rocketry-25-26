if (TARGET_DEVICE MATCHES "STM32")
    add_library(drv8245 STATIC
        drv8245.cc
        drv8245.h
    )

    target_include_directories(drv8245 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries(drv8245 PUBLIC
        gpio
        pwm
    )
endif()