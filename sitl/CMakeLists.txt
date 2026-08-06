add_library(sitl_core INTERFACE)

add_subdirectory(fake_drivers)
add_subdirectory(inverse_drivers)
# add_subdirectory(models)

target_link_libraries(sitl_core INTERFACE fake_drivers inverse_drivers)