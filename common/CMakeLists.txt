add_library(core INTERFACE)


target_link_libraries(core INTERFACE driver)
add_subdirectory(drivers)
