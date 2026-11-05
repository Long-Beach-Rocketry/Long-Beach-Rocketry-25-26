add_library(sitl_core INTERFACE)

add_subdirectory(drivers/fake)
add_subdirectory(drivers/inverse)
# Other dynamics and sensor models will join this target as they are built.

target_link_libraries(sitl_core INTERFACE fake inverse sitl_noise)
