set(EXECUTABLE spi_test)

add_executable(${EXECUTABLE}
    main.cc
)

target_include_directories(${EXECUTABLE} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}                                   # app/spi_test/
    ${CMAKE_SOURCE_DIR}/common/drivers/bus                        # for bus headers
    ${CMAKE_SOURCE_DIR}/common/drivers/io                         # for io headers
    ${CMAKE_SOURCE_DIR}/common/drivers/platform/stm32l4           # for st_spi.h, st_gpio.h
    ${CMAKE_SOURCE_DIR}/BSP_L476                                  # if you have board headers here
    ${CMAKE_SOURCE_DIR}/cmsis_device/stm32l4xx/Include            # if your CMSIS/device headers live here
    ${CMAKE_SOURCE_DIR}/cmsis_core/Include                        # core_cm4.h etc.
)

target_link_libraries(${EXECUTABLE} PRIVATE
    hal
)
