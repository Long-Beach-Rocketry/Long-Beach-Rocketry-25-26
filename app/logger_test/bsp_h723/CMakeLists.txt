add_library(logger_app_bsp STATIC
    logger_app_bsp.cc
)

target_include_directories(logger_app_bsp PUBLIC
    .
    ../
)

target_link_libraries(logger_app_bsp
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
        # driver
        # core
        # bus
)