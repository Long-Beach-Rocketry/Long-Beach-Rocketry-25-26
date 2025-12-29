set(EXECUTABLE w25q_test)
set(LDF ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}.ld)

set(PREPROCESS_DEFS
	-DDEF_FLASH_START_ADDR=0x8000000
	-DDEF_FLASH_SIZE=1024K
)

add_executable_for(${TARGET_DEVICE} ${EXECUTABLE} ${LDF}
	main.cc
	${STARTUP_FILE}
)

target_include_directories_for(${TARGET_DEVICE} ${EXECUTABLE}
	PRIVATE ${CMAKE_SOURCE_DIR}/app/spi_test
	PRIVATE ${CMAKE_SOURCE_DIR}/app/spi_test/BSP_L476
	PRIVATE ${CMAKE_SOURCE_DIR}/common/core/periph/flash_chip
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries_for(${TARGET_DEVICE} ${EXECUTABLE} PRIVATE
    spi_app_bsp
	core
	w25q
)

target_preprocess_for(${TARGET_DEVICE} ${EXECUTABLE} ${LINKER_SCRIPT} ${LDF} ${PREPROCESS_DEFS})

