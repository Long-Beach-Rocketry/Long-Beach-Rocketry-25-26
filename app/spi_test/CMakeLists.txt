set(EXECUTABLE spi_test)
set(LDF ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}.ld)

set(PREPROCESS_DEFS
    -DDEF_FLASH_START_ADDR=0x8000000
    -DDEF_FLASH_SIZE=1024K
)

add_subdirectory_for(STM32L476 BSP_L476)
add_subdirectory_for(STM32H723 BSP_H723)

if (TARGET spi_app_bsp)

    # Choose the correct source file and include directory based on the target device
    if (${TARGET_DEVICE} STREQUAL "STM32H723")
        set(APP_SOURCE main_h7.cc)
        set(BSP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/BSP_H723)
    else()
        set(APP_SOURCE main_l4.cc)
        set(BSP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/BSP_L476)
    endif()

    # Create target using the selected source file
    add_executable_for(${TARGET_DEVICE} ${EXECUTABLE} ${LDF}
        ${APP_SOURCE}
        ${STARTUP_FILE}
    )

    target_include_directories_for(${TARGET_DEVICE} ${EXECUTABLE}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        PRIVATE ${BSP_INCLUDE_DIR}
    )

    target_link_libraries_for(${TARGET_DEVICE} ${EXECUTABLE} PRIVATE
        spi_app_bsp
        core
        w25q
    )

    target_preprocess_for(${TARGET_DEVICE} ${EXECUTABLE} ${LINKER_SCRIPT} ${LDF} ${PREPROCESS_DEFS})

endif()