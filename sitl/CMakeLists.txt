add_library(sitl_core INTERFACE)

add_subdirectory(drivers/fake)
add_subdirectory(drivers/inverse)

target_link_libraries(sitl_core INTERFACE fake inverse)