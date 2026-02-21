<<<<<<< HEAD
add_library(core INTERFACE)

add_subdirectory(drivers)
add_subdirectory(core)
target_link_libraries(core INTERFACE driver)
=======
add_library(core INTERFACE)

target_link_libraries(core INTERFACE driver)
add_subdirectory(drivers)
add_subdirectory(core)
>>>>>>> 319d772 (ekf)
