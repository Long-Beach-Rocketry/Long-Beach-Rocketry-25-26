set(EXECUTABLE kalman_test)
set(LDF ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}.ld)

set(PREPROCESS_DEFS
	-DDEF_FLASH_START_ADDR=0x8000000
	-DDEF_FLASH_SIZE=1024K
)
find_package(Eigen3 REQUIRED)

# Only add BSP for this target device
add_subdirectory_for(STM32H723 bsp_h723)

if (TARGET kalman_bsp)

	add_executable_for(${TARGET_DEVICE} ${EXECUTABLE} ${LDF}
		main.cc
		${CMAKE_SOURCE_DIR}/common/core/kalman_filter/kalman.cc
		${CMAKE_SOURCE_DIR}/common/core/math/altitude.cc
		${STARTUP_FILE}
	)
	
	target_compile_options(${EXECUTABLE} PRIVATE -O3)

	target_include_directories_for(${TARGET_DEVICE} ${EXECUTABLE}
		PRIVATE		${CMAKE_CURRENT_SOURCE_DIR}
                	${CMAKE_SOURCE_DIR}/common/core/kalman_filter
					${CMAKE_SOURCE_DIR}/common/core/math
	)

	target_link_options_for(${TARGET_DEVICE} ${EXECUTABLE} PRIVATE
        --specs=rdimon.specs
        -Wl,--gc-sections
    )

	target_link_libraries_for(${TARGET_DEVICE} ${EXECUTABLE} PRIVATE
		kalman_bsp
        Eigen3::Eigen
	)

	target_preprocess_for(${TARGET_DEVICE} ${EXECUTABLE} ${LINKER_SCRIPT} ${LDF} ${PREPROCESS_DEFS})

endif()