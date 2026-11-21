set(EXECUTABLE w25q_test)
set(LDF ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}.ld)

set(PREPROCESS_DEFS
    -DDEF_FLASH_START_ADDR=0x8000000
    -DDEF_FLASH_SIZE=1024K
)


if (${TARGET_DEVICE} STREQUAL "STM32H723")
    set(BSP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/app/spi_test/BSP_H723)
    set(BSP_TARGET spi_bsp)
elseif(${TARGET_DEVICE} STREQUAL "STM32L476")
    set(BSP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/app/spi_test/BSP_L476)
    set(BSP_TARGET spi_app_bsp)
endif()

if (BSP_TARGET)
    if (NOT TARGET ${BSP_TARGET})
        add_subdirectory(${BSP_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/spi_bsp)
    endif()

    add_executable_for(${TARGET_DEVICE} ${EXECUTABLE} ${LDF}
        main.cc
        ${STARTUP_FILE}
    )

    target_link_options_for(${TARGET_DEVICE} ${EXECUTABLE} PRIVATE
        --specs=nano.specs
        -Wl,--gc-sections
    )

    target_include_directories_for(${TARGET_DEVICE} ${EXECUTABLE}
        PRIVATE ${CMAKE_SOURCE_DIR}/app/spi_test
        PRIVATE ${BSP_INCLUDE_DIR}

        PRIVATE ${CMAKE_SOURCE_DIR}/common/core/periph/flash_chip
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries_for(${TARGET_DEVICE} ${EXECUTABLE} PRIVATE
        ${BSP_TARGET}
        core
        w25q
    )

    target_preprocess_for(${TARGET_DEVICE} ${EXECUTABLE} ${LINKER_SCRIPT} ${LDF} ${PREPROCESS_DEFS})

endif()