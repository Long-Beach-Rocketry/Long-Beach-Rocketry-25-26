FILE(GLOB SRC_FILES *.c)

add_library(st_f4_hal STATIC
    ${SRC_FILES}
)

target_link_libraries(st_f4_hal PUBLIC
    st_f4_support
)
target_include_directories(st_f4_hal PUBLIC .)