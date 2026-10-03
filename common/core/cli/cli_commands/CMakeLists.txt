add_library(cli_commands STATIC
   blink_cmd.cc
   sensor_cmd.cc
)

target_include_directories(cli_commands PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../cli_core
)

target_link_libraries(cli_commands PUBLIC
   driver
   utils
)