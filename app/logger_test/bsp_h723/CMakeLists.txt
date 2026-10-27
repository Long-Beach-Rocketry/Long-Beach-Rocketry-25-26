add_library(logger_app_bsp STATIC
    logger_app_bsp.cc
)

target_include_directories(logger_app_bsp PUBLIC
    .
    ../
)

target_link_libraries(logger_app_bsp
    PUBLIC
        driver
        core
        bus
)