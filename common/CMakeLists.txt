add_library(core INTERFACE)

add_subdirectory(drivers)
add_subdirectory(core)
target_link_libraries(core INTERFACE driver)
