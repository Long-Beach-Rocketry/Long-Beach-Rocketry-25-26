add_library(cli_manager STATIC
    cli.cc
)

target_include_directories(cli_manager PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(cli_manager PUBLIC
    container core cli_commands
)