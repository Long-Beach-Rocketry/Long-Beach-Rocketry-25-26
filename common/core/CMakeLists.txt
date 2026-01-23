add_subdirectory(periph)
add_subdirectory(utils)
add_subdirectory(container)

# Make core consumers also get utils and chip_select by adding them to the INTERFACE core target
# `core` is defined in the parent `common/CMakeLists.txt` as an INTERFACE target.
if (TARGET core)
<<<<<<< HEAD
	target_link_libraries(core INTERFACE utils chip_select bmp390 container)
=======
	target_link_libraries(core INTERFACE utils chip_select bmp390 bno055)
>>>>>>> 8be3386 (refactor folders for bno055)
endif()
