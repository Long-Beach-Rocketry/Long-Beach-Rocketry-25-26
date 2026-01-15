add_library(drv8245 STATIC ${DRV8245_SRCS} ${DRV8245_HDRS})

target_include_directories(drv8245 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(drv8245
    PUBLIC
        gpio
        pwm
)
