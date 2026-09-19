set(LIB rs485_app_bsp)

add_library_for(${TARGET_DEVICE} ${LIB} STATIC
    rs485_app_bsp.cc
)

# Expose the parent directory (where board.h lives) to this library
target_include_directories_for(${TARGET_DEVICE} ${LIB} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries_for(${TARGET_DEVICE} ${LIB} PRIVATE
    driver
    rs485
)