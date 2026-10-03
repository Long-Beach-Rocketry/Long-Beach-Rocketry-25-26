add_library(cli INTERFACE
)


target_include_directories(cli INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/cli_core
    ${CMAKE_CURRENT_SOURCE_DIR}/cli_commands 
    ${CMAKE_CURRENT_SOURCE_DIR}/cli_manager 
)
add_subdirectory(cli_core)
add_subdirectory(cli_commands)
add_subdirectory(cli_manager)


target_link_libraries(cli INTERFACE
   cli_commands cli_manager
)