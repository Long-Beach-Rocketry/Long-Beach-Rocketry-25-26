set(EXECUTABLE logger_app_test)
set(LDF ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}.ld)

set(PREPROCESS_DEFS
    -DDEF_FLASH_START_ADDR=0x8000000
    -DDEF_FLASH_SIZE=1024K
)

add_subdirectory_for(STM32H723 bsp_h723)

if (TARGET logger_app_bsp)

    add_executable_for(${TARGET_DEVICE} ${EXECUTABLE} ${LDF}
        main.cc
        ${STARTUP_FILE}
    )

    target_include_directories_for(${TARGET_DEVICE} ${EXECUTABLE}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/common/core/logger

    )

    target_link_options_for(${TARGET_DEVICE} ${EXECUTABLE} PRIVATE
        --specs=nano.specs
        -Wl,--gc-sections
    )
    target_link_libraries_for(${TARGET_DEVICE} ${EXECUTABLE} PRIVATE
        logger
    )

    if (TARGET logger_app_bsp)
        target_link_libraries_for(${TARGET_DEVICE} ${EXECUTABLE} PRIVATE
            logger_app_bsp
            logger

        )
    endif()

    target_preprocess_for(${TARGET_DEVICE} ${EXECUTABLE} ${LINKER_SCRIPT} ${LDF} ${PREPROCESS_DEFS})

endif()