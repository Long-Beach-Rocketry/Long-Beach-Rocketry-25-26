# Only build bno055 for STM32 targets (requires st_i2c.h and hal)
if (TARGET_DEVICE MATCHES "STM32")
    add_library(bno055 STATIC
        bno055_imu.cc
    )

    target_include_directories(bno055 PUBLIC
        .
        ${CMAKE_SOURCE_DIR}/common/core/math
        ${CMAKE_SOURCE_DIR}/common/drivers/utils
    )

    # Depends on platform HAL wrapper, core, and comm (I2C)
    target_link_libraries(bno055 PUBLIC
        hal
        core
        driver_utils
    )
endif()