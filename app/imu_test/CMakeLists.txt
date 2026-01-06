set(EXECUTABLE imu_test)
set(LDF ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}.ld)

set(PREPROCESS_DEFS
	-DDEF_FLASH_START_ADDR=0x8000000
	-DDEF_FLASH_SIZE=1024K
)

# Only add BSP for this target device
add_subdirectory_for(STM32L476 bsp_l476)

if (TARGET bsp_sensors)

	add_executable_for(${TARGET_DEVICE} ${EXECUTABLE} ${LDF}
		main.cc
		${STARTUP_FILE}
	)

	target_include_directories_for(${TARGET_DEVICE} ${EXECUTABLE}
		PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
	)

	target_link_libraries_for(${TARGET_DEVICE} ${EXECUTABLE} PRIVATE
		bsp_sensors
	)

	target_preprocess_for(${TARGET_DEVICE} ${EXECUTABLE} ${LINKER_SCRIPT} ${LDF} ${PREPROCESS_DEFS})

endif()