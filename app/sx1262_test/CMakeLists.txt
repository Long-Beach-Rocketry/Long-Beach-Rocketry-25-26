add_subdirectory_for(STM32H723 bsp_h723)

if (TARGET sx1262_test_bsp)
    set(PREPROCESS_DEFS
        -DDEF_FLASH_START_ADDR=0x8000000
        -DDEF_FLASH_SIZE=1024K
    )

    foreach(TEST_MODE transmit receive)
        set(EXECUTABLE sx1262_${TEST_MODE}_test)
        set(LDF ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}.ld)

        add_executable_for(${TARGET_DEVICE} ${EXECUTABLE} ${LDF}
            main_${TEST_MODE}.cc
            ${STARTUP_FILE}
        )

        target_include_directories_for(${TARGET_DEVICE} ${EXECUTABLE} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
        )

        target_link_options_for(${TARGET_DEVICE} ${EXECUTABLE} PRIVATE
            --specs=nano.specs
            -Wl,--gc-sections
        )

        target_link_libraries_for(${TARGET_DEVICE} ${EXECUTABLE} PRIVATE
            sx1262_test_bsp
            sx1262
        )

        target_preprocess_for(${TARGET_DEVICE} ${EXECUTABLE}
            ${LINKER_SCRIPT} ${LDF} ${PREPROCESS_DEFS}
        )
    endforeach()
endif()