FILE(GLOB SRC_FILES *.c)

add_library(st_h7_hal STATIC
    ${SRC_FILES}
)

target_link_libraries(st_h7_hal PUBLIC
    st_h7_support
)
target_include_directories(st_h7_hal PUBLIC .)