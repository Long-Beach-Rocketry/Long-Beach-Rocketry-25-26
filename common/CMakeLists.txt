add_library(core INTERFACE)

target_link_libraries(core INTERFACE driver chip_select)
add_subdirectory(drivers)
add_subdirectory(core)