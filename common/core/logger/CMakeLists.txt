add_library(logger STATIC
    logger.cc
)

target_sources(logger INTERFACE
    logger.cc
)

target_include_directories(logger INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(logger INTERFACE
    driver
    bus
    logger
)