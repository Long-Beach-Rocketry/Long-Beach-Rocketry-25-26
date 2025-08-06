FILE(GLOB SRC_FILES *.c)

add_library(st_l4_hal STATIC
    ${SRC_FILES}
)

target_link_libraries(st_l4_hal PUBLIC
    st_l4_support
)
target_include_directories(st_l4_hal PUBLIC .)