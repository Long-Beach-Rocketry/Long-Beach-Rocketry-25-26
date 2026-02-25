set(EXECUTABLE usart_app)
set(LDF ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}.ld)

set(PREPROCESS_DEFS
    -DDEF_FLASH_START_ADDR=0x8000000
    -DDEF_FLASH_SIZE=1024K
)

add_subdirectory(pb_helper)
add_subdirectory_for(STM32L476 BSP_L476)

if (TARGET usart_app_bsp)

    add_executable_for(${TARGET_DEVICE} ${EXECUTABLE} ${LDF}
        #main.cc remove this when not testing
        main_pb_test.cc 
        ${STARTUP_FILE}
    )

    target_include_directories_for(${TARGET_DEVICE} ${EXECUTABLE}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/BSP_L476
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/pb_helper
        PRIVATE ${CMAKE_SOURCE_DIR}/external/nanopb
    )

    target_link_libraries_for(${TARGET_DEVICE} ${EXECUTABLE} PRIVATE
        usart_app_bsp
    )

    target_preprocess_for(${TARGET_DEVICE} ${EXECUTABLE} ${LINKER_SCRIPT} ${LDF} ${PREPROCESS_DEFS})

    # This is for protobuff specific linking, which is currently only nanopb.
    if (TARGET ${EXECUTABLE})
        target_link_libraries(${EXECUTABLE} PRIVATE cmd_msg pb_helper protobuff)
        target_sources(${EXECUTABLE} PRIVATE
            ${CMAKE_SOURCE_DIR}/external/nanopb/pb_common.c
            ${CMAKE_SOURCE_DIR}/external/nanopb/pb_encode.c
            ${CMAKE_SOURCE_DIR}/external/nanopb/pb_decode.c
        )
        target_include_directories(${EXECUTABLE} PRIVATE
            ${CMAKE_SOURCE_DIR}/external/nanopb
        )
    endif()

endif()