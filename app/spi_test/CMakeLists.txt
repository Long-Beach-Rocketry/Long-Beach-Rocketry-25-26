set(EXECUTABLE spi_test)
set(LDF ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}.ld)

set(PREPROCESS_DEFS
    -DDEF_FLASH_START_ADDR=0x8000000
    -DDEF_FLASH_SIZE=1024K
)

add_subdirectory_for(STM32L476 BSP_L476)
add_subdirectory_for(STM32H723 BSP_H723)

if (TARGET spi_bsp OR TARGET spi_app_bsp)

    # Select the entry point that matches the active board BSP.
    if (TARGET spi_bsp)
        set(SPI_TEST_MAIN main_h7.cc)
    else()
        set(SPI_TEST_MAIN main_l4.cc)
    endif()

    add_executable_for(${TARGET_DEVICE} ${EXECUTABLE} ${LDF}
        ${SPI_TEST_MAIN}
        ${STARTUP_FILE}
    )

    target_include_directories_for(${TARGET_DEVICE} ${EXECUTABLE}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_options_for(${TARGET_DEVICE} ${EXECUTABLE} PRIVATE
        --specs=nano.specs
        -Wl,--gc-sections
    )

    if (TARGET spi_bsp)
        target_link_libraries_for(${TARGET_DEVICE} ${EXECUTABLE} PRIVATE
            spi_bsp
        )
    else()
        target_link_libraries_for(${TARGET_DEVICE} ${EXECUTABLE} PRIVATE
            spi_app_bsp
        )
    endif()

    target_preprocess_for(${TARGET_DEVICE} ${EXECUTABLE} ${LINKER_SCRIPT} ${LDF} ${PREPROCESS_DEFS})

endif()