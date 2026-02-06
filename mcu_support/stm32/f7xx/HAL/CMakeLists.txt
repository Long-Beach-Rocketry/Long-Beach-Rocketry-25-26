FILE(GLOB SRC_FILES *.c)

add_library(st_f7_hal STATIC
    ${SRC_FILES}
)

target_link_libraries(st_f7_hal PUBLIC
    st_f7_support
)
target_include_directories(st_f7_hal PUBLIC .)