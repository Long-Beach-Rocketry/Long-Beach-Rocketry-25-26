add_library(logger STATIC
    logger.cc
)

target_sources(logger INTERFACE
    logger.cc
)

target_include_directories(logger PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(logger PUBLIC
    core
    # bus
    driver
)